//
//  MeshFilterComponent.hpp
//  VWolf
//
//  Created by Enrique Ricalde on 6/23/23.
//

#pragma once

#include "BaseComponent.h"
#include "VWolf/Core/Render/Mesh.h"

#include "VWolf/Core/Utils/GenericSerialization.h"

namespace VWolf {
    class MeshFilterComponent: public Component {
    public:
        MeshFilterComponent();
        MeshFilterComponent(UUID meshId);
        MeshFilterComponent(const MeshFilterComponent& component);
        MeshFilterComponent(MeshFilterComponent&& component);
        ~MeshFilterComponent();
    public:
        virtual Ref<Component> Copy(entt::entity& handle, entt::registry& registry) override;
    public:
        Ref<Mesh> GetMesh() { return mesh; }
    public:
        void SetMesh(Ref<Mesh> mesh);
        void SetMeshId(UUID _id);
    public:
        MeshFilterComponent& operator=(const MeshFilterComponent& t);
    private:
        void Load();
    private:
        Ref<Mesh> mesh;
        UUID meshId;
        BOOST_DESCRIBE_CLASS(MeshFilterComponent, (Component), (), (id), (meshId))
        
        VWOLF_SERIALIZATION_FRIENDS(MeshFilterComponent)
    };
}

namespace YAML {
    VWOLF_CREATE_CONVERT_GENERIC_CLASS_DECODER(VWolf::MeshFilterComponent)
}
