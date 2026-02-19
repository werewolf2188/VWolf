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
#elif VWOLF_PLATFORM_MACOS
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
        LPCWSTR arguments[] = {
            L"-E", L"VS",         // Entry point name: "Main"
            L"-T", L"vs_6_0",       // Target profile: Vertex Shader Model 6.0
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
