//
//  MetalBuffer.cpp
//  VWolf
//
//  Created by Enrique Moises on 8/19/26.
//

#include "vwpch.h"

#if defined(VWOLF_PLATFORM_MACOS) || defined(VWOLF_PLATFORM_IOS)
#include "MetalBuffer.h"
#include "MetalDevice.h"
#include "VWolf/Platform/Metal/Core/SmartPointers.h"

namespace VWolf::RHI  {

    MetalBuffer::MetalBuffer(Weak<MetalRenderContext> context, const BufferSettings& settings): Buffer(settings), context(context) {
        buffer = CreateMetalRef<MTL::Buffer, MTL::Resource>(context.lock()->GetDevice()->GetDevice()
                                                            ->newBuffer(settings.GetSize(), {}));
    }

    void MetalBuffer::SetData(const SubResource& subResource) {
        memcpy(buffer->contents(), subResource.GetBytes(), settings.GetSize());
    }

    ResourceView MetalBuffer::GetResourceView() {
        return ResourceView(shared_from_this(), {});
    }

    void MetalBuffer::SetName(const std::string& name) {
        Object::SetName(name);
        
        if (!name.empty() && buffer != nullptr) {
            buffer->setLabel(NSSTRINGFROM(name.data()));
        }
    }
}

#endif
