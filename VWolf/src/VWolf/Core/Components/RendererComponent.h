//
//  RendererComponent.hpp
//  VWolf
//
//  Created by Enrique Ricalde on 4/14/23.
//

#pragma once

#include "BaseComponent.h"
#include "VWolf/Core/Render/RenderStructs.h"
#include "VWolf/Core/Render/Material.h"

namespace VWolf {

    class RendererComponent: public Component {
    public:
        RendererComponent();
        RendererComponent(std::string name);
        RendererComponent(std::string name, UUID id);
        ~RendererComponent();
        VWOLF_COMPONENT_INSPECTOR_DEFINE(RendererComponent);
        BOOST_DESCRIBE_CLASS(RendererComponent, (Component), (), (id), ())
    };

    class MeshRendererComponent: public RendererComponent {
    public:
        MeshRendererComponent();
        MeshRendererComponent(Material& material);
        MeshRendererComponent(MeshRendererComponent& component);
        MeshRendererComponent(MeshRendererComponent&& component);
        ~MeshRendererComponent();
    public:
        virtual void OnInspector() override;
        virtual Component* Copy(entt::entity& handle, entt::registry& registry) override;
    public:
        Material& GetMaterial() { return *material; }
        void SetMaterial(Material* material) { this->material = material; }
        std::string GetMaterialName() { return materialName; }
    public:
        void SetMaterialName(std::string name);
    public:
        MeshRendererComponent& operator=(MeshRendererComponent t);
    private:
        std::string materialName;
        Material* material;
        VWOLF_COMPONENT_INSPECTOR_DEFINE(MeshRendererComponent);
        BOOST_DESCRIBE_CLASS(MeshRendererComponent, (RendererComponent), (), (id), (materialName))
        VWOLF_SERIALIZATION_FRIENDS(MeshRendererComponent)
    };

    // TODO: Test. Will remove later
    class ShapeRendererComponent: public RendererComponent {
    public:
        ShapeRendererComponent();
        ShapeRendererComponent(MeshData data, Material& material);
        ShapeRendererComponent(ShapeRendererComponent& component);
        ShapeRendererComponent(ShapeRendererComponent&& component);
        ~ShapeRendererComponent();
    public:
        virtual void OnInspector() override;
        virtual Component* Copy(entt::entity& handle, entt::registry& registry) override;
    public:
        MeshData& GetData() { return data; }
        void SetData(MeshData data) { this->data = data; }
        Material& GetMaterial() { return *material; }
        void SetMaterial(Material* material) { this->material = material; }
        std::string GetMaterialName() { return materialName; }
        std::string GetShapeName() { return dataName; }
    public:
        void SetShapeName(std::string name);
        void SetMaterialName(std::string name);
    public:
        ShapeRendererComponent& operator=(ShapeRendererComponent t);
    private:
        std::string dataName;
        std::string materialName;
        
        MeshData data;
        Material* material;
        VWOLF_COMPONENT_INSPECTOR_DEFINE(ShapeRendererComponent);
        BOOST_DESCRIBE_CLASS(ShapeRendererComponent, (RendererComponent), (), (id), (dataName, materialName))
        VWOLF_SERIALIZATION_FRIENDS(ShapeRendererComponent)
    };
}

namespace YAML {
    VWOLF_CREATE_CONVERT_GENERIC_CLASS_DECODER(VWolf::MeshRendererComponent)
    VWOLF_CREATE_CONVERT_GENERIC_CLASS_DECODER(VWolf::ShapeRendererComponent)
}
