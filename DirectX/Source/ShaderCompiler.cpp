#include "ShaderCompiler.h"
#include "Debug.h"

template<class T>
using ComPtr = DirectXHelper::ComPtr<T>;

void ShaderCompiler::Initalize() {
	ShaderCompiler::GetInstance()->InternalInitalize();
}

ComPtr<IDxcBlob> ShaderCompiler::Compile(const std::wstring& fileName, const std::wstring& entryPoint, const std::wstring& profile) {
    return GetInstance()->InternalCompile(fileName, entryPoint, profile);
}

ShaderCompiler* ShaderCompiler::GetInstance() {
    static ShaderCompiler instance;
    return &instance;
}

void ShaderCompiler::InternalInitalize() {
	assert(!utils_);
	assert(!compiler_);

	CHECK_HRESULT(DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&utils_)));
	CHECK_HRESULT(DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&compiler_)));
	CHECK_HRESULT(utils_->CreateDefaultIncludeHandler(&includeHandler_));
}

ComPtr<IDxcBlob> ShaderCompiler::InternalCompile(const std::wstring& fileName, const std::wstring& entryPoint, const std::wstring& profile) {
	assert(utils_);
	assert(compiler_);
	assert(!fileName.empty());

	Debug::Log(std::format(L"Begin CompileShader, path:{}, profile:{}\n", fileName, profile));

	ComPtr<IDxcBlobEncoding> shaderSource = nullptr;
	CHECK_HRESULT(utils_->LoadFile(fileName.c_str(), nullptr, &shaderSource));

	DxcBuffer shaderSourceBuffer{};
	shaderSourceBuffer.Ptr = shaderSource->GetBufferPointer();
	shaderSourceBuffer.Size = shaderSource->GetBufferSize();
	shaderSourceBuffer.Encoding = DXC_CP_UTF8;

	LPCWSTR arguments[] = {
		fileName.c_str(),
		L"-E", entryPoint.c_str(),
		L"-T", profile.c_str(),
		L"-Zi", L"-Qembed_debug",
		L"-Od",
		L"-Zpr"
	};

	IDxcResult* shaderResult = nullptr;
	CHECK_HRESULT(compiler_->Compile(
		&shaderSourceBuffer,
		arguments,
		_countof(arguments),
		includeHandler_.Get(),
		IID_PPV_ARGS(&shaderResult)));

	ComPtr<IDxcBlobUtf8> shaderError = nullptr;
	shaderResult->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(&shaderError), nullptr);
	if (shaderError != nullptr && shaderError->GetStringLength() != 0) {
		Debug::Log(shaderError->GetStringPointer());
		assert(false);
	}

	ComPtr<IDxcBlob> shaderBlob = nullptr;
	CHECK_HRESULT(shaderResult->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&shaderBlob), nullptr));

	Debug::Log(std::format(L"Compile Succeeded, path:{}, profile:{}\n", fileName, profile));
	return shaderBlob;
}


