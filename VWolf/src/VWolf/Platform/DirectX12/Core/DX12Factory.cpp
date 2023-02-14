#include "vwpch.h"
#ifdef VWOLF_PLATFORM_WINDOWS
#include "DX12Factory.h"
#include "DX12Device.h"
#include "DX12Core.h"

constexpr D3D_FEATURE_LEVEL minimum_feature_level{ D3D_FEATURE_LEVEL_11_0 };

namespace VWolf {

	Microsoft::WRL::ComPtr<IDXGIFactory7> DX12Factory::m_factory = nullptr;
	Microsoft::WRL::ComPtr<IDXGIAdapter4> DX12Factory::m_currentAdapter = nullptr;
	D3D_FEATURE_LEVEL DX12Factory::maximum_feature_level = D3D_FEATURE_LEVEL_1_0_CORE; // The lowest you can have.

	D3D_FEATURE_LEVEL GetMaximumFeatureLevel(IDXGIAdapter4* adapter);

	bool DX12Factory::Initialize() {
		// Create Factory
		UINT dxgiFactoryFlags = 0;

#if defined(_DEBUG)
		// Create a Debug Controller to track errors
		{
			Microsoft::WRL::ComPtr<ID3D12Debug1> debugController;
			ID3D12Debug* dc;
			DXThrowIfFailedWithReturnValue(D3D12GetDebugInterface(IID_PPV_ARGS(&dc)), false);
			DXThrowIfFailedWithReturnValue(dc->QueryInterface(IID_PPV_ARGS(&debugController)), false);
			debugController->EnableDebugLayer();
			debugController->SetEnableGPUBasedValidation(true);

			dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;

			dc->Release();
			dc = nullptr;
		}
#endif

		DXThrowIfFailedWithReturnValue(CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&m_factory)), false);
		m_currentAdapter.Attach(GetHighestAdapter());		
		VWOLF_CORE_ASSERT(m_currentAdapter, "Adapter not found");
		maximum_feature_level = GetMaximumFeatureLevel(m_currentAdapter.Get());
		VWOLF_CORE_ASSERT(maximum_feature_level >= minimum_feature_level);
		if (maximum_feature_level < minimum_feature_level) return false;
		return true;
	}

	IDXGIAdapter4* DX12Factory::GetHighestAdapter() {
		IDXGIAdapter4* adapter = nullptr;
		IDXGIAdapter4* currentAdapter { nullptr };
		// Lets enumerate adapters
		for (UINT adapterIndex = 0; 
			m_factory->EnumAdapterByGpuPreference(adapterIndex, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&adapter)) != DXGI_ERROR_NOT_FOUND;
			adapterIndex++) {
			
			DXGI_ADAPTER_DESC3 desc;
			adapter->GetDesc3(&desc);

			// Don't select the Basic Render Driver adapter.
			if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
			{
				adapter->Release();
				continue;
			}
			else if (FAILED(D3D12CreateDevice(adapter, minimum_feature_level, __uuidof(ID3D12Device), nullptr))) {
				adapter->Release();
				continue;
			}			

			// It could iterate with the outputs inside the adapter

			if (adapterIndex == 0)
				currentAdapter = adapter;
			else 
				adapter->Release();
		}
		return currentAdapter;
	}

	D3D_FEATURE_LEVEL GetMaximumFeatureLevel(IDXGIAdapter4* adapter) {
		constexpr D3D_FEATURE_LEVEL featureLevels[4]{
			D3D_FEATURE_LEVEL_11_0,
			D3D_FEATURE_LEVEL_11_1,
			D3D_FEATURE_LEVEL_12_0,
			D3D_FEATURE_LEVEL_12_1
		};
		D3D12_FEATURE_DATA_FEATURE_LEVELS featureLevelsInfo { };
		featureLevelsInfo.NumFeatureLevels = _countof(featureLevels);
		featureLevelsInfo.pFeatureLevelsRequested = featureLevels;

		Microsoft::WRL::ComPtr<ID3D12Device> device;
		DXThrowIfFailedWithReturnValue(D3D12CreateDevice(adapter, minimum_feature_level, IID_PPV_ARGS(&device)), D3D_FEATURE_LEVEL_1_0_CORE);
		DXThrowIfFailedWithReturnValue(device->CheckFeatureSupport(D3D12_FEATURE_FEATURE_LEVELS, &featureLevelsInfo, sizeof(featureLevelsInfo)), D3D_FEATURE_LEVEL_1_0_CORE);
		return featureLevelsInfo.MaxSupportedFeatureLevel;
	}

	Ref<DX12Device> DX12Factory::CreateDevice() {
		return CreateRef<DX12Device>(m_currentAdapter, maximum_feature_level);
	}

	void DX12Factory::Shutdown() {

	}
}

#endif