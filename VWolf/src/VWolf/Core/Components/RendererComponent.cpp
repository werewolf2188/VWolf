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
    data(ShapeHelper::CreateTriangle()), dataName(ShapeHelper::CreateTriangle().GetName()) {}

    ShapeRendererComponent::ShapeRendererComponent(MeshData data, Material& material):
    RendererComponent("Shape Renderer"), data(data), dataName(data.GetName()),
    material(&material), materialId(material.GetID())  {}

    ShapeRendererComponent::ShapeRendererComponent(const ShapeRendererComponent& component):
    RendererComponent("Shape Renderer", component.id), dataName(component.dataName), materialId(component.materialId) {
        this->material = ObjectResourceManager::Get<Material>(component.materialId);
        this->data = ShapeHelper::Create(dataName.c_str());
        this->SetGameObject(const_cast<ShapeRendererComponent&>(component).GetGameObject());
    }
    ShapeRendererComponent::ShapeRendererComponent(ShapeRendererComponent&& component):
    RendererComponent("Shape Renderer", component.id), dataName(component.dataName), materialId(component.materialId) {
        this->material = ObjectResourceManager::Get<Material>(component.materialId);
        this->data = ShapeHelper::Create(dataName.c_str());
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
        this->data = t.data;
        this->dataName = t.dataName;
        this->materialId = t.materialId;
        this->material = t.material;
        this->SetGameObject(const_cast<ShapeRendererComponent&>(t).GetGameObject());
        return *this;
    }

    void ShapeRendererComponent::SetShapeName(std::string name) {
        this->dataName = name;
        this->data = ShapeHelper::Create(dataName.c_str());
    }

    void ShapeRendererComponent::SetMaterial(Ref<Material> material) {
        this->material = material;
        this->materialId = material->GetID();
    }

    VWOLF_COMPONENT_INSPECTOR_IMPLEMENTATION(ShapeRendererComponent);

    VWOLF_CREATE_CONVERT_GENERIC_CLASS_ENCODER_WITH_NAME(ShapeRendererComponent, "ShapeRendererComponent")
}
