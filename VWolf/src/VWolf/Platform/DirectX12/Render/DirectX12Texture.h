#pragma once

#include "VWolf/Core/Render/Texture.h"
#include "VWolf/Core/Math/VMath.h" 
#ifdef VWOLF_PLATFORM_WINDOWS
namespace VWolf {
	class DX12RenderTargetResource;
	class DX12TextureResource;
	class DX12BufferResource;

	class DirectX12Texture2D : public Texture2D {
	public:
		DirectX12Texture2D(TextureDefault textureDefault, uint32_t width, uint32_t height, TextureOptions options = {});
		DirectX12Texture2D(const std::string filePath, TextureOptions options = {});
		virtual ~DirectX12Texture2D();
		virtual void* GetHandler() override;
	protected:
		virtual void PopulateColor() override;
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

	class DirectX12Cubemap : public Cubemap {
	public:
		DirectX12Cubemap(TextureDefault textureDefault, uint32_t size, TextureOptions options = {});
		DirectX12Cubemap(std::array<std::string, 6> paths, TextureOptions options = {});
		virtual ~DirectX12Cubemap();
		virtual void* GetHandler() override;
	protected:
		virtual void PopulateColor() override;
#if defined(DEBUG) || defined(_DEBUG)
	private:
		//void PopulateTest(GLuint id, int checkIndex, Vector4Float otherColor);
		void PopulateTest();
		void* PopulateTest(int checkIndex, Vector4Float otherColor);
#endif
		void Initialize(uint32_t size, DXGI_FORMAT format, TextureOptions options = {});
	private:
		Ref<DX12TextureResource> m_texture;
		Ref<DX12BufferResource> m_textureUpload;
		bool hasBeenUpload = false;
		size_t numBytes = 0, rowBytes = 0, numRows = 0;
		std::array<void*, 6> m_data;
		uint32_t numberOfSides = 6;
	};
}
#endif