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
    RendererComponent("Mesh Renderer"), material(MaterialLibrary::Default()), materialName(MaterialLibrary::Default()->GetName()) {}

    MeshRendererComponent::MeshRendererComponent(Material& material):
    RendererComponent("Mesh Renderer"), material(&material), materialName(material.GetName()) {}

    MeshRendererComponent::MeshRendererComponent(MeshRendererComponent& component):
    RendererComponent("Mesh Renderer", component.id), materialName(component.materialName) {
        this->material = MaterialLibrary::GetMaterial(materialName);
        this->SetGameObject(component.GetGameObject());
    }
    MeshRendererComponent::MeshRendererComponent(MeshRendererComponent&& component):
    RendererComponent("Mesh Renderer", component.id), materialName(component.materialName) {
        this->material = MaterialLibrary::GetMaterial(materialName);
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

    MeshRendererComponent& MeshRendererComponent::operator=(MeshRendererComponent t) {
        this->materialName = t.materialName;
        this->material = MaterialLibrary::GetMaterial(t.materialName);
        this->SetGameObject(t.GetGameObject());
        return *this;
    }

    void MeshRendererComponent::SetMaterialName(std::string name) {
        this->materialName = name;
        this->material = MaterialLibrary::GetMaterial(name);
    }

    VWOLF_COMPONENT_INSPECTOR_IMPLEMENTATION(MeshRendererComponent);

    VWOLF_CREATE_CONVERT_GENERIC_CLASS_ENCODER_WITH_NAME(MeshRendererComponent, "MeshRendererComponent")

    // TODO: Test. Will remove later
    ShapeRendererComponent::ShapeRendererComponent():
    RendererComponent("Shape Renderer"),
    material(MaterialLibrary::Default()), materialName(MaterialLibrary::Default()->GetName()),
    data(ShapeHelper::CreateTriangle()), dataName(ShapeHelper::CreateTriangle().GetName()) {}

    ShapeRendererComponent::ShapeRendererComponent(MeshData data, Material& material):
    RendererComponent("Shape Renderer"), data(data), dataName(data.GetName()),
    material(&material), materialName(material.GetName()) {}

    ShapeRendererComponent::ShapeRendererComponent(ShapeRendererComponent& component):
    RendererComponent("Shape Renderer", component.id), dataName(component.dataName), materialName(component.materialName) {
        this->material = MaterialLibrary::GetMaterial(materialName);
        this->data = ShapeHelper::Create(dataName.c_str());
        this->SetGameObject(component.GetGameObject());
    }
    ShapeRendererComponent::ShapeRendererComponent(ShapeRendererComponent&& component):
    RendererComponent("Shape Renderer", component.id), dataName(component.dataName), materialName(component.materialName) {
        this->material = MaterialLibrary::GetMaterial(materialName);
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

    ShapeRendererComponent& ShapeRendererComponent::operator=(ShapeRendererComponent t) {
        this->data = t.data;
        this->dataName = t.dataName;
        this->materialName = t.materialName;
        this->material = t.material;
        this->SetGameObject(t.GetGameObject());
        return *this;
    }

    void ShapeRendererComponent::SetMaterialName(std::string name) {
        this->materialName = name;
        this->material = MaterialLibrary::GetMaterial(name);
    }

    void ShapeRendererComponent::SetShapeName(std::string name) {
        this->dataName = name;
        this->data = ShapeHelper::Create(dataName.c_str());
    }

    VWOLF_COMPONENT_INSPECTOR_IMPLEMENTATION(ShapeRendererComponent);

    VWOLF_CREATE_CONVERT_GENERIC_CLASS_ENCODER_WITH_NAME(ShapeRendererComponent, "ShapeRendererComponent")
}
