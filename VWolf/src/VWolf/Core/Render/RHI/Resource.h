//
//  Resource.h
//  VWolf
//
//  Created by Enrique Moises on 8/26/26.
//

#pragma once

#include "Object.h"

namespace VWolf::RHI {

    enum class ResourceType {
        Buffer,
        Texture,
        Sampler
    };

    enum class ResourceUsage {
        ShaderRead = 1 << 0,
        ShaderWrite = 1 << 1,
        RenderTarget = 1 << 2
    };
    
    inline ResourceUsage operator&(ResourceUsage lhs, ResourceUsage rhs) {
        using T = std::underlying_type_t<ResourceUsage>;
        return static_cast<ResourceUsage>(static_cast<T>(lhs) & static_cast<T>(rhs));
    }

    inline ResourceUsage operator|(ResourceUsage lhs, ResourceUsage rhs) {
        using T = std::underlying_type_t<ResourceUsage>;
        return static_cast<ResourceUsage>(static_cast<T>(lhs) | static_cast<T>(rhs));
    }

    inline ResourceUsage& operator&=(ResourceUsage& lhs, ResourceUsage rhs) {
        lhs = lhs & rhs;
        return lhs;
    }

    inline ResourceUsage& operator|=(ResourceUsage& lhs, ResourceUsage rhs) {
        lhs = lhs | rhs;
        return lhs;
    }

    class SubResourceIndex {
    public:
        SubResourceIndex() = default;
        SubResourceIndex(uint32_t depthSlice,
                         uint32_t arrayIndex,
                         uint32_t mipLevel): depthSlice(depthSlice), arrayIndex(arrayIndex), mipLevel(mipLevel) {}
        
        const uint32_t& GetDepthSlice() const { return depthSlice; }
        const uint32_t& GetArrayIndex() const { return arrayIndex; }
        const uint32_t& GetMipLevel() const { return mipLevel; }
    private:
        uint32_t depthSlice = 0;
        uint32_t arrayIndex = 0;
        uint32_t mipLevel   = 0;
    };

    class SubResource {
    public:
        SubResource() = default;
        SubResource(const void* bytes, SubResourceIndex index): bytes(bytes), index(index) {}
        
        const void* GetBytes() const { return bytes; }
        const SubResourceIndex& GetIndex() const { return index; }
    private:
        const void* bytes = nullptr;
        SubResourceIndex index;
    };

    class ResourceView;

    class Resource: public Object {
    public:
        virtual const ResourceType GetResourceType() const = 0;
        
        virtual ResourceView GetResourceView() = 0;
    protected:
    private:
    };

    class ResourceView {
    public:
        ResourceView() = default;
        ResourceView(Ref<Resource> resource, const SubResourceIndex& index): resource(resource), index(index) {}
        
        const Ref<Resource> GetResource() const { return resource; }
        const SubResourceIndex& GetIndex() { return index; }
    private:
        Ref<Resource> resource;
        SubResourceIndex index;
    };
}
