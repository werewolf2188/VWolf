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
    Component("Mesh Filter"), data(ShapeHelper::Create(VWOLF_GET_SHAPE_NAME(ShapeHelper::Triangle))) {

    }

    MeshFilterComponent::MeshFilterComponent(std::filesystem::path data): path(path),
    Component("Mesh Filter") {
        Load();
    }

    MeshFilterComponent::MeshFilterComponent(UUID meshId): meshId(meshId),
    Component("Mesh Filter") {
//        Load();
    }

    MeshFilterComponent::MeshFilterComponent(const MeshFilterComponent& component):
    Component("Mesh Filter", component.id), data(component.data), path(component.path), meshId(component.meshId) {
        Load();
        this->SetGameObject(const_cast<MeshFilterComponent&>(component).GetGameObject());
    }

    MeshFilterComponent::MeshFilterComponent(MeshFilterComponent&& component):
    Component("Mesh Filter", component.id), data(component.data), path(component.path), meshId(component.meshId) {
        Load();
        this->SetGameObject(component.GetGameObject());
    }

    MeshFilterComponent::~MeshFilterComponent() {

    }

    void MeshFilterComponent::Load() {
        if (path == "") return;
        data = OBJLoader::Load(path.string(), path.filename().string());
    }

    void MeshFilterComponent::OnInspector() {
        MeshFilterComponent::componentInspector->OnInspector(this);
    }

    MeshFilterComponent& MeshFilterComponent::operator=(const MeshFilterComponent& t) {
        this->data = t.data;
        this->meshId = t.meshId;
        this->SetGameObject(const_cast<MeshFilterComponent&>(t).GetGameObject());
        return *this;
    }

    Component* MeshFilterComponent::Copy(entt::entity& handle, entt::registry& registry) {
        MeshFilterComponent& component = registry.emplace<MeshFilterComponent>(handle, *this);
        return &component;
    }

    VWOLF_COMPONENT_INSPECTOR_IMPLEMENTATION(MeshFilterComponent);
    VWOLF_CREATE_CONVERT_GENERIC_CLASS_ENCODER_WITH_NAME(MeshFilterComponent, "MeshFilterComponent")
}
