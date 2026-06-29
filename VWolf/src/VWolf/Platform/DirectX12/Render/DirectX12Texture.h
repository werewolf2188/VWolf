#pragma once

#include "VWolf/Platform/PTexture.h"
#include "VWolf/Core/Math/VMath.h"
#ifdef VWOLF_PLATFORM_WINDOWS
namespace VWolf {
	class DX12RenderTargetResource;
	class DX12TextureResource;
	class DX12BufferResource;

	class DirectX12Texture2D : public PTexture2D {
	public:
		DirectX12Texture2D(void* bytes, uint32_t width, uint32_t height, TextureOptions options = {});
		virtual ~DirectX12Texture2D();
		virtual void* GetHandler() override;
		void Initialize(uint32_t width, uint32_t height, DXGI_FORMAT format, TextureOptions options = {});
	private:
		Ref<DX12TextureResource> m_texture;
		Ref<DX12BufferResource> m_textureUpload;
		bool hasBeenUpload = false;
		size_t numBytes = 0, rowBytes = 0, numRows = 0;
	};

	class DirectX12RenderTexture : public PRenderTexture {
	public:
		DirectX12RenderTexture(uint32_t width, uint32_t height, bool isDepthOnly = false, TextureOptions options = {});
		virtual ~DirectX12RenderTexture();

		virtual void* GetHandler() override;
		virtual void Resize(uint32_t width, uint32_t height) override;
	public:
		void Initialize();
		void Bind();
		void Transition(D3D12_RESOURCE_STATES);
		Ref<DX12RenderTargetResource> GetTexture() const { return rtvTexture; }
	private:
		Ref<DX12RenderTargetResource> rtvTexture;
		D3D12_VIEWPORT screenViewport;
		D3D12_RECT scissorRect;
		bool isDepthOnly = false;
	};

	class DirectX12Cubemap : public PCubemap {
	public:
		DirectX12Cubemap(TextureDefault textureDefault, uint32_t size, TextureOptions options = {});
        DirectX12Cubemap(std::filesystem::path path, TextureOptions options = {});

		DirectX12Cubemap(std::array<void*, 6> bytes, uint32_t size, TextureOptions options = {});
		virtual ~DirectX12Cubemap();
		virtual void* GetHandler() override;
		void Initialize(uint32_t size, DXGI_FORMAT format, TextureOptions options = {});
	private:
		Ref<DX12TextureResource> m_texture;
		Ref<DX12BufferResource> m_textureUpload;
		bool hasBeenUpload = false;
		size_t numBytes = 0, rowBytes = 0, numRows = 0;
		uint32_t numberOfSides = 6;
	};
}
#endif
