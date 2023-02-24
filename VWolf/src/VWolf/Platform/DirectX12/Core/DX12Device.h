#pragma once
#include "VWolf/Core/Base.h"

namespace VWolf {
	class DX12Command;

	class DX12Device {
	public:
		DX12Device(Microsoft::WRL::ComPtr<IDXGIAdapter4> currentAdapter, D3D_FEATURE_LEVEL maximum_level);
		~DX12Device();
	public:
		Ref<DX12Command> CreateCommand(
			UINT numCommandFrames,
			D3D12_COMMAND_LIST_TYPE type = D3D12_COMMAND_LIST_TYPE_DIRECT,
			D3D12_COMMAND_QUEUE_FLAGS flags = D3D12_COMMAND_QUEUE_FLAG_NONE);
	public:
		const Microsoft::WRL::ComPtr<ID3D12Device8>& GetDevice() { return device; }
		const UINT GetMSAAQuality() { return m4xMsaaQuality; }
	private:
		void QueryMSAAQuality(UINT sampleCount, D3D12_MULTISAMPLE_QUALITY_LEVEL_FLAGS flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE);
	private:
		DXGI_ADAPTER_DESC3 adapterDescription {};
		Microsoft::WRL::ComPtr<ID3D12Device8> device;

		UINT m4xMsaaQuality = 0;

		// TODO: Not sure if this belongs here
		DXGI_FORMAT mDepthStencilFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
		DXGI_FORMAT mBackBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
	};
}
