//
//  RendererComponent.hpp
//  VWolf
//
//  Created by Enrique Ricalde on 4/14/23.
//

#pragma once

#include "BaseComponent.h"
#include "VWolf/Core/Render/RenderStructs.h"
#include "VWolf/Core/Render/Material.h"

namespace VWolf {

    class RendererComponent: public Component {
    public:
        RendererComponent();
        RendererComponent(std::string name);
        ~RendererComponent();
    VWOLF_COMPONENT_INSPECTOR_DEFINE(RendererComponent);
    };

    class MeshRendererComponent: public RendererComponent {
    public:
        MeshRendererComponent();
        MeshRendererComponent(Material& material);
        MeshRendererComponent(MeshRendererComponent& component);
        MeshRendererComponent(MeshRendererComponent&& component);
        ~MeshRendererComponent();
    public:
        virtual void OnInspector() override;
    public:
        Material& GetMaterial() { return *material; }
        void SetMaterial(Material* material) { this->material = material; }
    public:
        MeshRendererComponent& operator=(MeshRendererComponent t);
    private:
        Material* material;
    VWOLF_COMPONENT_INSPECTOR_DEFINE(MeshRendererComponent);
    };

    // TODO: Test. Will remove later
    class ShapeRendererComponent: public RendererComponent {
    public:
        ShapeRendererComponent();
        ShapeRendererComponent(MeshData data, Material& material);
        ShapeRendererComponent(ShapeRendererComponent& component);
        ShapeRendererComponent(ShapeRendererComponent&& component);
        ~ShapeRendererComponent();
    public:
        virtual void OnInspector() override;
    public:
        MeshData& GetData() { return data; }
        void SetData(MeshData data) { this->data = data; }
        Material& GetMaterial() { return *material; }
        void SetMaterial(Material* material) { this->material = material; }
    public:
        ShapeRendererComponent& operator=(ShapeRendererComponent t);
    private:
        MeshData data;
        Material* material;
    VWOLF_COMPONENT_INSPECTOR_DEFINE(ShapeRendererComponent);
    };
}
