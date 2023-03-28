#include "vwpch.h"

#ifdef VWOLF_PLATFORM_WINDOWS
#include "DirectX12Texture.h"

#include "stb_image/stb_image.h"

#include "VWolf/Core/Math/VMath.h"

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


	DirectX12Texture2D::DirectX12Texture2D(uint32_t width, uint32_t height, TextureOptions options): Texture2D(width, height, options)
	{
		Initialize(width, height, DXGI_FORMAT_R32G32B32A32_FLOAT, options);
		m_data = PopulateTest();
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

	DirectX12RenderTexture::DirectX12RenderTexture(uint32_t width, uint32_t height, TextureOptions options): RenderTexture(width, height, options)
	{
		DX12TextureResourceInfo info;
		info.CreateRenderTargetInformation(DirectX12Driver::GetCurrent()->GetDevice(), width, height);

		rtvTexture = CreateRef<DX12RenderTargetResource>(info);
		rtvTexture->CreateWithShaderResource(DirectX12Driver::GetCurrent()->GetDevice(),
			DirectX12Driver::GetCurrent()->GetRenderTargetViewDescriptorHeap(),
			DirectX12Driver::GetCurrent()->GetShaderResourceViewDescriptorHeap());
	}

	DirectX12RenderTexture::~DirectX12RenderTexture()
	{
	}

	void* DirectX12RenderTexture::GetHandler()
	{
		if (rtvTexture->GetTexture().GetCurrentState() != D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE) {
			rtvTexture->GetTexture().TransitionResource(DirectX12Driver::GetCurrent()->GetCommands(),
				rtvTexture->GetTexture().GetCurrentState(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
		}

		return (void*)rtvTexture->GetTexture().GetHandle().GetGPUAddress().ptr;
	}

	void DirectX12RenderTexture::Resize(uint32_t width, uint32_t height)
	{
	}

	void DirectX12RenderTexture::Bind()
	{
		if (rtvTexture->GetTexture().GetCurrentState() != D3D12_RESOURCE_STATE_RENDER_TARGET) {
			rtvTexture->GetTexture().TransitionResource(DirectX12Driver::GetCurrent()->GetCommands(),
				rtvTexture->GetTexture().GetCurrentState(), D3D12_RESOURCE_STATE_RENDER_TARGET);
		}
		DirectX12Driver::GetCurrent()->GetCommands()->GetCommandList()
			->OMSetRenderTargets(1, &rtvTexture->GetHandle().GetCPUAddress(), FALSE, &DirectX12Driver::GetCurrent()->GetDepthStencilBuffer()->GetHandle().GetCPUAddress());
	}
}
#endif