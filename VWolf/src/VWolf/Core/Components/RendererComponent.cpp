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
    RendererComponent::~RendererComponent() {}

    VWOLF_COMPONENT_INSPECTOR_IMPLEMENTATION(RendererComponent);

    MeshRendererComponent::MeshRendererComponent():
    RendererComponent("Mesh Renderer"), material(MaterialLibrary::Default()) {}

    MeshRendererComponent::MeshRendererComponent(Material& material):
    RendererComponent("Mesh Renderer"), material(&material) {}

    MeshRendererComponent::MeshRendererComponent(MeshRendererComponent& component):
    RendererComponent("Mesh Renderer"), material(component.material) {
        this->SetGameObject(component.GetGameObject());
    }
    MeshRendererComponent::MeshRendererComponent(MeshRendererComponent&& component):
    RendererComponent("Mesh Renderer"), material(component.material) {
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
    //        this->material = t.material;
        this->SetGameObject(t.GetGameObject());
        return *this;
    }

    VWOLF_COMPONENT_INSPECTOR_IMPLEMENTATION(MeshRendererComponent);

    // TODO: Test. Will remove later
    ShapeRendererComponent::ShapeRendererComponent():
    RendererComponent("Shape Renderer"), material(MaterialLibrary::Default()), data(ShapeHelper::CreateTriangle()) {}

    ShapeRendererComponent::ShapeRendererComponent(MeshData data, Material& material):
    RendererComponent("Shape Renderer"), data(data), material(&material) {}

    ShapeRendererComponent::ShapeRendererComponent(ShapeRendererComponent& component):
    RendererComponent("Shape Renderer"), data(component.data), material(component.material) {
        this->SetGameObject(component.GetGameObject());
    }
    ShapeRendererComponent::ShapeRendererComponent(ShapeRendererComponent&& component):
    RendererComponent("Shape Renderer"), data(component.data), material(component.material) {
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
//        this->material = t.material;
        this->SetGameObject(t.GetGameObject());
        return *this;
    }

    VWOLF_COMPONENT_INSPECTOR_IMPLEMENTATION(ShapeRendererComponent);
}
