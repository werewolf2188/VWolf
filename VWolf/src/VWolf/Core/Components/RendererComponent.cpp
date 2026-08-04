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
    RendererComponent::RendererComponent(): Component(ClassNameCleaner::Current().GetClassName<RendererComponent>()) {}
    RendererComponent::RendererComponent(std::string name): Component(name) {}
    RendererComponent::RendererComponent(std::string name, UUID id): Component(name, id) {}
    RendererComponent::~RendererComponent() {}

    MeshRendererComponent::MeshRendererComponent():
    RendererComponent(ClassNameCleaner::Current().GetClassName<MeshRendererComponent>()), material(MaterialLibrary::Default()), materialId(MaterialLibrary::Default()->GetID()) {}

    MeshRendererComponent::MeshRendererComponent(Material& material):
    RendererComponent(ClassNameCleaner::Current().GetClassName<MeshRendererComponent>()), material(&material), materialId(material.GetID()) {}

    MeshRendererComponent::MeshRendererComponent(const MeshRendererComponent& component):
    RendererComponent(ClassNameCleaner::Current().GetClassName<MeshRendererComponent>(), component.id), materialId(component.materialId) {
        this->material = ObjectResourceManager::Get<Material>(component.materialId);
        this->SetGameObject(const_cast<MeshRendererComponent&>(component).GetGameObject());
    }
    MeshRendererComponent::MeshRendererComponent(MeshRendererComponent&& component):
    RendererComponent(ClassNameCleaner::Current().GetClassName<MeshRendererComponent>(), component.id), materialId(component.materialId) {
        this->material = ObjectResourceManager::Get<Material>(component.materialId);
        this->SetGameObject(component.GetGameObject());
    }
    MeshRendererComponent::~MeshRendererComponent() {}

    Ref<Component> MeshRendererComponent::Copy(entt::entity& handle, entt::registry& registry) {
        Ref<MeshRendererComponent> component = CopyComponent<MeshRendererComponent>(handle, registry);
        return component;
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

    VWOLF_CREATE_CONVERT_GENERIC_CLASS_ENCODER_WITH_NAME(MeshRendererComponent, "MeshRendererComponent")

    // TODO: Test. Will remove later
    ShapeRendererComponent::ShapeRendererComponent():
    RendererComponent(ClassNameCleaner::Current().GetClassName<ShapeRendererComponent>()),
    material(MaterialLibrary::Default()), materialId(MaterialLibrary::Default()->GetID()),
    mesh(CreateRef<Mesh>(ShapeHelper::Create(VWOLF_GET_SHAPE_NAME(ShapeHelper::Triangle)))), meshId(VWOLF_GET_SHAPE_ID(ShapeHelper::Triangle)) {}

    ShapeRendererComponent::ShapeRendererComponent(Ref<Mesh> mesh, Material& material):
    RendererComponent(ClassNameCleaner::Current().GetClassName<ShapeRendererComponent>()), mesh(mesh), meshId(mesh->GetID()),
    material(&material), materialId(material.GetID())  {}

    ShapeRendererComponent::ShapeRendererComponent(const ShapeRendererComponent& component):
    RendererComponent(ClassNameCleaner::Current().GetClassName<ShapeRendererComponent>(), component.id),
    meshId(component.meshId), materialId(component.materialId) {
        this->material = ObjectResourceManager::Get<Material>(component.materialId);
        this->mesh = CreateRef<Mesh>(ShapeHelper::Create(meshId));
        this->SetGameObject(const_cast<ShapeRendererComponent&>(component).GetGameObject());
    }
    ShapeRendererComponent::ShapeRendererComponent(ShapeRendererComponent&& component):
    RendererComponent(ClassNameCleaner::Current().GetClassName<ShapeRendererComponent>(), component.id),
    meshId(component.meshId), materialId(component.materialId) {
        this->material = ObjectResourceManager::Get<Material>(component.materialId);
        this->mesh = CreateRef<Mesh>(ShapeHelper::Create(meshId));
        this->SetGameObject(component.GetGameObject());
    }
    ShapeRendererComponent::~ShapeRendererComponent() {}

    Ref<Component> ShapeRendererComponent::Copy(entt::entity& handle, entt::registry& registry) {
        Ref<ShapeRendererComponent> component = CopyComponent<ShapeRendererComponent>(handle, registry);
        return component;
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

    VWOLF_CREATE_CONVERT_GENERIC_CLASS_ENCODER_WITH_NAME(ShapeRendererComponent, "ShapeRendererComponent")
}
