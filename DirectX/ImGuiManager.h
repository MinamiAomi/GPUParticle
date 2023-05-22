#pragma once
#include "DirectXHelper.h"

class ImGuiManager {
public:
	void Initalize(ID3D12Device5* device, const DirectXHelper::Descriptor& descriptor);
	void NewFrame();
	void Render(ID3D12GraphicsCommandList4* cmdList);
	void Finalize();

private:
	DirectXHelper::Descriptor descriptor_;
};

