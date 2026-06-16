//
//  Object.cpp
//  VWolf
//
//  Created by Enrique Moises on 6/14/26.
//

#include "vwpch.h"
#include "Object.h"

#include <boost/type_index.hpp>

namespace VWolf {
    Object::Object(UUID id): id(id) {
    }

    Object::Object(const Object& other): id(other.id) {}

    Object::~Object() {
    }

    std::string Object::ToString() {
        std::string typeName = boost::typeindex::type_id_runtime(*this).pretty_name();

        std::stringstream ss;
        ss << static_cast<const void*>(this);
        std::string addressStr = ss.str();
        
        return typeName + " (id: " + id.ToString() + ". Memory Address: " + addressStr + ")";
    }

    size_t Object::HashCode() {
        return boost::typeindex::type_id_with_cvr<decltype(this)>().hash_code();
    }

    bool Object::operator==(Object& object) {
        return this->id == object.id;
    }

    bool Object::operator!=(Object& object) {
        return this->id != object.id;
    }

    Object& Object::operator=(const Object& object) {
        this->id = object.id;
        return *this;
    }

    void ObjectResourceManager::AddObject(const UUID& id, Ref<Object> object) {
        m_objects[id] = object;
    }

    Ref<Object> ObjectResourceManager::Get(const UUID& id) {
        return m_objects[id];
    }

    std::map<UUID, Ref<Object>> ObjectResourceManager::m_objects;
}
