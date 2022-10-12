#include "vwpch.h"
#ifdef VWOLF_PLATFORM_WINDOWS
#include "HLSLShader.h"

namespace VWolf {

	struct ConstantBufferContext
	{
		Microsoft::WRL::ComPtr<ID3D12Resource> mUploadBuffer;
		BYTE* mMappedData = nullptr;
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mSrvHeap;
		int binding;
	};

	struct ShaderContext {
		std::map<ShaderType, Microsoft::WRL::ComPtr<ID3DBlob>> shaderBlobs;
		Microsoft::WRL::ComPtr<ID3D12RootSignature> mRootSignature = nullptr;
		Microsoft::WRL::ComPtr<ID3D12PipelineState> mPSO = nullptr;
		std::map<std::string, Ref<ConstantBufferContext>> constantBuffers;
	};

	static UINT CalcConstantBufferByteSize(UINT byteSize)
	{
		// Constant buffers must be a multiple of the minimum hardware
		// allocation size (usually 256 bytes).  So round up to nearest
		// multiple of 256.  We do this by adding 255 and then masking off
		// the lower 2 bytes which store all bits < 256.
		// Example: Suppose byteSize = 300.
		// (300 + 255) & ~255
		// 555 & ~255
		// 0x022B & ~0x00ff
		// 0x022B & 0xff00
		// 0x0200
		// 512
		return (byteSize + 255) & ~255;
	}

	static DXGI_FORMAT ShaderDataTypeToDirectXBaseType(ShaderDataType type)
	{
		switch (type)
		{
		case ShaderDataType::Float:    return DXGI_FORMAT_R32_FLOAT;
		case ShaderDataType::Float2:   return DXGI_FORMAT_R32G32_FLOAT;
		case ShaderDataType::Float3:   return DXGI_FORMAT_R32G32B32_FLOAT;
		case ShaderDataType::Float4:   return DXGI_FORMAT_R32G32B32A32_FLOAT;
		case ShaderDataType::Mat3:     return DXGI_FORMAT_R32G32B32_FLOAT;
		case ShaderDataType::Mat4:     return DXGI_FORMAT_R32G32B32A32_FLOAT;
		case ShaderDataType::Int:      return DXGI_FORMAT_R32_SINT;
		case ShaderDataType::Int2:     return DXGI_FORMAT_R32G32_SINT;
		case ShaderDataType::Int3:     return DXGI_FORMAT_R32G32B32_SINT;
		case ShaderDataType::Int4:     return DXGI_FORMAT_R32G32B32A32_SINT;
		case ShaderDataType::Bool:     return DXGI_FORMAT_R8_UINT;
		}

		VWOLF_CORE_ASSERT(false, "Unknown ShaderDataType!");
		return DXGI_FORMAT_UNKNOWN;
	}

	std::string ShaderTypeEquivalent(ShaderType type) {
		switch (type) {
		case ShaderType::Vertex: return "vs_5_0";
		case ShaderType::Fragment: return "ps_5_0";
		case ShaderType::Pre_Tesselator: return "hs_5_0";
		case ShaderType::Post_Tesselator: return "ds_5_0";
		case ShaderType::Geometry: return "gs_5_0";
		case ShaderType::Compute: return "cs_5_0";
		}
	}

	void CompileShader(
		const ShaderSource shader,
		const D3D_SHADER_MACRO* defines,
		const std::string& entrypoint,
		const std::string& target,
		Ref<ShaderContext> shaderContext)
	{
		VWOLF_CORE_ASSERT(shader.sourceType == ShaderSourceType::File, "HLSL does not allow shader source different than files");
		std::wstring filename = std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes(shader.shader);
		UINT compileFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)  
		compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

		HRESULT hr = S_OK;

		Microsoft::WRL::ComPtr<ID3DBlob> byteCode = nullptr;
		Microsoft::WRL::ComPtr<ID3DBlob> errors;

		hr = D3DCompileFromFile(filename.c_str(), defines, D3D_COMPILE_STANDARD_FILE_INCLUDE,
			entrypoint.c_str(), target.c_str(), compileFlags, 0, &byteCode, &errors);

		if (errors != nullptr) {
			VWOLF_CORE_ERROR((char*)errors->GetBufferPointer());
			VWOLF_CORE_ASSERT(errors == nullptr, "Shader produce an error");
		}
			
		ThrowIfFailed(hr);

		shaderContext->shaderBlobs.insert(std::pair<ShaderType, Microsoft::WRL::ComPtr<ID3DBlob>>(shader.type, byteCode));
		
	}

	void BuildRootSignature(DirectX12Context *context, Ref<ShaderContext> shaderContext, std::vector<ShaderParameter> parameters)
	{
		// Shader programs typically require resources as input (constant buffers,
		// textures, samplers).  The root signature defines the resources the shader
		// programs expect.  If we think of the shader programs as a function, and
		// the input resources as function parameters, then the root signature can be
		// thought of as defining the function signature.  

		// Root parameter can be a table, root descriptor or root constants.
		std::vector<CD3DX12_ROOT_PARAMETER> slotRootParameter(parameters.size());

		// Create a single descriptor table of CBVs.
		//CD3DX12_DESCRIPTOR_RANGE cbvTable;
		//cbvTable.Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0);
		//slotRootParameter[0].InitAsDescriptorTable(1, &cbvTable);
		// Instead, create a root descriptor
		for (int i = 0; i < parameters.size(); i++)
			slotRootParameter[i].InitAsConstantBufferView(parameters[i].binding);

		// A root signature is an array of root parameters.
		CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc(parameters.size(), slotRootParameter.data(), 0, nullptr,
			D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

		// create a root signature with a single slot which points to a descriptor range consisting of a single constant buffer
		Microsoft::WRL::ComPtr<ID3DBlob> serializedRootSig = nullptr;
		Microsoft::WRL::ComPtr<ID3DBlob> errorBlob = nullptr;
		HRESULT hr = D3D12SerializeRootSignature(&rootSigDesc, D3D_ROOT_SIGNATURE_VERSION_1,
			serializedRootSig.GetAddressOf(), errorBlob.GetAddressOf());

		if (errorBlob != nullptr)
		{
			VWOLF_CORE_ERROR((char*)errorBlob->GetBufferPointer());
		}
		ThrowIfFailed(hr);

		Microsoft::WRL::ComPtr<ID3D12RootSignature> mRootSignature = nullptr;
		ThrowIfFailed(context->md3dDevice->CreateRootSignature(
			0,
			serializedRootSig->GetBufferPointer(),
			serializedRootSig->GetBufferSize(),
			IID_PPV_ARGS(&mRootSignature)));
		shaderContext->mRootSignature = mRootSignature;
	}

	void BuildPSO(DirectX12Context* context, Ref<ShaderContext> shaderContext, BufferLayout layout)
	{
		// TODO: This should improve to add matrices
		std::vector<D3D12_INPUT_ELEMENT_DESC> mInputLayout;
		for (const auto& element : layout)
		{
			mInputLayout.push_back({ element.Name.c_str(), 0, ShaderDataTypeToDirectXBaseType(element.Type), 0, (unsigned int)element.Offset, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });
		}

		D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc;
		ZeroMemory(&psoDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
		psoDesc.InputLayout = { mInputLayout.data(), (UINT)mInputLayout.size() };
		psoDesc.pRootSignature = shaderContext->mRootSignature.Get();

		for (auto [type, blob] : shaderContext->shaderBlobs) {
			switch (type) {
			case ShaderType::Vertex:
				psoDesc.VS =
				{
					reinterpret_cast<BYTE*>(blob->GetBufferPointer()),
					blob->GetBufferSize()
				};
				break;
			case ShaderType::Fragment:
				psoDesc.PS =
				{
					reinterpret_cast<BYTE*>(blob->GetBufferPointer()),
					blob->GetBufferSize()
				};
				break;
			case ShaderType::Pre_Tesselator: 
				psoDesc.HS =
				{
					reinterpret_cast<BYTE*>(blob->GetBufferPointer()),
					blob->GetBufferSize()
				};
				break;
			case ShaderType::Post_Tesselator: 
				psoDesc.DS =
				{
					reinterpret_cast<BYTE*>(blob->GetBufferPointer()),
					blob->GetBufferSize()
				};
				break;
			case ShaderType::Geometry: 
				psoDesc.GS =
				{
					reinterpret_cast<BYTE*>(blob->GetBufferPointer()),
					blob->GetBufferSize()
				};
				break;
			case ShaderType::Compute: 
				/*psoDesc.CS =
				{
					reinterpret_cast<BYTE*>(blob->GetBufferPointer()),
					blob->GetBufferSize()
				};*/
				break;
			}
		}
		psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
		psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
		//
	// PSO for transparent objects
	//


		D3D12_RENDER_TARGET_BLEND_DESC transparencyBlendDesc;
		transparencyBlendDesc.BlendEnable = true;
		transparencyBlendDesc.LogicOpEnable = false;
		transparencyBlendDesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;
		transparencyBlendDesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
		transparencyBlendDesc.BlendOp = D3D12_BLEND_OP_ADD;
		transparencyBlendDesc.SrcBlendAlpha = D3D12_BLEND_ONE;
		transparencyBlendDesc.DestBlendAlpha = D3D12_BLEND_ZERO;
		transparencyBlendDesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;
		transparencyBlendDesc.LogicOp = D3D12_LOGIC_OP_NOOP;
		transparencyBlendDesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

		psoDesc.BlendState.RenderTarget[0] = transparencyBlendDesc;
		psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
		psoDesc.SampleMask = UINT_MAX;
		psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		psoDesc.NumRenderTargets = 1;
		psoDesc.RTVFormats[0] = context->mBackBufferFormat;
		psoDesc.SampleDesc.Count = context->m4xMsaaState ? 4 : 1;
		psoDesc.SampleDesc.Quality = context->m4xMsaaState ? (context->m4xMsaaQuality - 1) : 0;
		psoDesc.DSVFormat = context->mDepthStencilFormat;
		ThrowIfFailed(context->md3dDevice->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&shaderContext->mPSO)));
	}

	HLSLShader::HLSLShader(HWND__* window, 
		DirectX12Context* context,
		const char* name,
		ShaderSource vertexShader,
		BufferLayout layout,
		std::initializer_list<ShaderSource> otherShaders,
		std::initializer_list<ShaderParameter> parameters,
		ShaderConfiguration configuration) : 
		Shader(name, 
			vertexShader, 
			layout, 
			otherShaders, 
			parameters, 
			configuration), 
		m_window(window), m_context(context)
	{
		m_shaderContext = CreateRef<ShaderContext>();
		BuildRootSignature(m_context, m_shaderContext, m_parameters);
		VWOLF_CORE_ASSERT(m_shaderContext->mRootSignature);
	
		VWOLF_CORE_ASSERT(vertexShader.type == ShaderType::Vertex, "The first shader should be a vertex shader");
		bool hasFragmentShader = false;
		for (ShaderSource source : m_otherShaders) {
			hasFragmentShader = source.type == ShaderType::Fragment;
			if (hasFragmentShader) break;
		}
		VWOLF_CORE_ASSERT(hasFragmentShader, "There should be at least one fragment shader");
		CompileShader(vertexShader, nullptr, vertexShader.mainFunction, ShaderTypeEquivalent(vertexShader.type), m_shaderContext);
		for (ShaderSource source : m_otherShaders)
			CompileShader(source, nullptr, source.mainFunction, ShaderTypeEquivalent(source.type), m_shaderContext);
		VWOLF_CORE_ASSERT(m_shaderContext->shaderBlobs.size() == (m_otherShaders.size() + 1), "One of the shaders didn't compile");

		BuildPSO(m_context, m_shaderContext, layout);
		VWOLF_CORE_ASSERT(m_shaderContext->mPSO);

		// TODO: Build constant buffers
		// Constant Buffers
		for (ShaderParameter param : m_parameters) {
			Ref<ConstantBufferContext> cb = CreateRef< ConstantBufferContext>();
			cb->binding = param.binding;
			ThrowIfFailed(context->md3dDevice->CreateCommittedResource(
				&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
				D3D12_HEAP_FLAG_NONE,
				&CD3DX12_RESOURCE_DESC::Buffer(CalcConstantBufferByteSize(param.size) * 1),
				D3D12_RESOURCE_STATE_GENERIC_READ,
				nullptr,
				IID_PPV_ARGS(&cb->mUploadBuffer)));

			ThrowIfFailed(cb->mUploadBuffer->Map(0, nullptr, reinterpret_cast<void**>(&cb->mMappedData)));
			//ZeroMemory(m_cbContext->mMappedData, CalcConstantBufferByteSize(size) * 1);
			VWOLF_CORE_ASSERT(cb->mUploadBuffer);
			VWOLF_CORE_ASSERT(cb->mMappedData);

			// This is if I want to go back to descriptor table
			/*UINT objCBByteSize = CalcConstantBufferByteSize(param.size);
		D3D12_GPU_VIRTUAL_ADDRESS cbAddress = cb->mUploadBuffer.Get()->GetGPUVirtualAddress();
		int boxCBufIndex = 0;
		cbAddress += boxCBufIndex * objCBByteSize;

		D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc;
		cbvDesc.BufferLocation = cbAddress;
		cbvDesc.SizeInBytes = CalcConstantBufferByteSize(param.size);

		dx12InitializeDescriptorHeap(context->md3dDevice, cb->mSrvHeap, 1, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE);

		context->md3dDevice->CreateConstantBufferView(
			&cbvDesc,
			cb->mSrvHeap->GetCPUDescriptorHandleForHeapStart());*/

			m_shaderContext->constantBuffers.insert(std::pair<std::string, Ref<ConstantBufferContext>>(param.name, cb));
		}
		
	}
	HLSLShader::~HLSLShader()
	{
	}
	void HLSLShader::Bind() const
	{
		m_context->mCommandList->SetGraphicsRootSignature(m_shaderContext->mRootSignature.Get());		
	}
	void HLSLShader::Unbind() const
	{
	}


	void HLSLShader::SetData(const void* data, const char* name, uint32_t size, uint32_t offset) {
		Ref<ConstantBufferContext> cb = m_shaderContext->constantBuffers[name];
		memcpy(&cb->mMappedData[offset * CalcConstantBufferByteSize(size)], data, size);
		// Attach root descriptor directly to the command list
		m_context->mCommandList->SetGraphicsRootConstantBufferView(cb->binding, cb->mUploadBuffer.Get()->GetGPUVirtualAddress());
		// Attach descriptor table via descriptor heap
		/*dx12SetDescriptorHeaps(m_context, m_cbContext->mSrvHeap);
		m_context->mCommandList->SetGraphicsRootDescriptorTable(0, m_cbContext->mSrvHeap->GetGPUDescriptorHandleForHeapStart());*/
	}
	
	const char* HLSLShader::GetName() const
	{
		return m_name;
	}
	Microsoft::WRL::ComPtr<ID3D12PipelineState> HLSLShader::GetPipeline()
	{
		return m_shaderContext->mPSO;
	}
}
#endif