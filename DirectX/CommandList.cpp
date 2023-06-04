#include "stdafx.h"
#include "CommandList.h"
#include "Logger.h"

namespace DirectXHelper {

	bool CommandList::Initalize(ID3D12Device5* device, ID3D12CommandQueue* commandQueue) {
		// キューを割り当てる
		commandQueue_.Attach(commandQueue);
		// アロケータを生成
		if (FAILED(device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(commandAllocator_.ReleaseAndGetAddressOf())))) {
			Logger::Error("CreateCommandAllocator()");
			assert(false);
			return false;
		}

		// リストを生成
		if (FAILED(device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocator_.Get(), nullptr, IID_PPV_ARGS(commandList_.ReleaseAndGetAddressOf())))) {
			Logger::Error("CreateCommandList()");
			assert(false);
			return false;
		}
		// リストを閉じる
		if (FAILED(commandList_->Close())) {
			Logger::Error("ID3D12GraphicsCommandList4::Close()");
			assert(false);
			return false;
		}
		// フェンスを初期化
		if (!fence_.Initalize(device)) {
			return false;
		}
		isRecordingCommands = false;
		return true;
	}

	bool CommandList::Execute() {
		assert(isRecordingCommands);
		// リストを閉じる
		if (FAILED(commandList_->Close())) {
			Logger::Error("ID3D12GraphicsCommandList4::Close()");
			assert(false);
			return false;
		}
		// コマンドを実行
		ID3D12CommandList* cmdList[] = { commandList_.Get() };
		commandQueue_->ExecuteCommandLists(_countof(cmdList), cmdList);
		// シグナルを送る
		if (!fence_.Signal(commandQueue_.Get())) {
			return false;
		}
		isRecordingCommands = false;
		return true;
	}

	bool CommandList::Wait() {
		assert(!isRecordingCommands);
		if (!fence_.WaitForGPU()) {
			return false;
		}
		return true;
	}

	bool CommandList::Reset() {
		assert(!isRecordingCommands);
		if (FAILED(commandAllocator_->Reset())) {
			Logger::Error("ID3D12CommandAllocator::Reset()");
			assert(false);
			return false;
		}
		if (FAILED(commandList_->Reset(commandAllocator_.Get(), nullptr))) {
			Logger::Error("ID3D12GraphicsCommandList4::Reset()");
			assert(false);
			return false;
		}
		isRecordingCommands = true;
		return true;
	}
}
