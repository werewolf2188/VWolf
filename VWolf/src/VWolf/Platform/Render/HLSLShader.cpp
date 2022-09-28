#include "vwpch.h"
#ifdef VWOLF_PLATFORM_WINDOWS
#include "HLSLShader.h"

namespace VWolf {

	struct ShaderContext {
		Microsoft::WRL::ComPtr<ID3DBlob> mvsByteCode = nullptr;
		Microsoft::WRL::ComPtr<ID3DBlob> mpsByteCode = nullptr;
		Microsoft::WRL::ComPtr<ID3D12RootSignature> mRootSignature = nullptr;
		Microsoft::WRL::ComPtr<ID3D12PipelineState> mPSO = nullptr;
	};

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

	void CompileShader(
		const std::wstring& filename,
		const D3D_SHADER_MACRO* defines,
		const std::string& entrypoint,
		const std::string& target,
		Ref<ShaderContext> shaderContext)
	{
		UINT compileFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)  
		compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

		HRESULT hr = S_OK;

		Microsoft::WRL::ComPtr<ID3DBlob> byteCode = nullptr;
		Microsoft::WRL::ComPtr<ID3DBlob> errors;

		hr = D3DCompileFromFile(filename.c_str(), defines, D3D_COMPILE_STANDARD_FILE_INCLUDE,
			entrypoint.c_str(), target.c_str(), compileFlags, 0, &byteCode, &errors);

		if (errors != nullptr)
			VWOLF_CORE_ERROR((char*)errors->GetBufferPointer());

		ThrowIfFailed(hr);

		if (target == "vs_5_0") {
			shaderContext->mvsByteCode = byteCode;
		}
		else if (target == "ps_5_0") {
			shaderContext->mpsByteCode = byteCode;
		}
		
	}

	void BuildRootSignature(DirectX12Context *context, Ref<ShaderContext> shaderContext)
	{
		// Shader programs typically require resources as input (constant buffers,
		// textures, samplers).  The root signature defines the resources the shader
		// programs expect.  If we think of the shader programs as a function, and
		// the input resources as function parameters, then the root signature can be
		// thought of as defining the function signature.  

		// Root parameter can be a table, root descriptor or root constants.
		CD3DX12_ROOT_PARAMETER slotRootParameter[1];

		// Create a single descriptor table of CBVs.
		CD3DX12_DESCRIPTOR_RANGE cbvTable;
		cbvTable.Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0);
		//slotRootParameter[0].InitAsDescriptorTable(1, &cbvTable);
		// Instead, create a root descriptor
		slotRootParameter[0].InitAsConstantBufferView(0);

		// A root signature is an array of root parameters.
		CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc(1, slotRootParameter, 0, nullptr,
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
		psoDesc.VS =
		{
			reinterpret_cast<BYTE*>(shaderContext->mvsByteCode->GetBufferPointer()),
			shaderContext->mvsByteCode->GetBufferSize()
		};
		psoDesc.PS =
		{
			reinterpret_cast<BYTE*>(shaderContext->mpsByteCode->GetBufferPointer()),
			shaderContext->mpsByteCode->GetBufferSize()
		};
		psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
		psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
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

	HLSLShader::HLSLShader(const std::string& name, BufferLayout layout, HWND__* window, DirectX12Context* context) : Shader(name, layout), m_window(window), m_context(context)
	{
		m_shaderContext = CreateRef<ShaderContext>();
		BuildRootSignature(m_context, m_shaderContext);
		VWOLF_CORE_ASSERT(m_shaderContext->mRootSignature);
		std::wstring str_turned_to_wstr = std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes(filepath);
		std::wstring file = str_turned_to_wstr + L".hlsl";
		CompileShader(file, nullptr, "VS", "vs_5_0", m_shaderContext);
		VWOLF_CORE_ASSERT(m_shaderContext->mvsByteCode);
		CompileShader(file, nullptr, "PS", "ps_5_0", m_shaderContext);
		VWOLF_CORE_ASSERT(m_shaderContext->mpsByteCode);
		BuildPSO(m_context, m_shaderContext, layout);
		VWOLF_CORE_ASSERT(m_shaderContext->mPSO);
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
	void HLSLShader::SetInt(const std::string& name, int value)
	{
	}
	void HLSLShader::SetIntArray(const std::string& name, int* values, uint32_t count)
	{
	}
	void HLSLShader::SetFloat(const std::string& name, float value)
	{
	}
	void HLSLShader::SetFloat2(const std::string& name, const Vector2Float& value)
	{
	}
	void HLSLShader::SetFloat3(const std::string& name, const Vector3Float& value)
	{
	}
	void HLSLShader::SetFloat4(const std::string& name, const Vector4Float& value)
	{
	}
	void HLSLShader::SetMat3(const std::string& name, const MatrixFloat3x3& value)
	{
	}
	void HLSLShader::SetMat4(const std::string& name, const MatrixFloat4x4& value)
	{
	}
	const std::string& HLSLShader::GetName() const
	{
		return filepath;
	}
	Microsoft::WRL::ComPtr<ID3D12PipelineState> HLSLShader::GetPipeline()
	{
		return m_shaderContext->mPSO;
	}
}
#endif