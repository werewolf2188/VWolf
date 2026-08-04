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
    Component(ClassNameCleaner::Current().GetClassName<MeshFilterComponent>()),
    mesh(CreateRef<Mesh>(ShapeHelper::Create(VWOLF_GET_SHAPE_NAME(ShapeHelper::Triangle)))), meshId(VWOLF_GET_SHAPE_ID(ShapeHelper::Triangle)) {

    }

    MeshFilterComponent::MeshFilterComponent(UUID meshId): meshId(meshId),
    Component(ClassNameCleaner::Current().GetClassName<MeshFilterComponent>()) {
        Load();
    }

    MeshFilterComponent::MeshFilterComponent(const MeshFilterComponent& component):
    Component(ClassNameCleaner::Current().GetClassName<MeshFilterComponent>(), component.id), meshId(component.meshId) {
        Load();
        this->SetGameObject(const_cast<MeshFilterComponent&>(component).GetGameObject());
    }

    MeshFilterComponent::MeshFilterComponent(MeshFilterComponent&& component):
    Component(ClassNameCleaner::Current().GetClassName<MeshFilterComponent>(), component.id), meshId(component.meshId) {
        Load();
        this->SetGameObject(component.GetGameObject());
    }

    MeshFilterComponent::~MeshFilterComponent() {

    }

    void MeshFilterComponent::Load() {        
        if (ObjectResourceManager::HasKey(meshId))
            mesh = ObjectResourceManager::Get<Mesh>(meshId);
    }

    void MeshFilterComponent::SetMesh(Ref<Mesh> _mesh) {
        mesh = _mesh;
        meshId = _mesh->GetID();
    }

    MeshFilterComponent& MeshFilterComponent::operator=(const MeshFilterComponent& t) {
        this->meshId = t.meshId;
        this->mesh = t.mesh;
        this->SetGameObject(const_cast<MeshFilterComponent&>(t).GetGameObject());
        return *this;
    }

    void MeshFilterComponent::SetMeshId(UUID _id) {
        this->meshId = _id;
        Load();
    }

    Ref<Component> MeshFilterComponent::Copy(entt::entity& handle, entt::registry& registry) {
        Ref<MeshFilterComponent> component = CopyComponent<MeshFilterComponent>(handle, registry);
        return component;
    }

    VWOLF_CREATE_CONVERT_GENERIC_CLASS_ENCODER_WITH_NAME(MeshFilterComponent, "MeshFilterComponent")
}
