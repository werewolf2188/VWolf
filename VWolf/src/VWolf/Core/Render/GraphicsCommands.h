//
//  GraphicsCommands.h
//  VWolf
//
//  Created by Enrique Moises on 7/25/26.
//

#pragma once

#include <mutex>

#include "VWolf/Core/Base.h"
#include "VWolf/Core/Math/VMath.h"
#include "Mesh.h"
#include "VWolf/Core/Components/CameraComponent.h"
#include "Material.h"

namespace VWolf {

    // These classes will hold the commands for the renderer to process once the scene is done, and in the future,
    // The UI commands as well.
    class GraphicsCommand {
    public:
        virtual void Test() = 0;
    private:
    };

    class MeshGraphicsCommand: public GraphicsCommand {
    public:
        MeshGraphicsCommand(Ref<Mesh> mesh, Vector3 position, Quaternion rotation);
        MeshGraphicsCommand(Ref<Mesh> mesh, Matrix4x4 transform);
    public:
        virtual void Test() override {}
        Ref<Mesh> GetMesh() { return mesh; }
        Matrix4x4& GetTransform() { return transform; }
    protected:
        Ref<Mesh> mesh;
        Matrix4x4 transform;
    };

    class DrawMeshCommand: public MeshGraphicsCommand {
    public:
        DrawMeshCommand(Ref<Mesh> mesh, Vector3 position, Quaternion rotation, Ref<Material> material, int layer, int submeshIndex, Ref<CameraComponent> CameraComponent = nullptr, bool castShadows = true, bool receiveShadows = true);
        DrawMeshCommand(Ref<Mesh> mesh, Matrix4x4 transform, Ref<Material> material, int layer, int submeshIndex, Ref<CameraComponent> camera = nullptr, bool castShadows = true, bool receiveShadows = true);
    public:
        Ref<Material> GetMaterial() { return material; }
        Ref<CameraComponent> GetCamera() { return camera; }
        int GetLayer() { return layer; }
        int GetSubmeshIndex() { return submeshIndex; }
        bool GetCastShadows() { return castShadows; }
        bool GetReceiveShadows() { return receiveShadows; }
    private:
        Ref<Material> material;
        Ref<CameraComponent> camera;
        int layer;
        int submeshIndex;
        bool castShadows;
        bool receiveShadows;
        // bool useLightProbes
    };

    class GraphicsCommandList {
    public:
        GraphicsCommandList() = default;
    public:
        void Add(Ref<GraphicsCommand> command) {
            std::scoped_lock lock(mtx);
            commands.push_back(command);
        }
        
        void ClearQueue() {
            std::scoped_lock lock(mtx);
            commands.clear();
        }
        
        const std::vector<Ref<GraphicsCommand>>& GetCommands() const { return commands; }
    private:
        std::vector<Ref<GraphicsCommand>> commands;
        std::mutex mtx;
    };
}
