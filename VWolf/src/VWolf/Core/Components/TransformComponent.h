//
//  TransformComponent.hpp
//  VWolf
//
//  Created by Enrique Ricalde on 4/14/23.
//

#pragma once

#include "BaseComponent.h"
#include "VWolf/Core/Math/VMath.h"
#include "VWolf/Core/Utils/GenericSerialization.h"

namespace VWolf {

    class TransformComponent: public Component {
    public:
        TransformComponent();
        TransformComponent(const TransformComponent& transform);
        TransformComponent(TransformComponent&& transform);
        ~TransformComponent();
    public:
        Vector3& GetPosition() { return position; }
        void SetPosition(Vector3 position) { this->position = position; }
        const Quaternion& GetQuaternion() const { return orientation; }
        void SetQuaternion(Quaternion quaternion);
        Vector3& GetLocalScale() { return localScale; }
        void SetLocalScale(Vector3 localScale) { this->localScale = localScale; }

        Matrix4x4& GetWorldMatrix() { return matrix; }
        
        Vector3 Up() const;
        Vector3 Right() const;
        Vector3 Forward() const;

        void Apply();
    public:
        virtual Ref<Component> Copy(entt::entity& handle, entt::registry& registry) override;
    public:
        TransformComponent& operator=(TransformComponent t);
    private:
        void RestoreQuaternionFromEulerAngles();
        Vector3 position;
        Vector3 eulerAngles;
        Vector3 localScale;
        Quaternion orientation;
        Matrix4x4 matrix;
        
        BOOST_DESCRIBE_CLASS(TransformComponent, (Component), (), (id), (position, eulerAngles, localScale))
        
        VWOLF_SERIALIZATION_FRIENDS(TransformComponent)
    };
}

namespace YAML {
    template<>
    struct convert<VWolf::TransformComponent> {
        static bool decode(const Node& node, VWolf::TransformComponent& rhs)
        {
            bool decoded = VWolf::DeserializeFromBoostDescribe(node, rhs);
            rhs.RestoreQuaternionFromEulerAngles();
            return decoded;
        }
    };
}
