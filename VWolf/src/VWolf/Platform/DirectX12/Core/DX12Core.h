#pragma once


#if defined( __cplusplus )

#if defined(_DEBUG)
#define DXThrowIfFailed(x) \
{\
    HRESULT hr__ = (x);\
    if(FAILED(hr__)) {\
        VWOLF_CORE_ERROR("DirectX 12 Error: File %s. Line: %s.", __FILE__, __LINE__);\
        VWOLF_CORE_ASSERT(false);\
        return;\
    }\
}

#define DXThrowIfFailedWithReturnValue(x, y) \
{\
    HRESULT hr__ = (x);\
    if(FAILED(hr__)) {\
        VWOLF_CORE_ERROR("DirectX 12 Error: File %s. Line: %s.", __FILE__, __LINE__);\
        VWOLF_CORE_ASSERT(false);\
        return y;\
    }\
}

#define DXNAME_D3D12_OBJECT(obj, name)\
obj->SetName(L##name);\
VWOLF_CORE_DEBUG("DirectX 12 Object name: %s", name);

#define DXNAME_D3D12_OBJECT_INDEXED(obj, name, i)\
obj->SetName(L##name);\
VWOLF_CORE_DEBUG("DirectX 12 Object name: %s %d", name, i);

#else

#define DXThrowIfFailed(x) x
#define DXThrowIfFailedWithReturnValue(x, y) x; return y;
#define DXNAME_D3D12_OBJECT(obj, name)
#define DXNAME_D3D12_OBJECT_INDEXED(obj, name, i)

#endif

#endif

//// TODO: Move later
template<typename T>
constexpr void SafeRelease(Microsoft::WRL::ComPtr<T> pointer) {
	if (pointer) {
		pointer->Release();
		//pointer = nullptr;
	}
}

template<class Mutex>
auto make_lock(Mutex& m)
{
    return std::lock_guard<Mutex>(m);
}


namespace VWolf {
    constexpr struct  {
        const D3D12_HEAP_PROPERTIES DefaultHeap {
                D3D12_HEAP_TYPE_DEFAULT, // Type
                D3D12_CPU_PAGE_PROPERTY_UNKNOWN, // CPU Page Property
                D3D12_MEMORY_POOL_UNKNOWN, // Memory Pool Reference
                0, // Creation Node Mask
                0 // Visible Node Mask
        };

        const D3D12_HEAP_PROPERTIES UploadHeap{
                D3D12_HEAP_TYPE_UPLOAD, // Type
                D3D12_CPU_PAGE_PROPERTY_UNKNOWN, // CPU Page Property
                D3D12_MEMORY_POOL_UNKNOWN, // Memory Pool Reference
                0, // Creation Node Mask
                0 // Visible Node Mask
        };
    } Heaps;
    
}