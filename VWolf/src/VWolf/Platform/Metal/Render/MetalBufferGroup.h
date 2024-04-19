//
//  MetalBufferGroup.hpp
//  VWolf
//
//  Created by Enrique Ricalde on 5/6/24.
//

#pragma once

#include "VWolf/Core/Base.h"
#include "VWolf/Core/Render/RenderStructs.h"
#include "VWolf/Platform/Metal/Core/Core.h"
#include "VWolf/Platform/Metal/Render/MetalBuffer.h"

namespace VWolf {
    class MetalBufferGroup {
    public:
        MetalBufferGroup(MeshData& mesh);
        ~MetalBufferGroup();
    public:
        Ref<MetalVertexBuffer> GetVertexBuffer() const { return vertexBuffer; }
        Ref<MetalIndexBuffer> GetIndexBuffer() const { return indexBuffer; }
    public:
        void SetData(MeshData& data);
    private:
        Ref<MetalVertexBuffer> vertexBuffer;
        Ref<MetalIndexBuffer> indexBuffer;
    };
}
