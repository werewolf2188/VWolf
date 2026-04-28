#include "vwpch.h"
#ifdef VWOLF_PLATFORM_WINDOWS
#include "HLSLShader.h"

#include "VWolf/Platform/DXILHelpers.h"
#include "VWolf/Platform/DirectX12/DirectX12Driver.h"

#include "VWolf/Platform/DirectX12/Core/DX12Core.h"
#include "VWolf/Platform/DirectX12/Core/DX12Device.h"
#include "VWolf/Platform/DirectX12/Core/DX12Command.h"
#include "VWolf/Platform/DirectX12/Core/DX12Surface.h"
#include "VWolf/Platform/DirectX12/Core/DX12Resources.h"

namespace VWolf {

	// TODO: Should I leave this here?
	bool UseDescriptorTable = true;

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
		HLAttribute(DXIL::Attribute& attribute):
			name(attribute.GetName()),
			index(attribute.GetIndex()), 
			offset(attribute.GetOffset()), 
			elementType(attribute.GetElementType()), 
			numberOfElements(attribute.GetNumberOfElements()),
			format((DXGI_FORMAT)attribute.GetFormat()){};

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
		HLConstantBufferVariable(DXIL::ConstantBufferVariable& cbv):
			name(cbv.GetName()), offset(cbv.GetOffset()), size(cbv.GetSize()), index(cbv.GetIndex()),
			typeDesc({ (D3D_SHADER_VARIABLE_CLASS)cbv.GetClass(), (D3D_SHADER_VARIABLE_TYPE)cbv.GetType(), 1, cbv.GetColumns(), 0, 0, 0, NULL }) {
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

		D3D12_SHADER_VARIABLE_DESC desc;
		D3D12_SHADER_TYPE_DESC typeDesc;
	};

	class HLConstantBuffer {
	public:
		HLConstantBuffer() = default;
		HLConstantBuffer(D3D12_SHADER_BUFFER_DESC desc, UINT bindingIndex): 
			name(desc.Name), size(desc.Size), bindingIndex(bindingIndex) {}
		HLConstantBuffer(DXIL::ConstantBuffer& cb):
			name(cb.GetName()), size(cb.GetSize()), bindingIndex(cb.GetBindingIndex()) {
		}

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
			return uploadBuffer->GetResource();
		}

		void CreateUploadBuffer(UINT size, UINT items, bool useDescriptorTable) {
			DX12BufferResourceInfo info;
			info.CreateBufferResourceDescription(true);
			uploadBuffer = CreateRef<DX12BufferResource>(info);
			uploadBuffer->CreateBuffer(DirectX12Driver::GetCurrent()->GetDevice(), CalcConstantBufferByteSize(size) * items);

			if (useDescriptorTable) {
				handles.resize(items);
				
				for (int index = 0; index < items; index++) {

					D3D12_GPU_VIRTUAL_ADDRESS cbAddress = uploadBuffer->GetResource()->GetGPUVirtualAddress();
					cbAddress += index * CalcConstantBufferByteSize(size);

					D3D12_CONSTANT_BUFFER_VIEW_DESC* cbvDesc = new D3D12_CONSTANT_BUFFER_VIEW_DESC;
					cbvDesc->BufferLocation = cbAddress;
					cbvDesc->SizeInBytes = CalcConstantBufferByteSize(size);

					handles[index] = DirectX12Driver::GetCurrent()->GetShaderResourceViewDescriptorHeap()->Allocate();

					DirectX12Driver::GetCurrent()->GetDevice()->GetDevice()->CreateConstantBufferView(cbvDesc, handles[index].GetCPUAddress());
				}
			}
		}

		void SetData(const void* data, uint32_t size, uint32_t offset) {
			BYTE* mMappedData = nullptr;

			D3D12_RANGE range{};
			DXThrowIfFailed(GetUploadBuffer()->Map(0, &range, reinterpret_cast<void**>(&mMappedData)));
			memcpy(&mMappedData[offset * CalcConstantBufferByteSize(size)], data, size);
			GetUploadBuffer()->Unmap(0, &range);
		}

		void SetVariables(std::vector<DXIL::ConstantBufferVariable>& variables) {
			std::vector<HLConstantBufferVariable> newVariables;

			for (DXIL::ConstantBufferVariable& variable : variables) {
				newVariables.push_back(HLConstantBufferVariable(variable));
			}	
			this->variables = newVariables;
		}

		std::vector<HLConstantBufferVariable> GetVariables() {
			return variables;
		}

		DX12DescriptorHandle& GetHandle(UINT index) {
			return handles[index];
		}
	private:
		std::string name;
		size_t size;
		UINT bindingIndex;

		Ref<DX12BufferResource> uploadBuffer;
		std::vector<DX12DescriptorHandle> handles;

		std::vector<HLConstantBufferVariable> variables;
	};

	class HLTexture {
	public:
		HLTexture(): name(""), bindingIndex(0), returnType(D3D_RETURN_TYPE_UNORM), dimension(D3D_SRV_DIMENSION_UNKNOWN) {

		}
		HLTexture(DXIL::Texture& texture) :
			name(texture.GetName()), 
			bindingIndex(texture.GetBindingIndex()), 
			returnType((D3D_RESOURCE_RETURN_TYPE)texture.GetReturnType()),
			dimension((D3D_SRV_DIMENSION)texture.GetDimension()) { }
		~HLTexture() {

		}
	public:
		std::string GetName() { return name; }

		UINT GetBindingIndex() { return bindingIndex; }

		D3D_RESOURCE_RETURN_TYPE GetReturnType() { return returnType; }

		int GetTextureType() {
			switch (returnType) {
				case D3D_RETURN_TYPE_FLOAT: {
					switch (dimension) {
					case D3D_SRV_DIMENSION_TEXTURE2D: return (int)ShaderSamplerType::Sampler2D;
					case D3D_SRV_DIMENSION_TEXTURECUBE:
						return (int)ShaderSamplerType::SamplerCube;
					}
				}
				case D3D_RETURN_TYPE_SINT: {
					switch (dimension) {
					case D3D_SRV_DIMENSION_TEXTURE2D: return (int)ShaderSamplerType::Sampler2D;
					case D3D_SRV_DIMENSION_TEXTURECUBE:
						return (int)ShaderSamplerType::SamplerCube;
					}
				}									 
			}
			return -1;
		}

		D3D_SRV_DIMENSION GetDimension() { return dimension; }

		ShaderDataType GetShaderDataType() {
			switch (returnType) {
			case D3D_RETURN_TYPE_FLOAT: {
				switch (dimension) {
				case D3D_SRV_DIMENSION_TEXTURE1D: return ShaderDataType::Float;
				case D3D_SRV_DIMENSION_TEXTURE2D: return ShaderDataType::Float2;
				case D3D_SRV_DIMENSION_TEXTURE3D: return ShaderDataType::Float3;
				case D3D_SRV_DIMENSION_TEXTURECUBE: 
					return ShaderDataType::Float3;
				}				
			}
			case D3D_RETURN_TYPE_SINT: {
				switch (dimension) {
				case D3D_SRV_DIMENSION_TEXTURE1D: return ShaderDataType::Int;
				case D3D_SRV_DIMENSION_TEXTURE2D: return ShaderDataType::Int2;
				case D3D_SRV_DIMENSION_TEXTURE3D: return ShaderDataType::Int3;
				case D3D_SRV_DIMENSION_TEXTURECUBE: 
					return ShaderDataType::Int3;
				}
			}
			case D3D_RETURN_TYPE_UNORM: return ShaderDataType::Bool;
			}
			return ShaderDataType::None;
		}
	private:
		std::string name;
		UINT bindingIndex = 0;
		D3D_RESOURCE_RETURN_TYPE returnType;
		D3D_SRV_DIMENSION dimension;
	};

	class HLSampler {
	public:
		HLSampler() : name(""), bindingIndex(0), numSamples(0) {

		}

		HLSampler(DXIL::Sampler& sampler): name(sampler.GetName()), bindingIndex(sampler.GetBindingIndex()), numSamples(sampler.GetNumSamples()) {}
		~HLSampler() {

		}
	public:
		std::string GetName() { return name; }

		UINT GetBindingIndex() { return bindingIndex; }

		UINT GetNumSamples() { return numSamples; }
	private:
		std::string name;
		UINT bindingIndex = 0;
		UINT numSamples = 0;
	};

	class HLProgram {
	public:
		HLProgram(Shader& coreShader) :
			name(name) {
			for (Stage stage : coreShader.GetSubShader().GetStages()) {
				CompileNewShaderIntoDXIL(coreShader.GetName(), stage, coreShader.GetSubShader().GetCode());
			}
			BuildRootSignature(UseDescriptorTable);
			BuildPSO(
				coreShader.GetSettings().GetRasterization().GetCullEnabled(), coreShader.GetSettings().GetRasterization().GetCullMode(), coreShader.GetSettings().GetRasterization().GetFillMode(), coreShader.GetSettings().GetRasterization().GetCounterClockwise(),
				coreShader.GetSettings().GetBlend().GetEnabled(), coreShader.GetSettings().GetBlend().GetSourceFunction(), coreShader.GetSettings().GetBlend().GetDestinationFunction(), coreShader.GetSettings().GetBlend().GetEquation(),
				coreShader.GetSettings().GetDepthStencil().GetDepthTest(), coreShader.GetSettings().GetDepthStencil().GetDepthFunction()
			);
		}
	public:
		std::map<std::string, Ref<HLConstantBuffer>>& GetConstantBuffers() {
			return constantBuffers;
		}

		std::map<std::string, HLTexture>& GetTextures() {
			return textures;
		}

		std::map<std::string, HLSampler>& GetSamplers() {
			return samplers;
		}

		Microsoft::WRL::ComPtr<ID3D12RootSignature>& GetRootSignature() {
			return mRootSignature;
		}

		Microsoft::WRL::ComPtr<ID3D12PipelineState>& GetPSO() {
			return mPSO;
		}
	private:
		void ReflectFromDXIL(DXIL::Shader dxil) {
			for (DXIL::Attribute& attribute : dxil.GetStageInAttributes	()) {
				auto it = attributes.find(attribute.GetName());
				if (it == attributes.end()) {
					attributes[attribute.GetName()] = attribute;
				}
			}

			for (DXIL::Sampler& sampler : dxil.GetSamplers()) {
				auto it = samplers.find(sampler.GetName());
				if (it == samplers.end()) {
					samplers[sampler.GetName()] = sampler;
				}
			}

			for (DXIL::Texture& texture : dxil.GetTextures()) {
				auto it = textures.find(texture.GetName());
				if (it == textures.end()) {
					textures[texture.GetName()] = texture;
				}
			}

			for (DXIL::ConstantBuffer& buffer : dxil.GetConstantBuffers()) {
				auto it = constantBuffers.find(buffer.GetName());
				if (it == constantBuffers.end()) {
					constantBuffers[buffer.GetName()] = CreateRef<HLConstantBuffer>(buffer);
					constantBuffers[buffer.GetName()]->SetVariables(buffer.GetVariables());
				}
			}
		}

		void CompileNewShaderIntoDXIL(std::string name, Stage stage, std::string code) {
			DXIL::Shader newShader(name, stage, code, DXIL::Shader::ArgumentType::DirectX);
			ReflectFromDXIL(newShader);
			dxils.push_back(newShader);
		}

		void BuildRootSignature(bool useDescriptorTables) {
			std::vector<CD3DX12_ROOT_PARAMETER> slotRootParameter(constantBuffers.size() + textures.size());
			std::vector<CD3DX12_STATIC_SAMPLER_DESC> samplersRootParameter(samplers.size());

			/*if (name == "BlinnPhong") {
				VWOLF_CORE_INFO("Test");
			}*/

			for (auto [key,value] : constantBuffers) {
				if (useDescriptorTables) {
					CD3DX12_DESCRIPTOR_RANGE* cbRange = new CD3DX12_DESCRIPTOR_RANGE(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, value->GetBindingIndex());
					slotRootParameter[value->GetBindingIndex()].InitAsDescriptorTable(1, cbRange);
				}
				else {
					slotRootParameter[value->GetBindingIndex()].InitAsConstantBufferView(value->GetBindingIndex());
				}				
			}

			for (auto [key, value] : textures) {
				CD3DX12_DESCRIPTOR_RANGE* cbRange = new CD3DX12_DESCRIPTOR_RANGE(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, value.GetBindingIndex());
				slotRootParameter[value.GetBindingIndex()].InitAsDescriptorTable(1, cbRange);
			}

			for (auto [key, value] : samplers) {
				// TODO: Move this somewhere else
				if (key == "gsamShadow") {
					samplersRootParameter[value.GetBindingIndex()].Init(value.GetBindingIndex(), D3D12_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT);
				} else
					samplersRootParameter[value.GetBindingIndex()].Init(value.GetBindingIndex(), D3D12_FILTER_MIN_MAG_POINT_MIP_LINEAR);
			}

			CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc(slotRootParameter.size(), slotRootParameter.data(), samplersRootParameter.size(), samplersRootParameter.data(),
				D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

			Microsoft::WRL::ComPtr<ID3DBlob> serializedRootSig = nullptr;
			Microsoft::WRL::ComPtr<ID3DBlob> errorBlob = nullptr;
			HRESULT hr = D3D12SerializeRootSignature(&rootSigDesc, D3D_ROOT_SIGNATURE_VERSION_1,
				serializedRootSig.GetAddressOf(), errorBlob.GetAddressOf());

			if (errorBlob != nullptr)
			{
				VWOLF_CORE_ERROR((char*)errorBlob->GetBufferPointer());
			}
			DXThrowIfFailed(hr);
			

			DXThrowIfFailed(DirectX12Driver::GetCurrent()->GetDevice()->GetDevice()->CreateRootSignature(
				0,
				serializedRootSig->GetBufferPointer(),
				serializedRootSig->GetBufferSize(),
				IID_PPV_ARGS(&mRootSignature)));
			VWOLF_CORE_ASSERT(mRootSignature);
		}

		void BuildPSO(
			bool cullEnabled, CullMode cullMode, FillMode fillMode, bool counterClockwise,
			bool blendEnabled, BlendFunction sourceBlend, BlendFunction destinationBlend, BlendEquation blendEquation,
			bool depthTestEnabled, DepthFunction depthFunction
			) {
			std::vector<D3D12_INPUT_ELEMENT_DESC> mInputLayout(attributes.size());
			
			for (auto& [key, value] : attributes)
			{
				mInputLayout[value.GetIndex()] = value.GetInputElementDesc();
			}

			D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc;
			ZeroMemory(&psoDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
			psoDesc.InputLayout = { mInputLayout.data(), (UINT)mInputLayout.size() };
			psoDesc.pRootSignature = mRootSignature.Get();

			for (auto& dxil : dxils) {
				switch (dxil.GetType()) {
				case ShaderType::Vertex:
					psoDesc.VS =
					{
						reinterpret_cast<BYTE*>(dxil.GetShader()->GetBufferPointer()),
						dxil.GetShader()->GetBufferSize()
					};
					break;
				case ShaderType::Fragment:
					psoDesc.PS =
					{
						reinterpret_cast<BYTE*>(dxil.GetShader()->GetBufferPointer()),
						dxil.GetShader()->GetBufferSize()
					};
					break;
				case ShaderType::Pre_Tesselator:
					psoDesc.HS =
					{
						reinterpret_cast<BYTE*>(dxil.GetShader()->GetBufferPointer()),
						dxil.GetShader()->GetBufferSize()
					};
					break;
				case ShaderType::Post_Tesselator:
					psoDesc.DS =
					{
						reinterpret_cast<BYTE*>(dxil.GetShader()->GetBufferPointer()),
						dxil.GetShader()->GetBufferSize()
					};
					break;
				case ShaderType::Geometry:
					psoDesc.GS =
					{
						reinterpret_cast<BYTE*>(dxil.GetShader()->GetBufferPointer()),
						dxil.GetShader()->GetBufferSize()
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
				if (cullEnabled) {
					switch (cullMode) {
					case CullMode::Front:
						psoDesc.RasterizerState.CullMode = D3D12_CULL_MODE_FRONT;
						break;
					case CullMode::Back:
						psoDesc.RasterizerState.CullMode = D3D12_CULL_MODE_BACK;
						break;
					case CullMode::FrontAndBack:
						break;
					}
				}
				else
					psoDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;

				switch (fillMode) {
				case FillMode::Wireframe:
					psoDesc.RasterizerState.FillMode = D3D12_FILL_MODE_WIREFRAME;
					break;
				case FillMode::Solid:
					psoDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
					break;
				}

				psoDesc.RasterizerState.FrontCounterClockwise = counterClockwise;
				if (name == "Shadow") { // TODO: Move this 
					psoDesc.RasterizerState.DepthBias = 100000;
					psoDesc.RasterizerState.DepthBiasClamp = 0.0f;
					psoDesc.RasterizerState.SlopeScaledDepthBias = 1.0f;
				}

			}
			// Blend
			{
				psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);

				D3D12_RENDER_TARGET_BLEND_DESC rtBlendDesc;

				rtBlendDesc.BlendEnable = blendEnabled;
				rtBlendDesc.LogicOpEnable = false;
				rtBlendDesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;//GetBlendFunction(sourceBlend);
				rtBlendDesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;//GetBlendFunction(destinationBlend);
				rtBlendDesc.BlendOp = GetBlendOp((BlendEquation)blendEquation);
				rtBlendDesc.SrcBlendAlpha = GetBlendFunction((BlendFunction)sourceBlend);
				rtBlendDesc.DestBlendAlpha = GetBlendFunction((BlendFunction)destinationBlend);
				rtBlendDesc.BlendOpAlpha = GetBlendOp((BlendEquation)blendEquation);
				rtBlendDesc.LogicOp = D3D12_LOGIC_OP_NOOP;
				rtBlendDesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

				psoDesc.BlendState.RenderTarget[0] = rtBlendDesc;
			}

			// Depth/Stencil 
			{
				psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
				psoDesc.DepthStencilState.DepthEnable = depthTestEnabled;
				switch (depthFunction) {
				case DepthFunction::Never:
					psoDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_NEVER;
					break;
				case DepthFunction::Less:
					psoDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
					break;
				case DepthFunction::LEqual:
					psoDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
					break;
				case DepthFunction::Equal:
					psoDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_EQUAL;
					break;
				case DepthFunction::NotEqual:
					psoDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_NOT_EQUAL;
					break;
				case DepthFunction::GEqual:
					psoDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_GREATER_EQUAL;
					break;
				case DepthFunction::Greater:
					psoDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_GREATER;
					break;
				case DepthFunction::Always:
					psoDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_ALWAYS;
					break;
				}

			}

			psoDesc.SampleMask = UINT_MAX;
			psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

			if (name == "Shadow") { // TODO: Move this 
				psoDesc.NumRenderTargets = 0;
			}
			else {
				psoDesc.NumRenderTargets = 1;
				psoDesc.RTVFormats[0] = DirectX12Driver::GetCurrent()->GetSurface()->GetFormat();
			}
			
			psoDesc.SampleDesc.Count = 1; // DirectX12Driver::GetCurrent()->GetDevice()->GetMSAAQuality() ? 4 : 1;
			psoDesc.SampleDesc.Quality = 0; // DirectX12Driver::GetCurrent()->GetDevice()->GetMSAAQuality() ? (DirectX12Driver::GetCurrent()->GetDevice()->GetMSAAQuality() - 1) : 0;
			psoDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT; // FOR NOW //DirectX12Driver::GetCurrent()->GetContext()->mDepthStencilFormat;
			DXThrowIfFailed(DirectX12Driver::GetCurrent()->GetDevice()->GetDevice()->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&mPSO)));
			VWOLF_CORE_ASSERT(mPSO.Get());
		}

		D3D12_BLEND GetBlendFunction(BlendFunction function) {
			switch (function) {
			case BlendFunction::Zero: return D3D12_BLEND_ZERO;
			case BlendFunction::One: return D3D12_BLEND_ONE;
			case BlendFunction::SrcColor: return D3D12_BLEND_SRC_COLOR;
			case BlendFunction::InvSrcColor: return D3D12_BLEND_INV_SRC_COLOR;
			case BlendFunction::DstColor: return D3D12_BLEND_DEST_COLOR;
			case BlendFunction::InvDstColor: return D3D12_BLEND_INV_DEST_COLOR;
			case BlendFunction::SrcAlpha: return D3D12_BLEND_SRC_ALPHA;
			case BlendFunction::InvSrcAlpha: return D3D12_BLEND_INV_SRC_ALPHA;
			case BlendFunction::DstAlpha: return D3D12_BLEND_DEST_ALPHA;
			case BlendFunction::InvDstAlpha: return D3D12_BLEND_INV_DEST_ALPHA;
			case BlendFunction::Src1Color: return D3D12_BLEND_SRC1_COLOR;
			case BlendFunction::InvSrc1Color: return D3D12_BLEND_INV_SRC1_COLOR;
			case BlendFunction::Src1Alpha: return D3D12_BLEND_SRC1_ALPHA;
			case BlendFunction::InvSrc1Alpha: return D3D12_BLEND_INV_SRC1_ALPHA;
			case BlendFunction::SrcAlphaSat: return D3D12_BLEND_SRC_ALPHA_SAT;
			case BlendFunction::CnstColor: return D3D12_BLEND_BLEND_FACTOR;
			case BlendFunction::InvCnstColor: return D3D12_BLEND_INV_BLEND_FACTOR;
				/*case ShaderConfiguration::Blend::Function::CnstAlpha: return GL_CONSTANT_ALPHA;
				case ShaderConfiguration::Blend::Function::InvCnstAlpha: return GL_ONE_MINUS_CONSTANT_ALPHA;*/
			}
			return D3D12_BLEND_ZERO;
		}

		D3D12_BLEND_OP GetBlendOp(BlendEquation equation) {
			switch (equation) {
			case BlendEquation::Add: return D3D12_BLEND_OP_ADD;
			case BlendEquation::Substract: return D3D12_BLEND_OP_SUBTRACT;
			case BlendEquation::ReverseSubstract: return D3D12_BLEND_OP_REV_SUBTRACT;
			case BlendEquation::Min: return D3D12_BLEND_OP_MIN;
			case BlendEquation::Max: return D3D12_BLEND_OP_MAX;
			}
			return D3D12_BLEND_OP_ADD;
		}
	private:
		std::vector<DXIL::Shader> dxils;

		std::string name;
		std::map<std::string, HLAttribute> attributes;
		std::map<std::string, Ref<HLConstantBuffer>> constantBuffers;
		std::map<std::string, HLTexture> textures;
		std::map<std::string, HLSampler> samplers;

		Microsoft::WRL::ComPtr<ID3D12RootSignature> mRootSignature = nullptr;
		Microsoft::WRL::ComPtr<ID3D12PipelineState> mPSO = nullptr;
	};

    HLSLShader::HLSLShader(Shader& coreShader): PShader(coreShader) {
		m_program = CreateRef<HLProgram>(coreShader);

		// Constant Buffers
		// TODO: This is an expected amount, but I'm not satisfied with this.
		// TODO: I should be able to let resources grow and shrink
		uint32_t expectedObjects = 100;
		for (std::pair<std::string, Ref<HLConstantBuffer>> param : m_program->GetConstantBuffers()) {
			Ref<HLConstantBuffer> cb = param.second;
			cb->CreateUploadBuffer(cb->GetSize(), expectedObjects, UseDescriptorTable);
			VWOLF_CORE_ASSERT(cb->GetUploadBuffer());
		}
    }

	HLSLShader::~HLSLShader()
	{
	}

	void HLSLShader::Bind() const
	{
		DirectX12Driver::GetCurrent()->GetCommands()->GetCommandList()->SetGraphicsRootSignature(m_program->GetRootSignature().Get());
	}

	void HLSLShader::Unbind() const
	{
	}

	void HLSLShader::SetData(const void* data, const char* name, uint32_t size, uint32_t offset) {
		// TODO: The names of the constant buffers should be the same as the names of the uniform buffers in OpenGL
		// TODO: Rename the variables
		Ref<HLConstantBuffer> cb = m_program->GetConstantBuffers()[std::string("cbPer") + std::string(name)];
		if (!cb) return;

		cb->SetData(data, size, offset);

		if (UseDescriptorTable) 
		{
			DirectX12Driver::GetCurrent()->GetCommands()->GetCommandList()->SetGraphicsRootDescriptorTable(cb->GetBindingIndex(), cb->GetHandle(offset).GetGPUAddress());
		}
		else 
		{
			DirectX12Driver::GetCurrent()->GetCommands()->GetCommandList()->SetGraphicsRootConstantBufferView(cb->GetBindingIndex(), cb->GetUploadBuffer().Get()->GetGPUVirtualAddress() + (offset * CalcConstantBufferByteSize(size)));
		}
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
		std::vector<ShaderInput> inputs;

		for (auto variable : m_program->GetTextures()) {
			inputs.push_back(ShaderInput(variable.second.GetName(),
				variable.second.GetShaderDataType(),
				variable.second.GetBindingIndex(),
				variable.second.GetTextureType(),
				0));
		}
		return inputs;
	}
	
	std::string HLSLShader::GetName() const
	{		
		return m_name;
	}
	Microsoft::WRL::ComPtr<ID3D12PipelineState> HLSLShader::GetPipeline()
	{
		return m_program->GetPSO();
	}
}
#endif
