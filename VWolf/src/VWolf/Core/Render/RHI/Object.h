//
//  Object.h
//  VWolf
//
//  Created by Enrique Moises on 9/10/26.
//

// https://github.com/MethanePowered/MethaneKit/tree/master

#pragma once

namespace VWolf::RHI {

    class Object {
    public:
        virtual void SetName(const std::string& _name) {
            name = _name;
        }
        const std::string& GetName() const { return name; }
        
        virtual std::string ToString() {
            std::string typeName = boost::typeindex::type_id_runtime(*this).pretty_name();

            std::stringstream ss;
            ss << static_cast<const void*>(this);
            std::string addressStr = ss.str();
            
            return typeName + " (name: " + name + ". Memory Address: " + addressStr + ")";
        }

        virtual size_t HashCode() {
            return boost::typeindex::type_id_with_cvr<decltype(this)>().hash_code();
        }

        virtual bool operator==(Object& object) {
            return this->name == object.name;
        }

        virtual bool operator!=(Object& object) {
            return this->name != object.name;
        }
    protected:
        std::string name;
    };

    class ObjectRegistry {
    public:
        ObjectRegistry() = default;

        virtual void AddObject(Ref<Object> object) {
            m_objects[object->GetName()] = object;
        }
        
        virtual void RemoveObject(Ref<Object> object) {
            m_objects.erase(object->GetName());
        }
        
        virtual Ref<Object> Get(const std::string& id) {
            return m_objects[id];
        }
        
        virtual bool HasKey(const std::string& id) {
            return m_objects.find(id) != m_objects.end();
        }
    public:
        template<typename T>
        Ref<T> Get(const std::string& id) {
            if (auto derivedPtr = std::dynamic_pointer_cast<T>(Get(id))) {
                return derivedPtr;
            }
            return nullptr;
        }
        
        template<typename T>
        std::vector<Ref<T>> Filter() {
            std::vector<Ref<T>> filtered;
            
            for(auto& [key, object]: m_objects) {
                if (auto derivedPtr = std::dynamic_pointer_cast<T>(object))
                    filtered.push_back(derivedPtr);
            }
            return filtered;
        }
    protected:
        std::map<std::string, Ref<Object>> m_objects;
    private:
        ObjectRegistry(const ObjectRegistry&) = delete;
        ObjectRegistry(ObjectRegistry&&) = delete;
    };
}
