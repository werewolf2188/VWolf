//
//  Material.hpp
//  VWolf
//
//  Created by Enrique Ricalde on 1/19/23.
//

#pragma once

#include "Shader.h"
#include "Texture.h"
#include "VWolf/Core/Object.h"
#include "VWolf/Core/Math/VMath.h"

#include "VWolf/Core/Utils/GenericSerialization.h"

namespace VWolf {
    class Material: public Object {
    public:
        Material(): Object(UUID::NewUUID()) {};
        Material(std::filesystem::path path, UUID _id);
        Material(std::string name, std::string shaderName);
        Material(std::string name, Ref<Shader> shader);
        Material(const Material& material);
        Material(Material&& material);

        ~Material();
    public:
        bool IsDefault();
        std::string GetShaderName();
    public:
        Color& GetColor(std::string name);
        void SetColor(std::string name, Color color);
        Vector4& GetVector4(std::string name);
        void SetVector4(std::string name, Vector3 vector);
        float& GetFloat(std::string name);
        void SetFloat(std::string name, float floatNumber);
        Ref<Texture> GetTexture(std::string name);
        void SetTexture(std::string name, Ref<Texture> texture);
        
        void SetAsDefault();
        std::vector<Property> GetProperties();
        bool HasProperty(std::string name);
        const Property& GetPropertyBy(std::string name);
    public:
        void Save(std::filesystem::path path);
    public:
        static Ref<Material> Load(std::filesystem::path path, UUID _id);
#ifdef VWOLF_CORE
    public:
        void * GetDataPointer() const;
        size_t GetSize() const;
#endif
    public:
        Material& operator=(const Material& material);
    private:
        void InternalLoad(Ref<Shader> shader);
    private:
        bool isDefault = false;
        std::string shaderName;
        size_t size;
        std::map<std::string, Color> colors;
        std::map<std::string, Vector4> vectors;
        std::map<std::string, float> floats;
        std::map<std::string, Ref<Texture>> _textures;
        std::map<std::string, UUID> textures;
        
        std::map<std::string, std::tuple<uint32_t, uint32_t, uint32_t>> inputs_information;
        std::vector<Property> properties;
        
        BOOST_DESCRIBE_CLASS(Material, (Object), (), (name), (isDefault, shaderName, colors, vectors, floats, textures))
        
        VWOLF_SERIALIZATION_FRIENDS(Material)
    };

    class MaterialLibrary {
    public:
        static Ref<Material> GetMaterial(std::string name);
        static Ref<Material> Default();
#ifdef VWOLF_CORE
        static void SetDefault(Ref<Material>);
        static void SetMaterial(std::string name, Ref<Material> material);
#endif
    private:
        static std::map<std::string, Ref<Material>> materials;
    };
}
