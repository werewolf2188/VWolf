//
//  Object.h
//  VWolf
//
//  Created by Enrique Moises on 4/23/26.
//

#pragma once

#include "VWolf/Core/Base.h"
#include "VWolf/Core/Utils/UUID.h"
#include <boost/describe.hpp>

namespace VWolf {
    class Object {
    protected:
        Object(UUID id);
        Object(const Object& other);
    public:
        virtual ~Object();
    public:
        const UUID& GetID() const { return id; }
        const std::string& GetName() const { return name; }
    public:
        std::string ToString();
        size_t HashCode();
    public:
        Object& operator=(const Object& object);
        bool operator==(Object& object);
        bool operator!=(Object& object);
    protected:
        UUID id;
        std::string name = "";
        
        BOOST_DESCRIBE_CLASS(Object, (), (), (id, name), ())
    };

    class ObjectResourceManager {
    private:
        ObjectResourceManager() = delete;
        ObjectResourceManager(const ObjectResourceManager&) = delete;
        ObjectResourceManager(ObjectResourceManager&&) = delete;
    public:
        static void AddObject(const UUID& id, Ref<Object>);
        static Ref<Object> Get(const UUID& id);
    public:
        template<typename T>
        static Ref<T> Get(const UUID& id) {
            if (auto derivedPtr = std::dynamic_pointer_cast<T>(Get(id))) {
                return derivedPtr;
            }
            return nullptr;
        }
        
        template<typename T>
        static std::vector<Ref<T>> Filter() {
            std::vector<Ref<T>> filtered;
            
            for(auto& [key, object]: m_objects) {
                if (auto derivedPtr = std::dynamic_pointer_cast<T>(object))
                    filtered.push_back(derivedPtr);
            }
            return filtered;
        }
    private:
        static std::map<UUID, Ref<Object>> m_objects;
    };
}
