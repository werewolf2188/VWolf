//
//  Bindable.h
//  VWolf
//
//  Created by Enrique Moises on 10/6/26.
//

#pragma once

namespace VWolf {
    class Bindable {
    public:
        virtual void Bind() = 0;
        virtual void Unbind() = 0;
    };

    class BaseBindable {
    public:
        virtual void Bind(uint32_t base) = 0;
        virtual void Unbind(uint32_t base) = 0;
    };
}
