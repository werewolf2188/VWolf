//
//  BoxColliderComponent.h
//  VWolf
//
//  Created by Enrique Ricalde on 7/20/24.
//

#pragma once

#include "BaseComponent.h"
#include "VWolf/Core/Render/RenderStructs.h"

namespace reactphysics3d{
    class BoxShape;
    class Collider;
}

namespace VWolf {
    class TransformComponent;

    class BoxColliderComponent: public Component {
    public:
        BoxColliderComponent();
        BoxColliderComponent(BoxColliderComponent& sphereCollider);
        BoxColliderComponent(BoxColliderComponent&& sphereCollider);
        ~BoxColliderComponent();
    public:
        virtual void OnInspector() override;
        virtual Component* Copy(entt::entity& handle, entt::registry& registry) override;
    public:
        void CreateBoxCollider(MeshData& data, TransformComponent& component);
        void Update(TransformComponent& component);
        void Destroy();
    private:
        reactphysics3d::BoxShape* boxShape;
        reactphysics3d::Collider* collider;
        Vector3 scale;

        VWOLF_COMPONENT_INSPECTOR_DEFINE(BoxColliderComponent);
    };
}
