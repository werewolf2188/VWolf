//
//  SmartPointers.h
//  VWolf
//
//  Created by Enrique Moises on 8/12/26.
//

#pragma once

#include "VWolf/Core/Base.h"
#include "VWolf/Core/Utils/GenericSerialization.h"

namespace VWolf {
    template<typename T, typename B = NS::Object, typename ... Args>
    constexpr Ref<T> CreateMetalRef(Args&& ... args)
    {
        static_assert(std::is_base_of_v<NS::Referencing<T, B>, T>, "T must derive from NS::Referencing !");
        
        return Ref<T>(std::forward<Args>(args)..., [](T* object){
            
//            std::cout << "Releasing class: " << ClassNameCleaner::Current().GetClassName<T>() << std::endl;
            object->release();
        });
    }

    template<typename T, typename B = NS::Object, typename ... Args>
    constexpr Ref<T> CreateMetalUnownedRef(Args&& ... args)
    {
        static_assert(std::is_base_of_v<NS::Referencing<T, B>, T>, "T must derive from NS::Referencing !");
        
        return Ref<T>(std::forward<Args>(args)..., [](T* object){
            
//            std::cout << "Releasing class: " << ClassNameCleaner::Current().GetClassName<T>() << std::endl;
        });
    }
}
