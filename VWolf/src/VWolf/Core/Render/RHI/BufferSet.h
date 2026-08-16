//
//  BufferSet.h
//  VWolf
//
//  Created by Enrique Moises on 8/19/26.
//

#pragma once

#include "Object.h"

namespace VWolf::RHI  {
    class Buffer;
    enum class BufferType;

    class BufferSet: public Object {
    public:
        
        const std::vector<Ref<Buffer>>& GetBuffers() { return buffers; }
        
        virtual void AddBuffers(const std::vector<Ref<Buffer>> _buffers) {
            buffers = _buffers;
        };
    protected:
        std::vector<Ref<Buffer>> buffers;
    private:
    };
}
