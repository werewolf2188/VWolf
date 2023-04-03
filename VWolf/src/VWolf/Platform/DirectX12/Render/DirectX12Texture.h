#pragma once

#include "VWolf/Core/Render/Texture.h"
#ifdef VWOLF_PLATFORM_WINDOWS
namespace VWolf {
	class DX12RenderTargetResource;
	class DX12TextureResource;
	class DX12BufferResource;

	class DirectX12Texture2D : public Texture2D {
	public:
		DirectX12Texture2D(uint32_t width, uint32_t height, TextureOptions options = {});
		DirectX12Texture2D(const std::string filePath, TextureOptions options = {});
		virtual ~DirectX12Texture2D();
		virtual void* GetHandler() override;
#if defined(DEBUG) || defined(_DEBUG)
	private:
		void* PopulateTest();
#endif
		void Initialize(uint32_t width, uint32_t height, DXGI_FORMAT format, TextureOptions options = {});
	private:
		Ref<DX12TextureResource> m_texture;
		Ref<DX12BufferResource> m_textureUpload;
		bool hasBeenUpload = false;
		void* m_data = nullptr;
		size_t numBytes = 0, rowBytes = 0, numRows = 0;
	};

	class DirectX12RenderTexture : public RenderTexture {
	public:
		DirectX12RenderTexture(uint32_t width, uint32_t height, TextureOptions options = {});
		virtual ~DirectX12RenderTexture();

		virtual void* GetHandler() override;

		virtual void Resize(uint32_t width, uint32_t height) override;
	public:
		void Initialize();
		void Bind();
		Ref<DX12RenderTargetResource> GetTexture() const { return rtvTexture; }
	private:
		Ref<DX12RenderTargetResource> rtvTexture;
	};
}
#endif