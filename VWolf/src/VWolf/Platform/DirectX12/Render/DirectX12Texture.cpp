#include "vwpch.h"

#ifdef VWOLF_PLATFORM_WINDOWS
#include "DirectX12Texture.h"

#include "stb_image/stb_image.h"

#include "VWolf/Platform/DirectX12/DirectX12Driver.h"

#include "VWolf/Platform/DirectX12/Core/DX12Core.h"
#include "VWolf/Platform/DirectX12/Core/DX12Device.h"
#include "VWolf/Platform/DirectX12/Core/DX12Command.h"
#include "VWolf/Platform/DirectX12/Core/DX12Resources.h"

namespace VWolf {
	//--------------------------------------------------------------------------------------
// Return the BPP for a particular format
//--------------------------------------------------------------------------------------
	static size_t BitsPerPixel(_In_ DXGI_FORMAT fmt)
	{
		switch (fmt)
		{
		case DXGI_FORMAT_R32G32B32A32_TYPELESS:
		case DXGI_FORMAT_R32G32B32A32_FLOAT:
		case DXGI_FORMAT_R32G32B32A32_UINT:
		case DXGI_FORMAT_R32G32B32A32_SINT:
			return 128;

		case DXGI_FORMAT_R32G32B32_TYPELESS:
		case DXGI_FORMAT_R32G32B32_FLOAT:
		case DXGI_FORMAT_R32G32B32_UINT:
		case DXGI_FORMAT_R32G32B32_SINT:
			return 96;

		case DXGI_FORMAT_R16G16B16A16_TYPELESS:
		case DXGI_FORMAT_R16G16B16A16_FLOAT:
		case DXGI_FORMAT_R16G16B16A16_UNORM:
		case DXGI_FORMAT_R16G16B16A16_UINT:
		case DXGI_FORMAT_R16G16B16A16_SNORM:
		case DXGI_FORMAT_R16G16B16A16_SINT:
		case DXGI_FORMAT_R32G32_TYPELESS:
		case DXGI_FORMAT_R32G32_FLOAT:
		case DXGI_FORMAT_R32G32_UINT:
		case DXGI_FORMAT_R32G32_SINT:
		case DXGI_FORMAT_R32G8X24_TYPELESS:
		case DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
		case DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS:
		case DXGI_FORMAT_X32_TYPELESS_G8X24_UINT:
		case DXGI_FORMAT_Y416:
		case DXGI_FORMAT_Y210:
		case DXGI_FORMAT_Y216:
			return 64;

		case DXGI_FORMAT_R10G10B10A2_TYPELESS:
		case DXGI_FORMAT_R10G10B10A2_UNORM:
		case DXGI_FORMAT_R10G10B10A2_UINT:
		case DXGI_FORMAT_R11G11B10_FLOAT:
		case DXGI_FORMAT_R8G8B8A8_TYPELESS:
		case DXGI_FORMAT_R8G8B8A8_UNORM:
		case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
		case DXGI_FORMAT_R8G8B8A8_UINT:
		case DXGI_FORMAT_R8G8B8A8_SNORM:
		case DXGI_FORMAT_R8G8B8A8_SINT:
		case DXGI_FORMAT_R16G16_TYPELESS:
		case DXGI_FORMAT_R16G16_FLOAT:
		case DXGI_FORMAT_R16G16_UNORM:
		case DXGI_FORMAT_R16G16_UINT:
		case DXGI_FORMAT_R16G16_SNORM:
		case DXGI_FORMAT_R16G16_SINT:
		case DXGI_FORMAT_R32_TYPELESS:
		case DXGI_FORMAT_D32_FLOAT:
		case DXGI_FORMAT_R32_FLOAT:
		case DXGI_FORMAT_R32_UINT:
		case DXGI_FORMAT_R32_SINT:
		case DXGI_FORMAT_R24G8_TYPELESS:
		case DXGI_FORMAT_D24_UNORM_S8_UINT:
		case DXGI_FORMAT_R24_UNORM_X8_TYPELESS:
		case DXGI_FORMAT_X24_TYPELESS_G8_UINT:
		case DXGI_FORMAT_R9G9B9E5_SHAREDEXP:
		case DXGI_FORMAT_R8G8_B8G8_UNORM:
		case DXGI_FORMAT_G8R8_G8B8_UNORM:
		case DXGI_FORMAT_B8G8R8A8_UNORM:
		case DXGI_FORMAT_B8G8R8X8_UNORM:
		case DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM:
		case DXGI_FORMAT_B8G8R8A8_TYPELESS:
		case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:
		case DXGI_FORMAT_B8G8R8X8_TYPELESS:
		case DXGI_FORMAT_B8G8R8X8_UNORM_SRGB:
		case DXGI_FORMAT_AYUV:
		case DXGI_FORMAT_Y410:
		case DXGI_FORMAT_YUY2:
			return 32;

		case DXGI_FORMAT_P010:
		case DXGI_FORMAT_P016:
			return 24;

		case DXGI_FORMAT_R8G8_TYPELESS:
		case DXGI_FORMAT_R8G8_UNORM:
		case DXGI_FORMAT_R8G8_UINT:
		case DXGI_FORMAT_R8G8_SNORM:
		case DXGI_FORMAT_R8G8_SINT:
		case DXGI_FORMAT_R16_TYPELESS:
		case DXGI_FORMAT_R16_FLOAT:
		case DXGI_FORMAT_D16_UNORM:
		case DXGI_FORMAT_R16_UNORM:
		case DXGI_FORMAT_R16_UINT:
		case DXGI_FORMAT_R16_SNORM:
		case DXGI_FORMAT_R16_SINT:
		case DXGI_FORMAT_B5G6R5_UNORM:
		case DXGI_FORMAT_B5G5R5A1_UNORM:
		case DXGI_FORMAT_A8P8:
		case DXGI_FORMAT_B4G4R4A4_UNORM:
			return 16;

		case DXGI_FORMAT_NV12:
		case DXGI_FORMAT_420_OPAQUE:
		case DXGI_FORMAT_NV11:
			return 12;

		case DXGI_FORMAT_R8_TYPELESS:
		case DXGI_FORMAT_R8_UNORM:
		case DXGI_FORMAT_R8_UINT:
		case DXGI_FORMAT_R8_SNORM:
		case DXGI_FORMAT_R8_SINT:
		case DXGI_FORMAT_A8_UNORM:
		case DXGI_FORMAT_AI44:
		case DXGI_FORMAT_IA44:
		case DXGI_FORMAT_P8:
			return 8;

		case DXGI_FORMAT_R1_UNORM:
			return 1;

		case DXGI_FORMAT_BC1_TYPELESS:
		case DXGI_FORMAT_BC1_UNORM:
		case DXGI_FORMAT_BC1_UNORM_SRGB:
		case DXGI_FORMAT_BC4_TYPELESS:
		case DXGI_FORMAT_BC4_UNORM:
		case DXGI_FORMAT_BC4_SNORM:
			return 4;

		case DXGI_FORMAT_BC2_TYPELESS:
		case DXGI_FORMAT_BC2_UNORM:
		case DXGI_FORMAT_BC2_UNORM_SRGB:
		case DXGI_FORMAT_BC3_TYPELESS:
		case DXGI_FORMAT_BC3_UNORM:
		case DXGI_FORMAT_BC3_UNORM_SRGB:
		case DXGI_FORMAT_BC5_TYPELESS:
		case DXGI_FORMAT_BC5_UNORM:
		case DXGI_FORMAT_BC5_SNORM:
		case DXGI_FORMAT_BC6H_TYPELESS:
		case DXGI_FORMAT_BC6H_UF16:
		case DXGI_FORMAT_BC6H_SF16:
		case DXGI_FORMAT_BC7_TYPELESS:
		case DXGI_FORMAT_BC7_UNORM:
		case DXGI_FORMAT_BC7_UNORM_SRGB:
			return 8;

		default:
			return 0;
		}
	}

	static void GetSurfaceInfo(_In_ size_t width,
		_In_ size_t height,
		_In_ DXGI_FORMAT fmt,
		_Out_opt_ size_t* outNumBytes,
		_Out_opt_ size_t* outRowBytes,
		_Out_opt_ size_t* outNumRows)
	{
		size_t numBytes = 0;
		size_t rowBytes = 0;
		size_t numRows = 0;

		bool bc = false;
		bool packed = false;
		bool planar = false;
		size_t bpe = 0;
		switch (fmt)
		{
		case DXGI_FORMAT_BC1_TYPELESS:
		case DXGI_FORMAT_BC1_UNORM:
		case DXGI_FORMAT_BC1_UNORM_SRGB:
		case DXGI_FORMAT_BC4_TYPELESS:
		case DXGI_FORMAT_BC4_UNORM:
		case DXGI_FORMAT_BC4_SNORM:
			bc = true;
			bpe = 8;
			break;

		case DXGI_FORMAT_BC2_TYPELESS:
		case DXGI_FORMAT_BC2_UNORM:
		case DXGI_FORMAT_BC2_UNORM_SRGB:
		case DXGI_FORMAT_BC3_TYPELESS:
		case DXGI_FORMAT_BC3_UNORM:
		case DXGI_FORMAT_BC3_UNORM_SRGB:
		case DXGI_FORMAT_BC5_TYPELESS:
		case DXGI_FORMAT_BC5_UNORM:
		case DXGI_FORMAT_BC5_SNORM:
		case DXGI_FORMAT_BC6H_TYPELESS:
		case DXGI_FORMAT_BC6H_UF16:
		case DXGI_FORMAT_BC6H_SF16:
		case DXGI_FORMAT_BC7_TYPELESS:
		case DXGI_FORMAT_BC7_UNORM:
		case DXGI_FORMAT_BC7_UNORM_SRGB:
			bc = true;
			bpe = 16;
			break;

		case DXGI_FORMAT_R8G8_B8G8_UNORM:
		case DXGI_FORMAT_G8R8_G8B8_UNORM:
		case DXGI_FORMAT_YUY2:
			packed = true;
			bpe = 4;
			break;

		case DXGI_FORMAT_Y210:
		case DXGI_FORMAT_Y216:
			packed = true;
			bpe = 8;
			break;

		case DXGI_FORMAT_NV12:
		case DXGI_FORMAT_420_OPAQUE:
			planar = true;
			bpe = 2;
			break;

		case DXGI_FORMAT_P010:
		case DXGI_FORMAT_P016:
			planar = true;
			bpe = 4;
			break;
		}

		if (bc)
		{
			size_t numBlocksWide = 0;
			if (width > 0)
			{
				numBlocksWide = std::max<size_t>(1, (width + 3) / 4);
			}
			size_t numBlocksHigh = 0;
			if (height > 0)
			{
				numBlocksHigh = std::max<size_t>(1, (height + 3) / 4);
			}
			rowBytes = numBlocksWide * bpe;
			numRows = numBlocksHigh;
			numBytes = rowBytes * numBlocksHigh;
		}
		else if (packed)
		{
			rowBytes = ((width + 1) >> 1) * bpe;
			numRows = height;
			numBytes = rowBytes * height;
		}
		else if (fmt == DXGI_FORMAT_NV11)
		{
			rowBytes = ((width + 3) >> 2) * 4;
			numRows = height * 2; // Direct3D makes this simplifying assumption, although it is larger than the 4:1:1 data
			numBytes = rowBytes * numRows;
		}
		else if (planar)
		{
			rowBytes = ((width + 1) >> 1) * bpe;
			numBytes = (rowBytes * height) + ((rowBytes * height + 1) >> 1);
			numRows = height + ((height + 1) >> 1);
		}
		else
		{
			size_t bpp = BitsPerPixel(fmt);
			rowBytes = (width * bpp + 7) / 8; // round up to nearest byte
			numRows = height;
			numBytes = rowBytes * height;
		}

		if (outNumBytes)
		{
			*outNumBytes = numBytes;
		}
		if (outRowBytes)
		{
			*outRowBytes = rowBytes;
		}
		if (outNumRows)
		{
			*outNumRows = numRows;
		}
	}

	Color Transform(TextureDefault textureDefault) {
		switch (textureDefault) {
		case TextureDefault::White: return Color(1, 1, 1, 1);
		case TextureDefault::Bump: return Color(0.5f, 0.5f, 1, 0.5f);
		case TextureDefault::Black: return Color(0, 0, 0, 1);
		case TextureDefault::Gray: return Color(0.5f, 0.5f, 0.5f, 1);
		case TextureDefault::Red: return Color(1, 0, 0, 1);
		}
	}

	DirectX12Texture2D::DirectX12Texture2D(TextureDefault textureDefault, uint32_t width, uint32_t height, TextureOptions options): Texture2D(textureDefault, width, height, options)
	{
		Initialize(width, height, DXGI_FORMAT_R32G32B32A32_FLOAT, options);
		PopulateColor();
		GetSurfaceInfo(width, height, DXGI_FORMAT_R32G32B32A32_FLOAT, &numBytes, &rowBytes, &numRows);
	}

	DirectX12Texture2D::DirectX12Texture2D(const std::string filePath, TextureOptions options): Texture2D(filePath, options)
	{
		int channels, width, height;
		auto img = stbi_load(filePath.c_str(), &width, &height, &channels, 0);
		m_width = width;
		m_height = height;
		DXGI_FORMAT format = DXGI_FORMAT_R32G32B32A32_FLOAT;

		if (channels == 4)
		{
			format = DXGI_FORMAT_R8G8B8A8_UNORM; // GL_RGBA8;
		}
		// TODO: What to do when there are only 3 channels?
		//else if (channels == 3)
		//{
		//	format = DXGI_FORMAT_R32G32B32_UINT; // GL_RGB8;
		//}
		Initialize(width, height, format, options);
		m_data = img;
		GetSurfaceInfo(width, height, format, &numBytes, &rowBytes, &numRows);
	}

	DirectX12Texture2D::~DirectX12Texture2D()
	{
		free(m_data);
	}

	void DirectX12Texture2D::PopulateColor() {
		size_t size = sizeof(Vector4Float) * m_width * m_height;
		Vector4Float* data = (Vector4Float*)malloc(size);
		memset(data, 0, size);
		uint32_t index = 0;
		Vector4Float value = Transform(m_textureDefault);
		for (uint32_t column = 0; column < m_height; column++) {			
			for (uint32_t row = 0; row < m_width; row++) {
				index = (column * m_height) + row;
				data[index] = value;
			}
		}
		m_data = data;
	}

#if defined(DEBUG) || defined(_DEBUG)
	void* DirectX12Texture2D::PopulateTest() {
		size_t size = sizeof(Vector4Float) * m_width * m_height;
		Vector4Float* data = (Vector4Float*)malloc(size);
		memset(data, 0, size);
		uint32_t index = 0;
		Vector4Float black(0, 0, 0, 1);
		Vector4Float white(1, 1, 1, 1);
		Vector4Float value = white;
		for (uint32_t column = 0; column < m_height; column++) {
			if (column % 32 == 0) {
				if (value.r == 1)
					value = black;
				else if (value.r == 0)
					value = white;
			}
			for (uint32_t row = 0; row < m_width; row++) {
				if (row % 32 == 0) {
					if (value.r == 1)
						value = black;
					else if (value.r == 0)
						value = white;
				}
				index = (column * m_height) + row;
				data[index] = value;
			}
		}
		return data;
	}
	
#endif

	void DirectX12Texture2D::Initialize(uint32_t width, uint32_t height, DXGI_FORMAT format, TextureOptions options)
	{
		DX12TextureResourceInfo info;
		info.newResourceDescription = new D3D12_RESOURCE_DESC;
		info.newResourceDescription->Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		info.newResourceDescription->Alignment = 0;
		info.newResourceDescription->Width = width;
		info.newResourceDescription->Height = height;
		info.newResourceDescription->DepthOrArraySize = 1;
		info.newResourceDescription->MipLevels = 1;
		info.newResourceDescription->Format = format;

		info.shaderResourceDescription = new D3D12_SHADER_RESOURCE_VIEW_DESC;
		info.shaderResourceDescription->Format = format;
		info.shaderResourceDescription->Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		info.shaderResourceDescription->ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		info.shaderResourceDescription->Texture2D.MipLevels = 1;
		info.shaderResourceDescription->Texture2D.MostDetailedMip = 0;
		info.shaderResourceDescription->Texture2D.PlaneSlice = 0;
		info.shaderResourceDescription->Texture2D.ResourceMinLODClamp = 0;

		info.newResourceDescription->SampleDesc.Count = 1; // device->GetMSAAQuality() ? 4 : 1;
		info.newResourceDescription->SampleDesc.Quality = 0; // device->GetMSAAQuality() ? (device->GetMSAAQuality() - 1) : 0;
		info.newResourceDescription->Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
		info.newResourceDescription->Flags = D3D12_RESOURCE_FLAG_NONE;

		info.newResourceState = D3D12_RESOURCE_STATE_COMMON; //D3D12_RESOURCE_STATE_COMMON;

		m_texture = CreateRef<DX12TextureResource>(format);
		m_texture->CreateTextureResource(info,
			DirectX12Driver::GetCurrent()->GetDevice(),
			DirectX12Driver::GetCurrent()->GetShaderResourceViewDescriptorHeap());

		DX12BufferResourceInfo bInfo;		
		bInfo.CreateBufferResourceDescription(true);
		bInfo.newResourceDescription->Width = sizeof(Vector4Float) * width * height;

		m_textureUpload = CreateRef<DX12BufferResource>(bInfo);		
		auto size = GetRequiredIntermediateSize(m_texture->GetResource().Get(), 0, 1);
		m_textureUpload->CreateBuffer(DirectX12Driver::GetCurrent()->GetDevice(), size);
	}

	void* DirectX12Texture2D::GetHandler()
	{
		if (!hasBeenUpload) {
			m_texture->TransitionResource(DirectX12Driver::GetCurrent()->GetCommands(),
				m_texture->GetCurrentState(), D3D12_RESOURCE_STATE_COPY_DEST);			
			D3D12_SUBRESOURCE_DATA data;
			data.pData = m_data;
			data.RowPitch = static_cast<UINT>(rowBytes);
			data.SlicePitch = static_cast<UINT>(numBytes);
			// TODO: I can't load anything beyond 512x512 Pixels
			UpdateSubresources<1>(DirectX12Driver::GetCurrent()->GetCommands()->GetCommandList().Get(), m_texture->GetResource().Get(), m_textureUpload->GetResource().Get(), 0, 0, 1, &data);
			m_texture->TransitionResource(DirectX12Driver::GetCurrent()->GetCommands(),
				m_texture->GetCurrentState(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
			hasBeenUpload = true;
		}
		return (void*)m_texture->GetHandle().GetGPUAddress().ptr;
	}

	DirectX12RenderTexture::DirectX12RenderTexture(uint32_t width, uint32_t height, bool isDepthOnly, TextureOptions options): RenderTexture(width, height, options), isDepthOnly(isDepthOnly)
	{
		Initialize();
	}

	DirectX12RenderTexture::~DirectX12RenderTexture()
	{

	}

	void* DirectX12RenderTexture::GetHandler()
	{
		return (void*)rtvTexture->GetTexture().GetHandle().GetGPUAddress().ptr;
	}

	void DirectX12RenderTexture::Resize(uint32_t width, uint32_t height)
	{
		if (this->m_width == width && this->m_height == height) return;
		//Initialize();
	}

	void DirectX12RenderTexture::Initialize() {
		DX12TextureResourceInfo info, depthInfo;
		info.CreateRenderTargetInformation(DirectX12Driver::GetCurrent()->GetDevice(), m_width, m_height);
		depthInfo.CreateDepthStencilInformation(DirectX12Driver::GetCurrent()->GetDevice(), m_width, m_height);

		rtvTexture = CreateRef<DX12RenderTargetResource>(isDepthOnly ? depthInfo: info, isDepthOnly);
		rtvTexture->CreateWithShaderResource(DirectX12Driver::GetCurrent()->GetDevice(),
			isDepthOnly ?
			DirectX12Driver::GetCurrent()->GetDepthStencilViewDescriptorHeap() :
			DirectX12Driver::GetCurrent()->GetRenderTargetViewDescriptorHeap(),
			DirectX12Driver::GetCurrent()->GetShaderResourceViewDescriptorHeap());
		rtvTexture->Name("Render texture");

		screenViewport.TopLeftX = 0;
		screenViewport.TopLeftY = 0;
		screenViewport.Width = static_cast<float>(m_width);
		screenViewport.Height = static_cast<float>(m_height);
		screenViewport.MinDepth = 0.0f;
		screenViewport.MaxDepth = 1.0f;

		// Does it have to be long?
		scissorRect = { 0, 0, static_cast<long>(m_width), static_cast<long>(m_height) };
	}

	void DirectX12RenderTexture::Bind()
	{
		DirectX12Driver::GetCurrent()->GetCommands()->GetCommandList()->RSSetViewports(1, &screenViewport);
		DirectX12Driver::GetCurrent()->GetCommands()->GetCommandList()->RSSetScissorRects(1, &scissorRect);

		if (!isDepthOnly) {
			DirectX12Driver::GetCurrent()->GetCommands()->GetCommandList()
				->OMSetRenderTargets(1, &rtvTexture->GetHandle().GetCPUAddress(), FALSE, &DirectX12Driver::GetCurrent()->GetDepthStencilBuffer()->GetHandle().GetCPUAddress());
		}
		else {
			DirectX12Driver::GetCurrent()->GetCommands()->GetCommandList()
				->OMSetRenderTargets(0, nullptr, FALSE, &rtvTexture->GetHandle().GetCPUAddress());
		}
		
	}

	void DirectX12RenderTexture::Transition(D3D12_RESOURCE_STATES transition) {
		if (rtvTexture->GetTexture().GetCurrentState() != transition) {
			rtvTexture->GetTexture().TransitionResource(DirectX12Driver::GetCurrent()->GetCommands(),
				rtvTexture->GetTexture().GetCurrentState(), transition);
		}
	}

	DirectX12Cubemap::DirectX12Cubemap(TextureDefault textureDefault, uint32_t size, TextureOptions options): Cubemap(textureDefault, size, options)
	{
		Initialize(size, DXGI_FORMAT_R32G32B32A32_FLOAT, options);
		PopulateColor();
		GetSurfaceInfo(size, size, DXGI_FORMAT_R32G32B32A32_FLOAT, &numBytes, &rowBytes, &numRows);
	}

	DirectX12Cubemap::DirectX12Cubemap(std::array<std::string, 6> paths, TextureOptions options): Cubemap(paths, options)
	{
		int channels, width, height;
		for (int index = 0; index < numberOfSides; index++) {
			auto img = stbi_load(paths[index].c_str(), &width, &height, &channels, 0);
			m_data[index] = img;
		}
		
		m_size = width;
		m_size = height;
		DXGI_FORMAT format = DXGI_FORMAT_R32G32B32A32_FLOAT;

		if (channels == 4)
		{
			format = DXGI_FORMAT_R8G8B8A8_UNORM; // GL_RGBA8;
		}
		// TODO: What to do when there are only 3 channels?
		//else if (channels == 3)
		//{
		//	format = DXGI_FORMAT_R32G32B32_UINT; // GL_RGB8;
		//}
		Initialize(m_size, format, options);
		GetSurfaceInfo(m_size, m_size, format, &numBytes, &rowBytes, &numRows);
	}

	DirectX12Cubemap::~DirectX12Cubemap()
	{
	}

	void* DirectX12Cubemap::GetHandler()
	{
		if (!hasBeenUpload) {
			m_texture->TransitionResource(DirectX12Driver::GetCurrent()->GetCommands(),
				m_texture->GetCurrentState(), D3D12_RESOURCE_STATE_COPY_DEST);
			D3D12_SUBRESOURCE_DATA data[6];
			for (int i = 0; i < numberOfSides; i++) {
				data[i].pData = m_data[i];
				data[i].RowPitch = static_cast<UINT>(rowBytes);
				data[i].SlicePitch = static_cast<UINT>(numBytes);
			}
			
			// TODO: I can't load anything beyond 512x512 Pixels
			UpdateSubresources(DirectX12Driver::GetCurrent()->GetCommands()->GetCommandList().Get(), m_texture->GetResource().Get(), m_textureUpload->GetResource().Get(), 0, 0, numberOfSides, data);
			m_texture->TransitionResource(DirectX12Driver::GetCurrent()->GetCommands(),
				m_texture->GetCurrentState(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
			hasBeenUpload = true;
		}
		return (void*)m_texture->GetHandle().GetGPUAddress().ptr;
	}

	void DirectX12Cubemap::PopulateColor()
	{
		std::array<Vector4Float, 6> colors = {
		   Vector4Float(1, 0, 0, 1),
		   Vector4Float(0, 1, 0, 1),
		   Vector4Float(0, 0, 1, 1),
		   Vector4Float(1, 1, 0, 1),
		   Vector4Float(1, 0, 1, 1),
		   Vector4Float(0, 1, 1, 1)
		};

		std::array<int, 6> indicesToCheck = {
			1,
			2,
			0,
			2,
			1,
			0
		};
		for (unsigned int i = 0; i < numberOfSides; i++)
		{
			m_data[i] = PopulateTest(indicesToCheck[i], colors[i]);
		}
	}

	void DirectX12Cubemap::PopulateTest()
	{		
		Vector4Float value  = Transform(m_textureDefault);
		for (unsigned int i = 0; i < numberOfSides; i++)
		{
			size_t size = sizeof(Vector4Float) * m_size * m_size;
			Vector4Float* data = (Vector4Float*)malloc(size);
			memset(data, 0, size);
			uint32_t index = 0;			
			for (uint32_t column = 0; column < m_size; column++) {				
				for (uint32_t row = 0; row < m_size; row++) {					
					index = (column * m_size) + row;
					data[index] = value;
				}
			}
			m_data[i] = data;
		}
	}

	void* DirectX12Cubemap::PopulateTest(int checkIndex, Vector4Float otherColor)
	{
		size_t size = sizeof(Vector4Float) * m_size * m_size;
		Vector4Float* data = (Vector4Float*)malloc(size);
		memset(data, 0, size);
		uint32_t index = 0;
		Vector4Float white(1, 1, 1, 1);
		Vector4Float value = white;
		for (uint32_t column = 0; column < m_size; column++) {
			if (column % 32 == 0) {
				if (value[checkIndex] == 1)
					value = otherColor;
				else if (value[checkIndex] == 0)
					value = white;
			}
			for (uint32_t row = 0; row < m_size; row++) {
				if (row % 32 == 0) {
					if (value[checkIndex] == 1)
						value = otherColor;
					else if (value[checkIndex] == 0)
						value = white;
				}
				index = (column * m_size) + row;
				data[index] = value;
			}
		}
		return data;
	}

	void DirectX12Cubemap::Initialize(uint32_t size, DXGI_FORMAT format, TextureOptions options)
	{
		DX12TextureResourceInfo info;
		info.newResourceDescription = new D3D12_RESOURCE_DESC;
		info.newResourceDescription->Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		info.newResourceDescription->Alignment = 0;
		info.newResourceDescription->Width = size;
		info.newResourceDescription->Height = size;
		info.newResourceDescription->DepthOrArraySize = numberOfSides;
		info.newResourceDescription->MipLevels = 1;
		info.newResourceDescription->Format = format;

		info.shaderResourceDescription = new D3D12_SHADER_RESOURCE_VIEW_DESC;
		info.shaderResourceDescription->Format = format;
		info.shaderResourceDescription->Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		info.shaderResourceDescription->ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
		info.shaderResourceDescription->TextureCube.MipLevels = 1;
		info.shaderResourceDescription->TextureCube.MostDetailedMip = 0;
		info.shaderResourceDescription->TextureCube.ResourceMinLODClamp = 0;

		info.newResourceDescription->SampleDesc.Count = 1; // device->GetMSAAQuality() ? 4 : 1;
		info.newResourceDescription->SampleDesc.Quality = 0; // device->GetMSAAQuality() ? (device->GetMSAAQuality() - 1) : 0;
		info.newResourceDescription->Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
		info.newResourceDescription->Flags = D3D12_RESOURCE_FLAG_NONE;

		info.newResourceState = D3D12_RESOURCE_STATE_COMMON; //D3D12_RESOURCE_STATE_COMMON;

		m_texture = CreateRef<DX12TextureResource>(format);
		m_texture->CreateTextureResource(info,
			DirectX12Driver::GetCurrent()->GetDevice(),
			DirectX12Driver::GetCurrent()->GetShaderResourceViewDescriptorHeap());

		DX12BufferResourceInfo bInfo;
		bInfo.CreateBufferResourceDescription(true);
		bInfo.newResourceDescription->Width = sizeof(Vector4Float) * size * size * numberOfSides;

		m_textureUpload = CreateRef<DX12BufferResource>(bInfo);
		auto requiredSize = GetRequiredIntermediateSize(m_texture->GetResource().Get(), 0, numberOfSides);
		m_textureUpload->CreateBuffer(DirectX12Driver::GetCurrent()->GetDevice(), requiredSize);
	}
}
#endif