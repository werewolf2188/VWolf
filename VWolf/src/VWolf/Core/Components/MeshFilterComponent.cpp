//
//  MeshFilterComponent.cpp
//  VWolf
//
//  Created by Enrique Ricalde on 6/23/23.
//

#include "vwpch.h"
#include "MeshFilterComponent.h"
#include "VWolf/Core/Debug/ShapeHelper.h"

namespace VWolf {
    MeshFilterComponent::MeshFilterComponent():
    Component("Mesh Filter"), data(ShapeHelper::CreateTriangle()) {

    }

    MeshFilterComponent::MeshFilterComponent(std::filesystem::path data): path(path),
    Component("Mesh Filter") {
        Load();
    }

    MeshFilterComponent::MeshFilterComponent(MeshFilterComponent& component):
    Component("Mesh Filter"), data(component.data), path(component.path) {
        this->SetGameObject(component.GetGameObject());
    }

    MeshFilterComponent::MeshFilterComponent(MeshFilterComponent&& component):
    Component("Mesh Filter"), data(component.data), path(component.path) {
        this->SetGameObject(component.GetGameObject());
    }

    MeshFilterComponent::~MeshFilterComponent() {

    }

    void MeshFilterComponent::Load() {
        data = OBJLoader::Load(path.string(), path.filename().string());
    }

    void MeshFilterComponent::OnInspector() {
        MeshFilterComponent::componentInspector->OnInspector(this);
    }

    MeshFilterComponent& MeshFilterComponent::operator=(MeshFilterComponent t) {
        this->data = t.data;
        this->SetGameObject(t.GetGameObject());
        return *this;
    }

    Component* MeshFilterComponent::Copy(entt::entity& handle, entt::registry& registry) {
        MeshFilterComponent& component = registry.emplace<MeshFilterComponent>(handle, *this);
        return &component;
    }

    VWOLF_COMPONENT_INSPECTOR_IMPLEMENTATION(MeshFilterComponent);
}
