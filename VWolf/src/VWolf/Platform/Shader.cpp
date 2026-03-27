#include "vwpch.h"
#include "VWolf/Core/Render/Shader.h"
#include "VWolf/Core/Application.h"

#include "VWolf/Platform/OpenGL/Render/GLSLShader.h"
#ifdef VWOLF_PLATFORM_WINDOWS
#include "VWolf/Platform/DirectX12/Render/HLSLShader.h"
#endif
#if defined(VWOLF_PLATFORM_MACOS) || defined(VWOLF_PLATFORM_IOS)
#include "VWolf/Platform/Metal/Render/MSLShader.h"
#endif

#include "dxcapi.h"

#ifdef VWOLF_PLATFORM_WINDOWS
template<typename T>
using SmartPoint = Microsoft::WRL::ComPtr<T>;
#define DEREFERENCE(SP) SP.Get()
#elif defined(VWOLF_PLATFORM_MACOS)
template<typename T>
using SmartPoint = CComPtr<T>;
#define DEREFERENCE(SP) &*SP
#endif


namespace VWolf {

    std::vector<Ref<Shader>> ShaderLibrary::m_shaders;
    std::map<ShaderLibrary::ShaderSpecialty, std::string> ShaderLibrary::m_specialtiesShaders;

    // TODO: Remove
    const char* ShaderLibrary::CameraBufferName = "Camera";

    const char* ShaderLibrary::ObjectBufferName = "Object";
    //


    HRESULT LoadFileIntoBlob(IDxcUtils* pUtils, const wchar_t* filename, IDxcBlobEncoding** ppBlobEncoding) {
        uint32_t codePage = DXC_CP_ACP;
        return pUtils->LoadFile(filename, &codePage, ppBlobEncoding);
    }

    void ShaderLibrary::UseDirectXCompiler(std::string filename) {
        // Setup the converter using the appropriate facet
            // std::codecvt_utf8_utf16<wchar_t> is a common facet for this conversion
        std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
        std::wstring wide = converter.from_bytes(filename);
        
        const wchar_t* wFilename = wide.data();
        
        HRESULT hr;

        // 1. Initialize DXC components
        SmartPoint<IDxcUtils> utils;
        SmartPoint<IDxcCompiler3> compiler;
        SmartPoint<IDxcIncludeHandler> includeHandler;

        hr = DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&utils));
        if (FAILED(hr)) { /* handle error */ return; }
        hr = DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&compiler));
        if (FAILED(hr)) { /* handle error */ return; }
        
        // Create default include handler
        hr = utils->CreateDefaultIncludeHandler(&includeHandler);
        if (FAILED(hr)) { /* handle error */ return; }

        // 2. Load the HLSL source file
        SmartPoint<IDxcBlobEncoding> sourceBlob;
        // Assume "shader.hlsl" exists and contains your shader code
        hr = LoadFileIntoBlob(DEREFERENCE(utils), wFilename, &sourceBlob);
        if (FAILED(hr)) { /* handle error */ return; }

        DxcBuffer src = {
            sourceBlob->GetBufferPointer(),
            sourceBlob->GetBufferSize(),
            DXC_CP_ACP // Use the loaded encoding
        };
        
        // 3. Define compilation arguments (e.g., entry point, target profile, etc.)
        const char* entry = "PS";
        const wchar_t* lentry = L"PS";
        LPCWSTR arguments[] = {
            L"-E", lentry,         // Entry point name: "Main"
            L"-T", L"ps_6_0",       // Target profile: Vertex Shader Model 6.0
            L"-Zi",                 // Enable debug info (PDB)
            L"-Qstrip_debug",       // Strip debug info from the object code but keep it in the PDB
            L"-Fo", L"shader.cso"   // Output object file name
        };
        UINT32 argCount = _countof(arguments);

        // 4. Compile the shader
        SmartPoint<IDxcResult> result;
        hr = compiler->Compile(
            &src,                   // Source buffer
            arguments,              // Arguments
            argCount,               // Argument count
            DEREFERENCE(includeHandler),   // Include handler
            IID_PPV_ARGS(&result)   // Operation result
        );
        if (FAILED(hr)) { /* handle error */ return; }

        // 5. Handle the compilation result
        HRESULT compileStatus;
        result->GetStatus(&compileStatus);
        if (FAILED(compileStatus)) {
            SmartPoint<IDxcBlobUtf8> errors;
            result->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(&errors), nullptr);
            if (errors && errors->GetStringPointer()) {
                std::wcerr << L"Compilation failed with errors:\n" << errors->GetStringPointer() << std::endl;
            }
            return;
        }

        // 6. Retrieve the compiled shader bytecode (object code)
        SmartPoint<IDxcBlob> pShader;
        result->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&pShader), nullptr);
        
        if (pShader != nullptr) {
            // Save the shader binary to a file (e.g., shader.cso) or use it directly in your application
            // ... (file writing code omitted for brevity)
            std::wcout << L"Compilation successful. Generated shader bytecode size: " << pShader->GetBufferSize() << L" bytes." << std::endl;
        }
        
        SmartPoint<IDxcBlob> pPdb;
        result->GetOutput(DXC_OUT_PDB, IID_PPV_ARGS(&pPdb), nullptr);
        
        if (pPdb != nullptr) {
            // Save the shader binary to a file (e.g., shader.cso) or use it directly in your application
            // ... (file writing code omitted for brevity)
            std::wcout << L"Compilation successful. Generated pdb bytecode size: " << pPdb->GetBufferSize() << L" bytes." << std::endl;
        }
        
        // Reflection
        SmartPoint<IDxcContainerReflection> reflection;
        hr = DxcCreateInstance(CLSID_DxcContainerReflection, IID_PPV_ARGS(&reflection));
        if (FAILED(hr)) { /* handle error */ return; }
        
        hr = reflection->Load(DEREFERENCE(pShader));
        if (FAILED(hr)) { /* handle error */ return; }
        UINT32 parts = 0, partType = 0;
        hr = reflection->GetPartCount(&parts);
        if (FAILED(hr)) { /* handle error */ return; }
        std::cout << "Parts: " << parts << std::endl;
        
        for (UINT32 idx = 0; idx < parts; idx++) {
            hr = reflection->GetPartKind(idx, &partType);
            if (FAILED(hr)) { /* handle error */ return; }
            std::cout << "Part type: " << partType << std::endl;
        }
        
        // PDB or DXIL
        SmartPoint<IDxcPdbUtils> debug;
        hr = DxcCreateInstance(CLSID_DxcPdbUtils, IID_PPV_ARGS(&debug));
        if (FAILED(hr)) { /* handle error */ return; }
        hr = debug->Load(DEREFERENCE(pPdb));
        if (FAILED(hr)) { /* handle error */ return; }
        UINT32 args = 0, source = 0;
        BSTR name;
        hr = debug->GetSourceCount(&source);
        if (FAILED(hr)) { /* handle error */ return; }
        hr = debug->GetArgCount(&args);
        
        for (UINT32 idx = 0; idx < source; idx++) {
            hr = debug->GetSourceName(idx, &name);
            if (FAILED(hr)) { /* handle error */ return; }
            std::wcout << "Arg name: " << name << std::endl;
        }
        
        for (UINT32 idx = 0; idx < args; idx++) {
            hr = debug->GetArg(idx, &name);
            if (FAILED(hr)) { /* handle error */ return; }
            std::wcout << "Arg name: " << name << std::endl;
        }
        
#if defined(VWOLF_PLATFORM_MACOS) || defined(VWOLF_PLATFORM_IOS)
        IRCompiler* pCompiler = IRCompilerCreate();
        IRCompilerSetEntryPointName(pCompiler, entry);
        
        
        IRObject* pDXIL = IRObjectCreateFromDXIL(static_cast<uint8_t*>(pShader->GetBufferPointer()), pShader->GetBufferSize(), IRBytecodeOwnershipNone);
        // Compile DXIL to Metal IR:
        IRError* pError = nullptr;
        IRObject* pOutIR = IRCompilerAllocCompileAndLink(pCompiler, NULL,  pDXIL, &pError);
        std::cout << "Object type: " << IRObjectGetType(pDXIL) << std::endl;

        if (!pOutIR)
        {
          // Inspect pError to determine cause.
          IRErrorDestroy( pError );
        }
//        IRMetalLibBinary* pMetallib = IRMetalLibBinaryCreate();
//        IRObjectGetMetalLibBinary(pOutIR, IRShaderStageVertex, pMetallib);
//        size_t metallibSize = IRMetalLibGetBytecodeSize(pMetallib);
//        uint8_t* metallib = new uint8_t[metallibSize];
//        IRMetalLibGetBytecode(pMetallib, metallib);
        
        // Reflection
        IRShaderReflection* vsReflection = IRShaderReflectionCreate();
        IRShaderReflection* fsReflection = IRShaderReflectionCreate();
        IRObjectGetReflection(pOutIR, IRShaderStageVertex, vsReflection);
        IRObjectGetReflection(pOutIR, IRShaderStageFragment, fsReflection);
        std::string entryPointName = IRShaderReflectionGetEntryPointFunctionName(fsReflection);
        std::cout << entryPointName << std::endl;
        entryPointName = IRShaderReflectionGetEntryPointFunctionName(fsReflection);
        std::cout << entryPointName << std::endl;
        IRVersionedVSInfo vsinfo;
        IRVersionedFSInfo fsinfo;
        
        if (IRShaderReflectionCopyVertexInfo(vsReflection, IRReflectionVersion_1_0, &vsinfo))
        {
            for(int idx = 0; idx < vsinfo.info_1_0.num_vertex_inputs; idx++) {
                std::cout << vsinfo.info_1_0.vertex_inputs[idx].name << std::endl;
            }
            
            size_t resourcesCount = IRShaderReflectionGetResourceCount(vsReflection);
            IRResourceLocation* resourcesLocation = new IRResourceLocation[resourcesCount];
            IRShaderReflectionGetResourceLocations(vsReflection, resourcesLocation);
            for(int idx = 0; idx < resourcesCount; idx++) {
                std::cout << "type: " << resourcesLocation[idx].resourceType << ". top level offset " << resourcesLocation[idx].topLevelOffset << ". size: " << resourcesLocation[idx].sizeBytes << std::endl;
            }
            std::cout << IRShaderReflectionCopyJSONString(vsReflection) << std::endl;
            free(resourcesLocation);
            
            size_t functionConstantsCount = IRShaderReflectionGetFunctionConstantCount(vsReflection);
            IRFunctionConstant* functionConstants = (IRFunctionConstant*)calloc(functionConstantsCount, sizeof(IRFunctionConstant));
            IRShaderReflectionCopyFunctionConstants(vsReflection, functionConstants);
            for(int idx = 0; idx < functionConstantsCount; idx++) {
                std::cout << functionConstants[idx].name << std::endl;
            }
            IRShaderReflectionReleaseFunctionConstants(functionConstants, functionConstantsCount);
        }
        
        if (IRShaderReflectionCopyFragmentInfo(fsReflection, IRReflectionVersion_1_0, &fsinfo))
        {
            std::cout << "Render Targets" << fsinfo.info_1_0.num_render_targets << std::endl;
            
            size_t resourcesCount = IRShaderReflectionGetResourceCount(fsReflection);
            IRResourceLocation* resourcesLocation = new IRResourceLocation[resourcesCount];
            IRShaderReflectionGetResourceLocations(fsReflection, resourcesLocation);
            for(int idx = 0; idx < resourcesCount; idx++) {
                std::cout << "type: " << resourcesLocation[idx].resourceType << ". top level offset " << resourcesLocation[idx].topLevelOffset << ". size: " << resourcesLocation[idx].sizeBytes << std::endl;
            }
            std::cout << IRShaderReflectionCopyJSONString(fsReflection) << std::endl;
            free(resourcesLocation);
            
            size_t functionConstantsCount = IRShaderReflectionGetFunctionConstantCount(fsReflection);
            IRFunctionConstant* functionConstants = (IRFunctionConstant*)calloc(functionConstantsCount, sizeof(IRFunctionConstant));
            IRShaderReflectionCopyFunctionConstants(fsReflection, functionConstants);
            for(int idx = 0; idx < functionConstantsCount; idx++) {
                std::cout << functionConstants[idx].name << std::endl;
            }
            IRShaderReflectionReleaseFunctionConstants(functionConstants, functionConstantsCount);
        }
        
        // Clean up
        
        IRShaderReflectionReleaseVertexInfo(&vsinfo);
        IRShaderReflectionReleaseFragmentInfo(&fsinfo);
        IRShaderReflectionDestroy(vsReflection);
        IRShaderReflectionDestroy(fsReflection);
        // Store the metallib to custom format or disk, or use to create a MTLLibrary.

//        delete [] metallib;
//        IRMetalLibBinaryDestroy(pMetallib);
        IRObjectDestroy(pDXIL);
        IRObjectDestroy(pOutIR);
        IRCompilerDestroy(pCompiler);
        pCompiler = nullptr;
#endif
        
        // You can also retrieve PDB data and reflection data similarly
        // ...
        VWOLF_CORE_DEBUG("Lol");
    }

    void ShaderLibrary::LoadShader(std::string name,
                                   std::initializer_list<ShaderSource> otherShaders,
                                   ShaderConfiguration configuration) {
        Ref<Shader> shader;
        
        switch(Application::GetApplication()->GetDriverType()) {
            case DriverType::OpenGL:
                shader = CreateRef<GLSLShader>(name,
                                               otherShaders,
                                               configuration);
                break;
#ifdef VWOLF_PLATFORM_WINDOWS
            case DriverType::DirectX12:
                shader = CreateRef<HLSLShader>(name,
                                               otherShaders,
                                               configuration);
                break;
#endif
#if defined(VWOLF_PLATFORM_MACOS) || defined(VWOLF_PLATFORM_IOS)
            case DriverType::Metal:
                shader = CreateRef<MSLShader>(name,
                                              otherShaders,
                                              configuration);
                break;
#endif
            default:
                VWOLF_CORE_ASSERT(false, "Shader: Not yet implemented");
        }

        m_shaders.push_back(shader);
    }

    Ref<Shader> ShaderLibrary::GetShader(std::string name) {
        for (auto shader: m_shaders) {
            std::string shaderName = shader->GetName();
            if (shaderName == name) {
                return shader;
            }
        }
        return nullptr;
    }

    Ref<Shader> ShaderLibrary::GetShader(ShaderSpecialty type) {
        return GetShader(m_specialtiesShaders[type]);
    }

    void ShaderLibrary::SetShaderSpecialty(std::string name, ShaderSpecialty type) {
        m_specialtiesShaders[type] = name;
    }
}
