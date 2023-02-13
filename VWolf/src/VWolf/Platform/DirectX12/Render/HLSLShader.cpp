#include "vwpch.h"
#ifdef VWOLF_PLATFORM_WINDOWS
#include "HLSLShader.h"

#include "VWolf/Core/Application.h"
#include "VWolf/Platform/DirectX12/DirectX12Driver.h"

namespace VWolf {

	static DirectX12Context* GetMainContext() {
		return ((DirectX12Driver*)Application::GetApplication()->GetDriver())->GetContext();
	}

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

	class HLAttribute {
	public:
		HLAttribute() = default;
		HLAttribute(D3D11_SIGNATURE_PARAMETER_DESC desc, UINT index, UINT offset): 
			index(index), offset(offset), elementType(desc.ComponentType) {
			this->name = desc.SemanticName;
			BYTE oneElement = 1, twoElement = 3, threeElement = 7, fourElement = 15;
			
			if (desc.Mask == oneElement)
			{
				numberOfElements = 1;
				if (desc.ComponentType == D3D_REGISTER_COMPONENT_UINT32)
					this->format = DXGI_FORMAT_R32_UINT;
				else if (desc.ComponentType == D3D_REGISTER_COMPONENT_SINT32)
					this->format = DXGI_FORMAT_R32_SINT;
				else if (desc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32)
					this->format = DXGI_FORMAT_R32_FLOAT;
			}
			else if (desc.Mask <= twoElement)
			{
				numberOfElements = 2;
				if (desc.ComponentType == D3D_REGISTER_COMPONENT_UINT32)
					this->format = DXGI_FORMAT_R32G32_UINT;
				else if (desc.ComponentType == D3D_REGISTER_COMPONENT_SINT32)
					this->format = DXGI_FORMAT_R32G32_SINT;
				else if (desc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32)
					this->format = DXGI_FORMAT_R32G32_FLOAT;
			}
			else if (desc.Mask <= threeElement)
			{
				numberOfElements = 3;
				if (desc.ComponentType == D3D_REGISTER_COMPONENT_UINT32)
					this->format = DXGI_FORMAT_R32G32B32_UINT;
				else if (desc.ComponentType == D3D_REGISTER_COMPONENT_SINT32)
					this->format = DXGI_FORMAT_R32G32B32_SINT;
				else if (desc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32)
					this->format = DXGI_FORMAT_R32G32B32_FLOAT;
			}
			else if (desc.Mask <= fourElement)
			{
				numberOfElements = 4;
				if (desc.ComponentType == D3D_REGISTER_COMPONENT_UINT32)
					this->format = DXGI_FORMAT_R32G32B32A32_UINT;
				else if (desc.ComponentType == D3D_REGISTER_COMPONENT_SINT32)
					this->format = DXGI_FORMAT_R32G32B32A32_SINT;
				else if (desc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32)
					this->format = DXGI_FORMAT_R32G32B32A32_FLOAT;
			}
		}

		size_t GetSize() {
			if (elementType == D3D_REGISTER_COMPONENT_UINT32)
				return sizeof(UINT) * numberOfElements;
			else if (elementType == D3D_REGISTER_COMPONENT_SINT32)
				return sizeof(INT) * numberOfElements;
			else if (elementType == D3D_REGISTER_COMPONENT_FLOAT32)
				return sizeof(FLOAT) * numberOfElements;
		}

		UINT GetOffset() {
			return offset;
		}

		UINT GetIndex() {
			return index;
		}

		D3D12_INPUT_ELEMENT_DESC GetInputElementDesc() {
			return { name.c_str(), 0, format, 0, offset, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
		}
	private:
		std::string name;
		UINT index;
		DXGI_FORMAT format;
		UINT offset;

		UINT numberOfElements;
		BYTE elementType;
	};

	class HLConstantBufferVariable {
	public:
		HLConstantBufferVariable() = default;
		HLConstantBufferVariable(ID3D11ShaderReflectionVariable* vCb, UINT index): index(index) {
			// if NULL, throw exception
			ID3D11ShaderReflectionType* vCbType = vCb->GetType();

			D3D11_SHADER_VARIABLE_DESC vCbDesc;
			ZeroMemory(&vCbDesc, sizeof(D3D11_SHADER_VARIABLE_DESC));
			vCb->GetDesc(&vCbDesc);

			//VWOLF_CORE_DEBUG("Constant buffer variable %s in index :%d, offset %d, size %d", vCbDesc.Name, index, vCbDesc.StartOffset, vCbDesc.Size);

			D3D11_SHADER_TYPE_DESC vTypeCbDesc;
			ZeroMemory(&vTypeCbDesc, sizeof(D3D11_SHADER_TYPE_DESC));
			vCbType->GetDesc(&vTypeCbDesc);
			/*VWOLF_CORE_DEBUG("Constant buffer variable type %s is type %d, class %d, rows %d, columns, %d, elements %d, members %d, offset %d",
				vTypeCbDesc.Name, vTypeCbDesc.Class, vTypeCbDesc.Type, vTypeCbDesc.Rows, vTypeCbDesc.Columns, vTypeCbDesc.Elements, vTypeCbDesc.Members, vTypeCbDesc.Offset);*/

			this->name = vCbDesc.Name;
			this->offset = vCbDesc.StartOffset;
			this->size = vCbDesc.Size;

			this->desc = vCbDesc;
			this->typeDesc = vTypeCbDesc;
		}

		std::string GetName() {
			return name;
		}

		size_t GetSize() {
			return size;
		}

		UINT GetOffset() {
			return offset;
		}

		UINT GetIndex() {
			return index;
		}

		ShaderDataType GetShaderDataType() {			
			switch (typeDesc.Type) {
			case D3D_SVT_BOOL:
				return ShaderDataType::Bool;
			case D3D_SVT_INT:
				switch (typeDesc.Class) {
					case D3D_SVC_SCALAR:
						return ShaderDataType::Int;
					case D3D_SVC_VECTOR:
						switch (typeDesc.Columns) {
							case 2: return ShaderDataType::Int2;
							case 3: return ShaderDataType::Int3;
							case 4: return ShaderDataType::Int4;
						}
				}
			case D3D_SVT_FLOAT:
				switch (typeDesc.Class) {
				case D3D_SVC_SCALAR:
					return ShaderDataType::Float;
				case D3D_SVC_VECTOR:
					switch (typeDesc.Columns) {
					case 2: return ShaderDataType::Float2;
					case 3: return ShaderDataType::Float3;
					case 4: return ShaderDataType::Float4;
					}
				}

			}

			return ShaderDataType::None;
		}
	private:
		std::string name;
		size_t size;
		UINT offset;
		UINT index;

		D3D11_SHADER_VARIABLE_DESC desc;
		D3D11_SHADER_TYPE_DESC typeDesc;
	};
	class HLConstantBuffer {
	public:
		HLConstantBuffer() = default;
		HLConstantBuffer(D3D11_SHADER_BUFFER_DESC desc, UINT bindingIndex): 
			name(desc.Name), size(desc.Size), bindingIndex(bindingIndex) {}

		std::string GetName() {
			return name;
		}
		size_t GetSize() { 
			return size; 
		}
		UINT GetBindingIndex() { 
			return bindingIndex; 
		}

		Microsoft::WRL::ComPtr<ID3D12Resource>& GetUploadBuffer() { 
			return mUploadBuffer; 
		}

		// TODO: Move this into a getter
		BYTE* mMappedData = nullptr;

		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>& GetSrvHeap() {
			return mSrvHeap;
		}

		void SetVariables(std::vector<HLConstantBufferVariable> variables) {
			this->variables = variables;
		}

		std::vector<HLConstantBufferVariable> GetVariables() {
			return variables;
		}
	private:
		std::string name;
		size_t size;
		UINT bindingIndex;

		Microsoft::WRL::ComPtr<ID3D12Resource> mUploadBuffer;		
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mSrvHeap;

		std::vector<HLConstantBufferVariable> variables;
	};

	class HLProgram {
	public:
		HLProgram(const char* name,
				  std::initializer_list<ShaderSource> otherShaders,
				  ShaderConfiguration configuration = {}):
		name(name) {
			std::map<ShaderType, Microsoft::WRL::ComPtr<ID3DBlob>> shaderBlobs;
			for (ShaderSource source : otherShaders) {
				auto blob = CompileShader(source);
				ReflectHLSL(blob);
				shaderBlobs[source.type] = blob;
			}
			BuildRootSignature();
			BuildPSO(shaderBlobs, configuration);
		}
	public:
		std::map<std::string, Ref<HLConstantBuffer>>& GetConstantBuffers() {
			return constantBuffers;
		}

		Microsoft::WRL::ComPtr<ID3D12RootSignature>& GetRootSignature() {
			return mRootSignature;
		}

		Microsoft::WRL::ComPtr<ID3D12PipelineState>& GetPSO() {
			return mPSO;
		}
	private:
		void ReflectHLSL(Microsoft::WRL::ComPtr<ID3DBlob> byteCode) {
			// Decompiling
			HRESULT hr = S_OK;
			ID3D11ShaderReflection* pReflector = NULL;
			hr = D3DReflect(byteCode->GetBufferPointer(), byteCode->GetBufferSize(), IID_ID3D11ShaderReflection, (void**)&pReflector);
			ThrowIfFailed(hr);

			// Getting description
			D3D11_SHADER_DESC desc;
			ZeroMemory(&desc, sizeof(D3D11_SHADER_DESC));
			hr = pReflector->GetDesc(&desc);
			ThrowIfFailed(hr);

			// Getting attributes
			UINT offset = 0;
			if (attributes.size() == 0) { // TODO: Change the condition
				for (UINT index = 0; index < desc.InputParameters; index++) {
					D3D11_SIGNATURE_PARAMETER_DESC paramDesc;
					ZeroMemory(&paramDesc, sizeof(D3D11_SIGNATURE_PARAMETER_DESC));
					hr = pReflector->GetInputParameterDesc(index, &paramDesc);
					ThrowIfFailed(hr);

					HLAttribute attr(paramDesc, index, offset);
					offset += attr.GetSize();
					attributes[paramDesc.SemanticName] = attr;
				}
			}

			// Constant buffer
			for (UINT index = 0; index < desc.ConstantBuffers; index++) {
				ID3D11ShaderReflectionConstantBuffer* cb = pReflector->GetConstantBufferByIndex(index);
				D3D11_SHADER_BUFFER_DESC cbDesc;
				ZeroMemory(&cbDesc, sizeof(D3D11_SHADER_BUFFER_DESC));
				cb->GetDesc(&cbDesc);

				if (constantBuffers.count(cbDesc.Name)) continue;

				D3D11_SHADER_INPUT_BIND_DESC bindingDesc;
				ZeroMemory(&bindingDesc, sizeof(D3D11_SHADER_INPUT_BIND_DESC));
				pReflector->GetResourceBindingDescByName(cbDesc.Name, &bindingDesc);

				// Variables
				std::vector<HLConstantBufferVariable> variables;
				for (UINT cbIndex = 0; cbIndex < cbDesc.Variables; cbIndex++) {
					ID3D11ShaderReflectionVariable* vCb = cb->GetVariableByIndex(cbIndex);
					HLConstantBufferVariable variable(vCb, cbIndex);
					variables.push_back(variable);
				}

				Ref<HLConstantBuffer> cBuffer = CreateRef<HLConstantBuffer>(cbDesc, bindingDesc.BindPoint);
				cBuffer->SetVariables(variables);
				constantBuffers[cbDesc.Name] = cBuffer;
			}

			pReflector->Release();
		}

		Microsoft::WRL::ComPtr<ID3DBlob> CompileShader(ShaderSource source) {

			const D3D_SHADER_MACRO* defines = nullptr;
			UINT compileFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)  
			compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

			HRESULT hr = S_OK;
			Microsoft::WRL::ComPtr<ID3DBlob> byteCode = nullptr;
			Microsoft::WRL::ComPtr<ID3DBlob> errors;			

			switch (source.sourceType) {
			case ShaderSourceType::Text:
				{
					hr = D3DCompile(source.shader, strlen(source.shader), nullptr, defines, D3D_COMPILE_STANDARD_FILE_INCLUDE,
						source.mainFunction, ShaderTypeEquivalent(source.type).c_str(), compileFlags, 0, &byteCode, &errors);
				}
				break;
			case ShaderSourceType::File:
				{
					std::wstring file = std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes(source.shader);
					hr = D3DCompileFromFile(file.c_str(), defines, D3D_COMPILE_STANDARD_FILE_INCLUDE,
						source.mainFunction, ShaderTypeEquivalent(source.type).c_str(), compileFlags, 0, &byteCode, &errors);
				}
				break;
			case ShaderSourceType::Binary:
				VWOLF_CORE_ASSERT(false, "Shader does not support binaries");
				break;
			}

			if (errors != nullptr) {
				VWOLF_CORE_ERROR((char*)errors->GetBufferPointer());
				VWOLF_CORE_ASSERT(errors == nullptr, "Shader produce an error");
			}
			ThrowIfFailedWithReturnValue(hr, nullptr);
			return byteCode;
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

		void BuildRootSignature() {
			// Shader programs typically require resources as input (constant buffers,
			// textures, samplers).  The root signature defines the resources the shader
			// programs expect.  If we think of the shader programs as a function, and
			// the input resources as function parameters, then the root signature can be
			// thought of as defining the function signature.  

			// Root parameter can be a table, root descriptor or root constants.
			std::vector<CD3DX12_ROOT_PARAMETER> slotRootParameter(constantBuffers.size());

			// Create a single descriptor table of CBVs.
			//CD3DX12_DESCRIPTOR_RANGE cbvTable;
			//cbvTable.Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0);
			//slotRootParameter[0].InitAsDescriptorTable(1, &cbvTable);
			// Instead, create a root descriptor
			for (auto [key,value] : constantBuffers) {
				slotRootParameter[value->GetBindingIndex()].InitAsConstantBufferView(value->GetBindingIndex());
			}
				

			// A root signature is an array of root parameters.
			CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc(slotRootParameter.size(), slotRootParameter.data(), 0, nullptr,
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

			ThrowIfFailed(GetMainContext()->md3dDevice->CreateRootSignature(
				0,
				serializedRootSig->GetBufferPointer(),
				serializedRootSig->GetBufferSize(),
				IID_PPV_ARGS(&mRootSignature)));
			VWOLF_CORE_ASSERT(mRootSignature);
		}

		void BuildPSO(std::map<ShaderType, Microsoft::WRL::ComPtr<ID3DBlob>> shaderBlobs, ShaderConfiguration configuration = {}) {
			std::vector<D3D12_INPUT_ELEMENT_DESC> mInputLayout(attributes.size());
			
			for (auto& [key, value] : attributes)
			{
				mInputLayout[value.GetIndex()] = value.GetInputElementDesc();
			}

			D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc;
			ZeroMemory(&psoDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
			psoDesc.InputLayout = { mInputLayout.data(), (UINT)mInputLayout.size() };
			psoDesc.pRootSignature = mRootSignature.Get();

			for (auto [type, blob] : shaderBlobs) {
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

			// Rasterization
			{
				psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
				if (configuration.rasterization.cullEnabled) {
					switch (configuration.rasterization.cullMode) {
					case ShaderConfiguration::Rasterization::CullMode::Front:
						psoDesc.RasterizerState.CullMode = D3D12_CULL_MODE_FRONT;
						break;
					case ShaderConfiguration::Rasterization::CullMode::Back:
						psoDesc.RasterizerState.CullMode = D3D12_CULL_MODE_BACK;
						break;
					case ShaderConfiguration::Rasterization::CullMode::FrontAndBack:
						break;
					}
				}
				else
					psoDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;

				switch (configuration.rasterization.fillMode) {
				case ShaderConfiguration::Rasterization::FillMode::Wireframe:
					psoDesc.RasterizerState.FillMode = D3D12_FILL_MODE_WIREFRAME;
					break;
				case ShaderConfiguration::Rasterization::FillMode::Solid:
					psoDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
					break;
				}

				psoDesc.RasterizerState.FrontCounterClockwise = configuration.rasterization.counterClockwise;

			}
			// Blend
			{
				psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);

				psoDesc.BlendState.RenderTarget[0].BlendEnable = configuration.blend.enabled;
				psoDesc.BlendState.RenderTarget[0].SrcBlend = GetBlendFunction(configuration.blend.sourceFunction);
				psoDesc.BlendState.RenderTarget[0].DestBlend = GetBlendFunction(configuration.blend.destinationFunction);
				psoDesc.BlendState.RenderTarget[0].BlendOp = GetBlendOp(configuration.blend.equation);

				/*transparencyBlendDesc.SrcBlendAlpha = D3D12_BLEND_ONE;
				transparencyBlendDesc.DestBlendAlpha = D3D12_BLEND_ZERO;
				transparencyBlendDesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;

				transparencyBlendDesc.LogicOp = D3D12_LOGIC_OP_NOOP;*/

				psoDesc.BlendState.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
				//psoDesc.BlendState.RenderTarget[0] = transparencyBlendDesc;
			}

			// Depth/Stencil 
			{
				psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
				psoDesc.DepthStencilState.DepthEnable = configuration.depthStencil.depthTest;
			}

			psoDesc.SampleMask = UINT_MAX;
			psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
			psoDesc.NumRenderTargets = 1;
			psoDesc.RTVFormats[0] = GetMainContext()->mBackBufferFormat;
			psoDesc.SampleDesc.Count = GetMainContext()->m4xMsaaState ? 4 : 1;
			psoDesc.SampleDesc.Quality = GetMainContext()->m4xMsaaState ? (GetMainContext()->m4xMsaaQuality - 1) : 0;
			psoDesc.DSVFormat = GetMainContext()->mDepthStencilFormat;
			ThrowIfFailed(GetMainContext()->md3dDevice->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&mPSO)));
			VWOLF_CORE_ASSERT(mPSO.Get());
		}

		D3D12_BLEND GetBlendFunction(ShaderConfiguration::Blend::Function function) {
			switch (function) {
			case ShaderConfiguration::Blend::Function::Zero: return D3D12_BLEND_ZERO;
			case ShaderConfiguration::Blend::Function::One: return D3D12_BLEND_ONE;
			case ShaderConfiguration::Blend::Function::SrcColor: return D3D12_BLEND_SRC_COLOR;
			case ShaderConfiguration::Blend::Function::InvSrcColor: return D3D12_BLEND_INV_SRC_COLOR;
			case ShaderConfiguration::Blend::Function::DstColor: return D3D12_BLEND_DEST_COLOR;
			case ShaderConfiguration::Blend::Function::InvDstColor: return D3D12_BLEND_INV_DEST_COLOR;
			case ShaderConfiguration::Blend::Function::SrcAlpha: return D3D12_BLEND_SRC_ALPHA;
			case ShaderConfiguration::Blend::Function::InvSrcAlpha: return D3D12_BLEND_INV_SRC_ALPHA;
			case ShaderConfiguration::Blend::Function::DstAlpha: return D3D12_BLEND_DEST_ALPHA;
			case ShaderConfiguration::Blend::Function::InvDstAlpha: return D3D12_BLEND_INV_DEST_ALPHA;
			case ShaderConfiguration::Blend::Function::Src1Color: return D3D12_BLEND_SRC1_COLOR;
			case ShaderConfiguration::Blend::Function::InvSrc1Color: return D3D12_BLEND_INV_SRC1_COLOR;
			case ShaderConfiguration::Blend::Function::Src1Alpha: return D3D12_BLEND_SRC1_ALPHA;
			case ShaderConfiguration::Blend::Function::InvSrc1Alpha: return D3D12_BLEND_INV_SRC1_ALPHA;
			case ShaderConfiguration::Blend::Function::SrcAlphaSat: return D3D12_BLEND_SRC_ALPHA_SAT;
			case ShaderConfiguration::Blend::Function::CnstColor: return D3D12_BLEND_BLEND_FACTOR;
			case ShaderConfiguration::Blend::Function::InvCnstColor: return D3D12_BLEND_INV_BLEND_FACTOR;
				/*case ShaderConfiguration::Blend::Function::CnstAlpha: return GL_CONSTANT_ALPHA;
				case ShaderConfiguration::Blend::Function::InvCnstAlpha: return GL_ONE_MINUS_CONSTANT_ALPHA;*/
			}
			return D3D12_BLEND_ZERO;
		}

		D3D12_BLEND_OP GetBlendOp(ShaderConfiguration::Blend::Equation equation) {
			switch (equation) {
			case ShaderConfiguration::Blend::Equation::Add: return D3D12_BLEND_OP_ADD;
			case ShaderConfiguration::Blend::Equation::Substract: return D3D12_BLEND_OP_SUBTRACT;
			case ShaderConfiguration::Blend::Equation::ReverseSubstract: return D3D12_BLEND_OP_REV_SUBTRACT;
			case ShaderConfiguration::Blend::Equation::Min: return D3D12_BLEND_OP_MIN;
			case ShaderConfiguration::Blend::Equation::Max: return D3D12_BLEND_OP_MAX;
			}
			return D3D12_BLEND_OP_ADD;
		}
	private:
		std::string name;
		std::map<std::string, HLAttribute> attributes;
		std::map<std::string, Ref<HLConstantBuffer>> constantBuffers;

		Microsoft::WRL::ComPtr<ID3D12RootSignature> mRootSignature = nullptr;
		Microsoft::WRL::ComPtr<ID3D12PipelineState> mPSO = nullptr;
	};

	HLSLShader::HLSLShader(const char* name,
						   std::initializer_list<ShaderSource> otherShaders,
						   ShaderConfiguration configuration): Shader(name, otherShaders, configuration) {

		m_program = CreateRef<HLProgram>(name, otherShaders, configuration);

		// Constant Buffers
		uint32_t expectedObjects = 1000; // TODO: This is an expected amount, but I'm not satisfied with this.
		for (std::pair<std::string, Ref<HLConstantBuffer>> param : m_program->GetConstantBuffers()) {
			Ref<HLConstantBuffer> cb = param.second;
			ThrowIfFailed(GetMainContext()->md3dDevice->CreateCommittedResource(
				&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
				D3D12_HEAP_FLAG_NONE,
				&CD3DX12_RESOURCE_DESC::Buffer(CalcConstantBufferByteSize(cb->GetSize()) * expectedObjects), //TODO: This should not be the case but oh well.
				D3D12_RESOURCE_STATE_GENERIC_READ,
				nullptr,
				IID_PPV_ARGS(&cb->GetUploadBuffer())));

			ThrowIfFailed(cb->GetUploadBuffer()->Map(0, nullptr, reinterpret_cast<void**>(&cb->mMappedData))); // Check
			//ZeroMemory(m_cbContext->mMappedData, CalcConstantBufferByteSize(size) * expectedObjects);
			VWOLF_CORE_ASSERT(cb->GetUploadBuffer());
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
		}
	}

	HLSLShader::~HLSLShader()
	{
	}

	void HLSLShader::Bind() const
	{
		GetMainContext()->mCommandList->SetGraphicsRootSignature(m_program->GetRootSignature().Get());
	}

	void HLSLShader::Unbind() const
	{
	}

	void HLSLShader::SetData(const void* data, const char* name, uint32_t size, uint32_t offset) {
		// TODO: The names of the constant buffers should be the same as the names of the uniform buffers in OpenGL
		Ref<HLConstantBuffer> cb = m_program->GetConstantBuffers()[std::string("cbPer") + std::string(name)];
		if (!cb) return;

		memcpy(&cb->mMappedData[offset * CalcConstantBufferByteSize(size)], data, size);
		// Attach root descriptor directly to the command list


		GetMainContext()->mCommandList->SetGraphicsRootConstantBufferView(cb->GetBindingIndex(), cb->GetUploadBuffer().Get()->GetGPUVirtualAddress() + (offset * CalcConstantBufferByteSize(size)));

		// Attach descriptor table via descriptor heap
		/*dx12SetDescriptorHeaps(m_context, m_cbContext->mSrvHeap);
		m_context->mCommandList->SetGraphicsRootDescriptorTable(0, m_cbContext->mSrvHeap->GetGPUDescriptorHandleForHeapStart());*/
	}

	std::vector<Ref<ShaderInput>> HLSLShader::GetMaterialInputs() const
	{
		Ref<HLConstantBuffer> material = m_program->GetConstantBuffers()[std::string("cbPer") + std::string(materialName)];
		if (!material) return std::vector<Ref<ShaderInput>>();

		std::vector<Ref<ShaderInput>> inputs;

		for (HLConstantBufferVariable variable : material->GetVariables()) {
			if (variable.GetShaderDataType() == ShaderDataType::None) continue;
			inputs.push_back(CreateRef<ShaderInput>(variable.GetName(),
				variable.GetShaderDataType(),
				variable.GetIndex(),
				ShaderDataTypeSize(variable.GetShaderDataType()),
				variable.GetOffset()));
		}
		return inputs;
	}

	size_t HLSLShader::GetMaterialSize() const
	{
		Ref<HLConstantBuffer> cb = m_program->GetConstantBuffers()[std::string("cbPer") + std::string(materialName)];
		if (!cb) return 0;
		return cb->GetSize();
	}

	std::vector<ShaderInput> HLSLShader::GetTextureInputs() const {
		return std::vector<ShaderInput>();
	}
	
	const char* HLSLShader::GetName() const
	{		
		return m_name;
	}
	Microsoft::WRL::ComPtr<ID3D12PipelineState> HLSLShader::GetPipeline()
	{
		return m_program->GetPSO();
	}
}
#endif
