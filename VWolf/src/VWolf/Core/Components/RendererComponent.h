//
//  RendererComponent.hpp
//  VWolf
//
//  Created by Enrique Ricalde on 4/14/23.
//

#pragma once

#include "BaseComponent.h"
#include "VWolf/Core/Render/Mesh.h"
#include "VWolf/Core/Render/Material.h"

namespace VWolf {

    class RendererComponent: public Component {
    public:
        RendererComponent();
        RendererComponent(std::string name);
        RendererComponent(std::string name, UUID id);
        ~RendererComponent();
        BOOST_DESCRIBE_CLASS(RendererComponent, (Component), (), (id), ())
    };

    class MeshRendererComponent: public RendererComponent {
    public:
        MeshRendererComponent();
        MeshRendererComponent(Material& material);
        MeshRendererComponent(const MeshRendererComponent& component);
        MeshRendererComponent(MeshRendererComponent&& component);
        ~MeshRendererComponent();
    public:
        virtual Component* Copy(entt::entity& handle, entt::registry& registry) override;
    public:
        Material& GetMaterial() { return *material; }
        Ref<Material> GetMaterialEx() { return material; }
        std::string GetMaterialName() { return materialName; }
    public:
        void SetMaterial(Ref<Material> material);
    public:
        MeshRendererComponent& operator=(const MeshRendererComponent t);
    private:
        UUID materialId;
        Ref<Material> material;
        BOOST_DESCRIBE_CLASS(MeshRendererComponent, (RendererComponent), (), (id), (materialId))
        VWOLF_SERIALIZATION_FRIENDS(MeshRendererComponent)
    };

    // TODO: Test. Will remove later
    class ShapeRendererComponent: public RendererComponent {
    public:
        ShapeRendererComponent();
        ShapeRendererComponent(Ref<Mesh> mesh, Material& material);
        ShapeRendererComponent(const ShapeRendererComponent& component);
        ShapeRendererComponent(ShapeRendererComponent&& component);
        ~ShapeRendererComponent();
    public:
        virtual Component* Copy(entt::entity& handle, entt::registry& registry) override;
    public:
        Material& GetMaterial() { return *material; }
        Ref<Material> GetMaterialEx() { return material; }
        Ref<Mesh> GetMesh() { return mesh; };
    public:
        void SetMaterial(Ref<Material> material);
        void SetMesh(Ref<Mesh> mesh);
    public:
        ShapeRendererComponent& operator=(const ShapeRendererComponent& t);
    private:
        UUID materialId;
        UUID meshId;
        Ref<Material> material;
        Ref<Mesh> mesh;
        BOOST_DESCRIBE_CLASS(ShapeRendererComponent, (RendererComponent), (), (id), (materialId, meshId))
        VWOLF_SERIALIZATION_FRIENDS(ShapeRendererComponent)
    };
}

namespace YAML {
    VWOLF_CREATE_CONVERT_GENERIC_CLASS_DECODER(VWolf::MeshRendererComponent)
    VWOLF_CREATE_CONVERT_GENERIC_CLASS_DECODER(VWolf::ShapeRendererComponent)
}
