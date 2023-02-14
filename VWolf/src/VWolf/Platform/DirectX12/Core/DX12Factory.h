#pragma once
#include "VWolf/Core/Base.h"

namespace VWolf {
	class DX12Device;

	class DX12Factory {
	public:
		static bool Initialize();
		static void Shutdown();

		static Ref<DX12Device> CreateDevice();
		static Microsoft::WRL::ComPtr<IDXGIFactory7>& GetFactory() { return m_factory; }
	private:
		static IDXGIAdapter4* GetHighestAdapter();
	private:
		static Microsoft::WRL::ComPtr<IDXGIFactory7> m_factory;
		static Microsoft::WRL::ComPtr<IDXGIAdapter4> m_currentAdapter;

		static D3D_FEATURE_LEVEL maximum_feature_level;
	};
}
