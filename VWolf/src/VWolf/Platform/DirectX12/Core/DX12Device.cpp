#include "vwpch.h"
#ifdef VWOLF_PLATFORM_WINDOWS
#include "DX12Device.h"
#include "DX12Core.h"
#include "DX12Command.h"

namespace VWolf {
	DX12Device::DX12Device(Microsoft::WRL::ComPtr<IDXGIAdapter4> currentAdapter, D3D_FEATURE_LEVEL maximum_level) {		
		DXThrowIfFailed(D3D12CreateDevice(currentAdapter.Get(), maximum_level, IID_PPV_ARGS(&device)));
		DXNAME_D3D12_OBJECT(device, "VWolf DirectX 12 Device");

		currentAdapter->GetDesc3(&adapterDescription);
		VWOLF_CORE_INFO("DX12 Vendor %d", adapterDescription.VendorId);
		VWOLF_CORE_INFO("DX12 Render %ls", adapterDescription.Description);
		VWOLF_CORE_INFO("DX12 Version %d", adapterDescription.Revision);

		VWOLF_CORE_INFO("DX12 Dedicated Video Memory %zu", adapterDescription.DedicatedVideoMemory);
		VWOLF_CORE_INFO("DX12  Dedicated System Memory %zu", adapterDescription.DedicatedSystemMemory);
		VWOLF_CORE_INFO("DX12  Shared System Memory %zu", adapterDescription.SharedSystemMemory);
		QueryMSAAQuality(4);
#if defined(_DEBUG)
		{
			Microsoft::WRL::ComPtr<ID3D12InfoQueue> info_queue;
			DXThrowIfFailed(device->QueryInterface(IID_PPV_ARGS(&info_queue)));
			info_queue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
			info_queue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, true);
			info_queue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);
			//info_queue->SetMuteDebugOutput(true);
			
			// Filtering certain warning messages that are not needed.
			D3D12_MESSAGE_CATEGORY cats[] = { D3D12_MESSAGE_CATEGORY_EXECUTION };
			D3D12_MESSAGE_SEVERITY sevs[] = { D3D12_MESSAGE_SEVERITY_WARNING };
			D3D12_MESSAGE_ID ids[] = { D3D12_MESSAGE_ID_CLEARRENDERTARGETVIEW_MISMATCHINGCLEARVALUE };

			D3D12_INFO_QUEUE_FILTER filter;
			memset(&filter, 0, sizeof(filter));

			// To set the type of messages to allow, 
			// set filter.AllowList as follows:
			filter.DenyList.NumCategories = _countof(cats);
			filter.DenyList.pCategoryList = cats;
			filter.DenyList.NumSeverities = _countof(sevs);
			filter.DenyList.pSeverityList = sevs;
			filter.DenyList.NumIDs = _countof(ids);
			filter.DenyList.pIDList = ids;
			//DXThrowIfFailed(info_queue->AddRetrievalFilterEntries(&filter));
			DXThrowIfFailed(info_queue->AddStorageFilterEntries(&filter));
		}
#endif
	}

	DX12Device::~DX12Device() {
#if defined(_DEBUG)
		{
			Microsoft::WRL::ComPtr<ID3D12InfoQueue> info_queue;
			DXThrowIfFailed(device->QueryInterface(IID_PPV_ARGS(&info_queue)));
			info_queue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, false);
			info_queue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, false);
			info_queue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, false);

			Microsoft::WRL::ComPtr<ID3D12DebugDevice2> debug_device;
			DXThrowIfFailed(device->QueryInterface(IID_PPV_ARGS(&debug_device)));
			DXThrowIfFailed(debug_device->ReportLiveDeviceObjects(D3D12_RLDO_SUMMARY | D3D12_RLDO_DETAIL | D3D12_RLDO_IGNORE_INTERNAL));
		}
#endif

	}

	Ref<DX12Command> DX12Device::CreateCommand(
		UINT numCommandFrames,
		D3D12_COMMAND_LIST_TYPE type,
		D3D12_COMMAND_QUEUE_FLAGS flags) {
		return CreateRef<DX12Command>(device, numCommandFrames, type, flags);

	}

	void DX12Device::QueryMSAAQuality(UINT sampleCount, D3D12_MULTISAMPLE_QUALITY_LEVEL_FLAGS flags) {
		// Check 4X MSAA quality support for our back buffer format.
		// All Direct3D 11 capable devices support 4X MSAA for all render 
		// target formats, so we only need to check quality support.
		D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS msQualityLevels;
		msQualityLevels.Format = mBackBufferFormat;
		msQualityLevels.SampleCount = sampleCount;
		msQualityLevels.Flags = flags;
		msQualityLevels.NumQualityLevels = 0;
		DXThrowIfFailed(device->CheckFeatureSupport(
			D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS,
			&msQualityLevels,
			sizeof(msQualityLevels)));
		m4xMsaaQuality = msQualityLevels.NumQualityLevels;
	}
}

#endif