#include "vwpch.h"
#ifdef VWOLF_PLATFORM_WINDOWS
#include "HLSLShader.h"

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
		HLAttribute(D3D12_SIGNATURE_PARAMETER_DESC desc, UINT index, UINT offset): 
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
		HLConstantBufferVariable(ID3D12ShaderReflectionVariable* vCb, UINT index): index(index) {
			// if NULL, throw exception
			ID3D12ShaderReflectionType* vCbType = vCb->GetType();

			D3D12_SHADER_VARIABLE_DESC vCbDesc;
			ZeroMemory(&vCbDesc, sizeof(D3D12_SHADER_VARIABLE_DESC));
			vCb->GetDesc(&vCbDesc);

			//VWOLF_CORE_DEBUG("Constant buffer variable %s in index :%d, offset %d, size %d", vCbDesc.Name, index, vCbDesc.StartOffset, vCbDesc.Size);

			D3D12_SHADER_TYPE_DESC vTypeCbDesc;
			ZeroMemory(&vTypeCbDesc, sizeof(D3D12_SHADER_TYPE_DESC));
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

		D3D12_SHADER_VARIABLE_DESC desc;
		D3D12_SHADER_TYPE_DESC typeDesc;
	};

	class HLConstantBuffer {
	public:
		HLConstantBuffer() = default;
		HLConstantBuffer(D3D12_SHADER_BUFFER_DESC desc, UINT bindingIndex): 
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

		void SetVariables(std::vector<HLConstantBufferVariable> variables) {
			this->variables = variables;
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
		HLTexture(D3D12_SHADER_INPUT_BIND_DESC& desc): 
			name(desc.Name), bindingIndex(desc.BindPoint), returnType(desc.ReturnType), dimension(desc.Dimension) { }
		~HLTexture() {

		}
	public:
		std::string GetName() { return name; }

		UINT GetBindingIndex() { return bindingIndex; }

		D3D_RESOURCE_RETURN_TYPE GetReturnType() { return returnType; }

		D3D_SRV_DIMENSION GetDimension() { return dimension; }

		ShaderDataType GetShaderDataType() {
			switch (returnType) {
			case D3D_RETURN_TYPE_FLOAT: {
				switch (dimension) {
				case D3D_SRV_DIMENSION_TEXTURE1D: return ShaderDataType::Float;
				case D3D_SRV_DIMENSION_TEXTURE2D: return ShaderDataType::Float2;
				case D3D_SRV_DIMENSION_TEXTURE3D: return ShaderDataType::Float3;
				}				
			}
			case D3D_RETURN_TYPE_SINT: {
				switch (dimension) {
				case D3D_SRV_DIMENSION_TEXTURE1D: return ShaderDataType::Int;
				case D3D_SRV_DIMENSION_TEXTURE2D: return ShaderDataType::Int2;
				case D3D_SRV_DIMENSION_TEXTURE3D: return ShaderDataType::Int3;
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
		HLSampler(D3D12_SHADER_INPUT_BIND_DESC& desc) :
			name(desc.Name), bindingIndex(desc.BindPoint), numSamples(desc.NumSamples) { }
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
			BuildRootSignature(UseDescriptorTable);
			BuildPSO(shaderBlobs, configuration);
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
		void ReflectHLSL(Microsoft::WRL::ComPtr<ID3DBlob> byteCode) {
			// Decompiling
			HRESULT hr = S_OK;
			ID3D12ShaderReflection* pReflector = NULL;			
			hr = D3DReflect(byteCode->GetBufferPointer(), byteCode->GetBufferSize(), IID_ID3D12ShaderReflection, (void**)&pReflector);
			DXThrowIfFailed(hr);

			// Getting description
			D3D12_SHADER_DESC desc;
			ZeroMemory(&desc, sizeof(D3D12_SHADER_DESC));
			hr = pReflector->GetDesc(&desc);
			DXThrowIfFailed(hr);

			// Getting attributes
			UINT offset = 0;
			if (attributes.size() == 0) { // TODO: Change the condition
				for (UINT index = 0; index < desc.InputParameters; index++) {
					D3D12_SIGNATURE_PARAMETER_DESC paramDesc;
					ZeroMemory(&paramDesc, sizeof(D3D12_SIGNATURE_PARAMETER_DESC));
					hr = pReflector->GetInputParameterDesc(index, &paramDesc);
					DXThrowIfFailed(hr);

					HLAttribute attr(paramDesc, index, offset);
					offset += attr.GetSize();
					attributes[paramDesc.SemanticName] = attr;
				}
			}

			// Constant buffer
			for (UINT index = 0; index < desc.ConstantBuffers; index++) {
				ID3D12ShaderReflectionConstantBuffer* cb = pReflector->GetConstantBufferByIndex(index);
				D3D12_SHADER_BUFFER_DESC cbDesc;
				ZeroMemory(&cbDesc, sizeof(D3D12_SHADER_BUFFER_DESC));
				cb->GetDesc(&cbDesc);

				if (constantBuffers.count(cbDesc.Name)) continue;

				D3D12_SHADER_INPUT_BIND_DESC bindingDesc;
				ZeroMemory(&bindingDesc, sizeof(D3D12_SHADER_INPUT_BIND_DESC));
				pReflector->GetResourceBindingDescByName(cbDesc.Name, &bindingDesc);

				// Variables
				std::vector<HLConstantBufferVariable> variables;
				for (UINT cbIndex = 0; cbIndex < cbDesc.Variables; cbIndex++) {
					ID3D12ShaderReflectionVariable* vCb = cb->GetVariableByIndex(cbIndex);
					HLConstantBufferVariable variable(vCb, cbIndex);
					variables.push_back(variable);
				}

				Ref<HLConstantBuffer> cBuffer = CreateRef<HLConstantBuffer>(cbDesc, bindingDesc.BindPoint);
				cBuffer->SetVariables(variables);
				constantBuffers[cbDesc.Name] = cBuffer;
			}

			// Shader resource (textures) and samplers
			if (desc.TextureNormalInstructions > 0) { // This could be more than sampling
				for (UINT index = 0; index < desc.BoundResources; index++) {
					D3D12_SHADER_INPUT_BIND_DESC bindingDesc;
					pReflector->GetResourceBindingDesc(index, &bindingDesc);
					switch (bindingDesc.Type) {
					case D3D_SIT_TEXTURE: 
						{
							HLTexture texture(bindingDesc);
							textures[bindingDesc.Name] = texture;
						}
						break;
					case D3D_SIT_SAMPLER:
						{
							HLSampler sampler(bindingDesc);
							samplers[bindingDesc.Name] = sampler;
						}
						break;
					default: continue;
					}					
				}
			}
			

			pReflector->Release();
		}

		Microsoft::WRL::ComPtr<ID3DBlob> CompileShader(ShaderSource source) {

			const D3D_SHADER_MACRO* defines = nullptr;
			UINT compileFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)  
			compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION | D3DCOMPILE_SKIP_VALIDATION;
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
			DXThrowIfFailedWithReturnValue(hr, nullptr);
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

		void BuildRootSignature(bool useDescriptorTables) {
			std::vector<CD3DX12_ROOT_PARAMETER> slotRootParameter(constantBuffers.size() + textures.size());
			std::vector<CD3DX12_STATIC_SAMPLER_DESC> samplersRootParameter(samplers.size());

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
				samplersRootParameter[value.GetBindingIndex()].Init(value.GetBindingIndex(), D3D12_FILTER_MIN_MAG_MIP_POINT);
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

				D3D12_RENDER_TARGET_BLEND_DESC rtBlendDesc;

				rtBlendDesc.BlendEnable = configuration.blend.enabled;
				rtBlendDesc.LogicOpEnable = false;
				rtBlendDesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;//GetBlendFunction(configuration.blend.sourceFunction);
				rtBlendDesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;//GetBlendFunction(configuration.blend.destinationFunction);
				rtBlendDesc.BlendOp = GetBlendOp(configuration.blend.equation);

				rtBlendDesc.SrcBlendAlpha = GetBlendFunction(configuration.blend.sourceFunction);
				rtBlendDesc.DestBlendAlpha = GetBlendFunction(configuration.blend.destinationFunction);
				rtBlendDesc.BlendOpAlpha = GetBlendOp(configuration.blend.equation);
				rtBlendDesc.LogicOp = D3D12_LOGIC_OP_NOOP;
				rtBlendDesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

				psoDesc.BlendState.RenderTarget[0] = rtBlendDesc;
			}

			// Depth/Stencil 
			{
				psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
				psoDesc.DepthStencilState.DepthEnable = configuration.depthStencil.depthTest;
			}

			psoDesc.SampleMask = UINT_MAX;
			psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
			psoDesc.NumRenderTargets = 1;
			psoDesc.RTVFormats[0] = DirectX12Driver::GetCurrent()->GetSurface()->GetFormat();
			psoDesc.SampleDesc.Count = 1; // DirectX12Driver::GetCurrent()->GetDevice()->GetMSAAQuality() ? 4 : 1;
			psoDesc.SampleDesc.Quality = 0; // DirectX12Driver::GetCurrent()->GetDevice()->GetMSAAQuality() ? (DirectX12Driver::GetCurrent()->GetDevice()->GetMSAAQuality() - 1) : 0;
			psoDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT; // FOR NOW //DirectX12Driver::GetCurrent()->GetContext()->mDepthStencilFormat;
			DXThrowIfFailed(DirectX12Driver::GetCurrent()->GetDevice()->GetDevice()->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&mPSO)));
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
		std::map<std::string, HLTexture> textures;
		std::map<std::string, HLSampler> samplers;


		Microsoft::WRL::ComPtr<ID3D12RootSignature> mRootSignature = nullptr;
		Microsoft::WRL::ComPtr<ID3D12PipelineState> mPSO = nullptr;
	};

	HLSLShader::HLSLShader(const char* name,
						   std::initializer_list<ShaderSource> otherShaders,
						   ShaderConfiguration configuration): Shader(name, otherShaders, configuration) {

		m_program = CreateRef<HLProgram>(name, otherShaders, configuration);

		// Constant Buffers
		// TODO: This is an expected amount, but I'm not satisfied with this.
		// TODO: I should be able to let resources grow and shrink
		uint32_t expectedObjects = strcmp(name, "Grid") == 0 ? 1: 100; 
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
				1,
				0));
		}
		return inputs;
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
