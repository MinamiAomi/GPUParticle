#include "stdafx.h"
#include "DX12Core.h"

namespace DirectXHelper {
	template<class T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;

	Core* DirectXHelper::Core::GetInstance() {
		static Core instance;
		return &instance;
	}

	bool Core::Initalize() {
		return GetInstance()->InternalInitalize();
	}

	bool Core::InternalInitalize() {
#ifdef _DEBUG	
		// デバッグ時のみ
		ComPtr<ID3D12Debug1> debugController;
		if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)))) {
			// デバッグレイヤーを有効化する
			debugController->EnableDebugLayer();
			// さらにGPU側でもチェックを行えるようにする
			debugController->SetEnableGPUBasedValidation(TRUE);
		}
#endif

		if (FAILED(CreateDXGIFactory(IID_PPV_ARGS(factory_.GetAddressOf())))) {
			OutputDebugStringA("FAILED!! : Create DXGIFactory\n");
			return false;
		}

		// 使用するアダプタ用の変数
		ComPtr<IDXGIAdapter4> useAdapter;

		// 良い順にアダプターを頼む
		for (uint32_t i = 0;
			factory_->EnumAdapterByGpuPreference(i, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&useAdapter)) != DXGI_ERROR_NOT_FOUND;
			++i) {
			// アダプター情報を取得
			DXGI_ADAPTER_DESC3 adapterDesc{};
			if (FAILED(useAdapter->GetDesc3(&adapterDesc))) {
				OutputDebugStringA("FAILED!! : Get adapter desc3\n");
			}

			// ソフトウェアアダプタでなければ採用
			if (!(adapterDesc.Flags & DXGI_ADAPTER_FLAG3_SOFTWARE)) {
				// 採用したアダプタ情報を出力
				OutputDebugStringW(std::format(L"Use Adapter:{}\n", adapterDesc.Description).c_str());
				break;
			}
			useAdapter = nullptr; // ソフトウェアアダプタは見なかったことにする
		}
		if (!useAdapter) {
			OutputDebugStringA("FAILED!! : Use adapter null\n");
			return false;
		}
		useAdapter.As(&adapter_);

		// 機能レベルとログ出力用の文字列
		D3D_FEATURE_LEVEL featureLevels[] = {
			D3D_FEATURE_LEVEL_12_2, D3D_FEATURE_LEVEL_12_1, D3D_FEATURE_LEVEL_12_0
		};
		const char* featureLevelStrings[] = { "12.2", "12.1", "12.0" };
		// 高い順に生成できるか試していく
		for (size_t i = 0; i < _countof(featureLevels); ++i) {
			// 採用したアダプターデバイスを生成
			// 指定した機能レベルでデバイスが生成できたかを確認
			if (SUCCEEDED(D3D12CreateDevice(useAdapter.Get(), featureLevels[i], IID_PPV_ARGS(device_.GetAddressOf())))) {
				// 生成できたのでログ出力を行ってループを抜ける
				OutputDebugStringA(std::format("FeatureLevel : {}\n", featureLevelStrings[i]).c_str());
				break;
			}
		}
		if (!device_) {
			OutputDebugStringA("FAILED!! : Create device\n");
			return false;
		}

#ifdef _DEBUG
		// デバッグ時のみ
		ComPtr<ID3D12InfoQueue> infoQueue;
		if (SUCCEEDED(device_->QueryInterface(IID_PPV_ARGS(&infoQueue)))) {
			// やばいエラーの時に止まる
			infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
			// エラーの時に止まる
			infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);
			// 警告時に止まる
			infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, true);
			// 抑制するメッセージのID
			D3D12_MESSAGE_ID denyIds[] = {
				D3D12_MESSAGE_ID_RESOURCE_BARRIER_MISMATCHING_COMMAND_LIST_TYPE
			};
			// 抑制するレベル
			D3D12_MESSAGE_SEVERITY severities[] = { D3D12_MESSAGE_SEVERITY_INFO };
			D3D12_INFO_QUEUE_FILTER filter{};
			filter.DenyList.NumIDs = _countof(denyIds);
			filter.DenyList.pIDList = denyIds;
			filter.DenyList.NumSeverities = _countof(severities);
			filter.DenyList.pSeverityList = severities;
			// 指定したメッセージの表示を抑制する
			infoQueue->PushStorageFilter(&filter);
		}
#endif
		return true;
	}

}