//
//  MetalBuffer.h
//  VWolf
//
//  Created by Enrique Moises on 8/19/26.
//

#pragma once

#include "VWolf/Core/Base.h"
#include "VWolf/Platform/Metal/Core/Core.h"

#include "VWolf/Core/Render/RHI/Buffer.h"

#include "VWolf/Platform/Metal/Render/RHI/MetalRenderContext.h"

namespace VWolf::RHI  {
    class MetalBuffer: public Buffer, public Shareable<MetalBuffer>  {
    public:
        MetalBuffer(Weak<MetalRenderContext> context, const BufferSettings& settings);
        virtual void SetData(const SubResource& subResource) override;
        virtual ResourceView GetResourceView() override;
        virtual void SetName(const std::string& name) override;
        
        Ref<MTL::Buffer> GetBuffer() { return buffer; }
    protected:
    private:        
        Ref<MTL::Buffer> buffer;
        
        Weak<MetalRenderContext> context;
    };
}
