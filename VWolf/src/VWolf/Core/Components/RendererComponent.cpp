//
//  RendererComponent.cpp
//  VWolf
//
//  Created by Enrique Ricalde on 4/14/23.
//

#include "vwpch.h"
#include "RendererComponent.h"
#include "VWolf/Core/Debug/ShapeHelper.h"

namespace VWolf {
    RendererComponent::RendererComponent(): Component("Renderer") {}
    RendererComponent::RendererComponent(std::string name): Component(name) {}
    RendererComponent::RendererComponent(std::string name, UUID id): Component(name, id) {}
    RendererComponent::~RendererComponent() {}

    VWOLF_COMPONENT_INSPECTOR_IMPLEMENTATION(RendererComponent);

    MeshRendererComponent::MeshRendererComponent():
    RendererComponent("Mesh Renderer"), material(MaterialLibrary::Default()), materialId(MaterialLibrary::Default()->GetID()) {}

    MeshRendererComponent::MeshRendererComponent(Material& material):
    RendererComponent("Mesh Renderer"), material(&material), materialId(material.GetID()) {}

    MeshRendererComponent::MeshRendererComponent(const MeshRendererComponent& component):
    RendererComponent("Mesh Renderer", component.id), materialId(component.materialId) {
        this->material = ObjectResourceManager::Get<Material>(component.materialId);
        this->SetGameObject(const_cast<MeshRendererComponent&>(component).GetGameObject());
    }
    MeshRendererComponent::MeshRendererComponent(MeshRendererComponent&& component):
    RendererComponent("Mesh Renderer", component.id), materialId(component.materialId) {
        this->material = ObjectResourceManager::Get<Material>(component.materialId);
        this->SetGameObject(component.GetGameObject());
    }
    MeshRendererComponent::~MeshRendererComponent() {}

    void MeshRendererComponent::OnInspector() {
        MeshRendererComponent::componentInspector->OnInspector(this);
    }

    Component* MeshRendererComponent::Copy(entt::entity& handle, entt::registry& registry) {
        MeshRendererComponent& component = registry.emplace<MeshRendererComponent>(handle, *this);
        return &component;
    }

    void MeshRendererComponent::SetMaterial(Ref<Material> material) {
        this->material = material;
        this->materialId = material->GetID();
    }

    MeshRendererComponent& MeshRendererComponent::operator=(const MeshRendererComponent t) {
        this->materialId = t.materialId;
        this->material = ObjectResourceManager::Get<Material>(t.materialId);
        this->SetGameObject(const_cast<MeshRendererComponent&>(t).GetGameObject());
        return *this;
    }

    VWOLF_COMPONENT_INSPECTOR_IMPLEMENTATION(MeshRendererComponent);

    VWOLF_CREATE_CONVERT_GENERIC_CLASS_ENCODER_WITH_NAME(MeshRendererComponent, "MeshRendererComponent")

    // TODO: Test. Will remove later
    ShapeRendererComponent::ShapeRendererComponent():
    RendererComponent("Shape Renderer"),
    material(MaterialLibrary::Default()), materialId(MaterialLibrary::Default()->GetID()),
    mesh(CreateRef<Mesh>(ShapeHelper::Create(VWOLF_GET_SHAPE_NAME(ShapeHelper::Triangle)))), meshId(VWOLF_GET_SHAPE_ID(ShapeHelper::Triangle)) {}

    ShapeRendererComponent::ShapeRendererComponent(Ref<Mesh> mesh, Material& material):
    RendererComponent("Shape Renderer"), mesh(mesh), meshId(mesh->GetID()),
    material(&material), materialId(material.GetID())  {}

    ShapeRendererComponent::ShapeRendererComponent(const ShapeRendererComponent& component):
    RendererComponent("Shape Renderer", component.id),
    meshId(component.meshId), materialId(component.materialId) {
        this->material = ObjectResourceManager::Get<Material>(component.materialId);
        this->mesh = CreateRef<Mesh>(ShapeHelper::Create(meshId));
        this->SetGameObject(const_cast<ShapeRendererComponent&>(component).GetGameObject());
    }
    ShapeRendererComponent::ShapeRendererComponent(ShapeRendererComponent&& component):
    RendererComponent("Shape Renderer", component.id),
    meshId(component.meshId), materialId(component.materialId) {
        this->material = ObjectResourceManager::Get<Material>(component.materialId);
        this->mesh = CreateRef<Mesh>(ShapeHelper::Create(meshId));
        this->SetGameObject(component.GetGameObject());
    }
    ShapeRendererComponent::~ShapeRendererComponent() {}

    void ShapeRendererComponent::OnInspector() {
        ShapeRendererComponent::componentInspector->OnInspector(this);
    }

    Component* ShapeRendererComponent::Copy(entt::entity& handle, entt::registry& registry) {
        ShapeRendererComponent& component = registry.emplace<ShapeRendererComponent>(handle, *this);
        return &component;
    }

    ShapeRendererComponent& ShapeRendererComponent::operator=(const ShapeRendererComponent& t) {
        this->mesh = t.mesh;
        this->materialId = t.materialId;
        this->meshId = t.meshId;
        this->material = t.material;
        this->SetGameObject(const_cast<ShapeRendererComponent&>(t).GetGameObject());
        return *this;
    }

    void ShapeRendererComponent::SetMaterial(Ref<Material> material) {
        this->material = material;
        this->materialId = material->GetID();
    }

    void ShapeRendererComponent::SetMesh(Ref<Mesh> mesh) {
        this->mesh = mesh;
        this->meshId = mesh->GetID();
    }

    VWOLF_COMPONENT_INSPECTOR_IMPLEMENTATION(ShapeRendererComponent);

    VWOLF_CREATE_CONVERT_GENERIC_CLASS_ENCODER_WITH_NAME(ShapeRendererComponent, "ShapeRendererComponent")
}
