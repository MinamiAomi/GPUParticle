#pragma once
#include "DirectXHelper.h"

class ShaderCompiler {
public:
	static void Initalize();
	static DirectXHelper::ComPtr<IDxcBlob> Compile(const std::wstring& fileName, const std::wstring& entryPoint, const std::wstring& profile);

private:
	static ShaderCompiler* GetInstance();
	void InternalInitalize();
	DirectXHelper::ComPtr<IDxcBlob> InternalCompile(const std::wstring& fileName, const std::wstring& entryPoint, const std::wstring& profile);


	DirectXHelper::ComPtr<IDxcUtils> utils_;
	DirectXHelper::ComPtr<IDxcCompiler3> compiler_;
	DirectXHelper::ComPtr<IDxcIncludeHandler> includeHandler_;
};

