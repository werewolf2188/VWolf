//
//  GraphicsContext.cpp
//  VWolf
//
//  Created by Enrique Moises on 7/25/26.
//

#include "vwpch.h"
#include "GraphicsContext.h"
#include "VWolf/Core/Components/GameObject.h"
#include "VWolf/Core/Time.h"

namespace VWolf {

    namespace {
        Bounds GetWorldBounds(const Bounds& localBounds, const Matrix4x4& worldMatrix) {
            Vector3 min = localBounds.GetMin();
            Vector3 max = localBounds.GetMax();

            Vector3 corner1(min.GetX(), min.GetY(), min.GetZ());
            Vector3 corner2(max.GetX(), min.GetY(), min.GetZ());
            Vector3 corner3(min.GetX(), max.GetY(), min.GetZ());
            Vector3 corner4(min.GetX(), min.GetY(), max.GetZ());
            Vector3 corner5(max.GetX(), max.GetY(), min.GetZ());
            Vector3 corner6(min.GetX(), max.GetY(), max.GetZ());
            Vector3 corner7(max.GetX(), min.GetY(), max.GetZ());
            Vector3 corner8(max.GetX(), max.GetY(), max.GetZ());
            
            Vector3 worldCorner1 = worldMatrix * corner1;
            Vector3 worldCorner2 = worldMatrix * corner2;
            Vector3 worldCorner3 = worldMatrix * corner3;
            Vector3 worldCorner4 = worldMatrix * corner4;
            Vector3 worldCorner5 = worldMatrix * corner5;
            Vector3 worldCorner6 = worldMatrix * corner6;
            Vector3 worldCorner7 = worldMatrix * corner7;
            Vector3 worldCorner8 = worldMatrix * corner8;
            
            Bounds worldBounds(worldCorner1, worldCorner1);
            worldBounds.Encapsulate(worldCorner2);
            worldBounds.Encapsulate(worldCorner3);
            worldBounds.Encapsulate(worldCorner4);
            worldBounds.Encapsulate(worldCorner5);
            worldBounds.Encapsulate(worldCorner6);
            worldBounds.Encapsulate(worldCorner7);
            worldBounds.Encapsulate(worldCorner8);
            
            return worldBounds;
        }
    
        std::vector<Vector4> GetFrustumCornersWorldSpace(const Matrix4x4& proj, const Matrix4x4& view) {
            Matrix4x4 invCam = (proj * view).GetInverse();
            std::vector<Vector4> frustumCorners;
            for (unsigned int x = 0; x < 2; ++x) {
                for (unsigned int y = 0; y < 2; ++y) {
                    for (unsigned int z = 0; z < 2; ++z) {
                        Vector4 pt = invCam * Vector4(
                            static_cast<float>(x) * 2.0f - 1.0f,
                            static_cast<float>(y) * 2.0f - 1.0f,
                            static_cast<float>(z) * 2.0f - 1.0f,
                            1.0f
                        );
                        frustumCorners.push_back(pt / pt.GetW());
                    }
                }
            }
            return frustumCorners;
        }
    
        void ClampCornersToShadowDistance(std::vector<Vector4>& corners, const Vector3& cameraPos, float shadowDistance) {
            for (auto& corner : corners) {
                Vector3 point(corner.GetX(), corner.GetY(), corner.GetZ());
                Vector3 offset = point - cameraPos;
                float distance = offset.Magnitude();
                if (distance > shadowDistance && distance > 0.0f) {
                    Vector3 clamped = cameraPos + offset * (shadowDistance / distance);
                    corner = Vector4(clamped.GetX(), clamped.GetY(), clamped.GetZ(), 1.0f);
                }
            }
        }
    
        Matrix4x4 MakeLightProjection(const Bounds& bounds, uint32_t shadowMapSize) {
            Vector3 minVector3 = bounds.GetMin();
            Vector3 maxVector3 = bounds.GetMax();
            
            Vector2 minVector2 = minVector3;
            Vector2 maxVector2 = maxVector3;
            
            const float shadowMapSizeF = static_cast<float>(shadowMapSize);
            
            Vector2 unitsPerTexel = (maxVector2 - minVector2) / shadowMapSizeF;
            
            if (unitsPerTexel.GetX() > 0.0f) {
                minVector2.SetX(std::floor(minVector2.GetX() / unitsPerTexel.GetX()) * unitsPerTexel.GetX());
                maxVector2.SetX(minVector2.GetX() + shadowMapSizeF * unitsPerTexel.GetX());
            }
            
            if (unitsPerTexel.GetY() > 0.0f) {
                minVector2.SetY(std::floor(minVector2.GetY() / unitsPerTexel.GetY()) * unitsPerTexel.GetY());
                maxVector2.SetY(minVector2.GetY() + shadowMapSizeF * unitsPerTexel.GetY());
            }
            
            const float zPadding = 1.0f;
            float zNear = -maxVector3.GetZ() - zPadding;
            float zFar = -minVector3.GetZ() + zPadding;
            if (zNear < 0.1f) {
                zNear = 0.1f;
            }
            if (zFar <= zNear) {
                zFar = zNear + 1.0f;
            }

            return Matrix4x4::Ortho(minVector2.GetX(), maxVector3.GetX(), minVector2.GetY(), maxVector3.GetX(), zNear, zFar);
        }
    
        void CreateLightSpaceAABB(Bounds& bounds, const std::vector<Vector4>& corners, const Matrix4x4& lightView, float shadowDistance) {
            float min = std::numeric_limits<float>::max();
            float max = std::numeric_limits<float>::lowest();
            
            Vector3 minVector = { min, min, min };
            Vector3 maxVector = { max, max, max };
            
            for (const auto& v : corners) {
                Vector4 trf = lightView * v;
                
                minVector.SetX(std::min(minVector.GetX(), trf.GetX()));
                maxVector.SetX(std::max(maxVector.GetX(), trf.GetX()));
                
                minVector.SetY(std::min(minVector.GetY(), trf.GetY()));
                maxVector.SetY(std::max(maxVector.GetY(), trf.GetY()));
                
                minVector.SetZ(std::min(minVector.GetZ(), trf.GetZ()));
                maxVector.SetZ(std::max(maxVector.GetZ(), trf.GetZ()));
            }
            
            Vector3 center = (minVector + maxVector) * 0.5f;
            
            if (maxVector.GetX() - minVector.GetX() > shadowDistance * 2.0f) {
                minVector.SetX(center.GetX() - shadowDistance);
                maxVector.SetX(center.GetX() + shadowDistance);
            }
            
            if (maxVector.GetY() - minVector.GetY() > shadowDistance * 2.0f) {
                minVector.SetY(center.GetY() - shadowDistance);
                maxVector.SetY(center.GetY() + shadowDistance);
            }
            
            if (maxVector.GetZ() - minVector.GetZ() > shadowDistance * 2.0f) {
                minVector.SetZ(center.GetZ() - shadowDistance);
                maxVector.SetZ(center.GetZ() + shadowDistance);
            }
            
            bounds.SetMinMax(minVector, maxVector);
        }
    
        Matrix4x4 GetLightSpaceMatrix(
                                      const Light& gpuLight,
                                      const Bounds& sceneBounds,
                                      Matrix4x4 cameraProj,
                                      Matrix4x4 cameraView,
                                      float shadowDistance = 50,
                                      uint32_t shadowMapSize = 1024
                                      ) {
            
            Vector3 lightDir = const_cast<Light&>(gpuLight).direction;

            std::vector<Vector4> volumeCorners;
            Vector3 target;
            if (sceneBounds.HasVolume()) {
                // Fit to the scene so the ortho does not slide when the camera rotates.
                volumeCorners = sceneBounds.GetBoundsCornersWorldSpace();
                target = sceneBounds.GetCenter();
            } else {
                Matrix4x4 invView = cameraView.GetInverse();
                Vector3 cameraPos = invView.GetPosition();
                volumeCorners = GetFrustumCornersWorldSpace(cameraProj, cameraView);
                ClampCornersToShadowDistance(volumeCorners, cameraPos, shadowDistance);
                target = Vector3(0.0f, 0.0f, 0.0f);
                for (const auto& corner : volumeCorners) {
                    target += Vector3(corner.GetX(), corner.GetY(), corner.GetZ());
                }
                target = target / static_cast<float>(volumeCorners.size());
            }
            
            float radius = 1.0f;
            for (const auto& corner : volumeCorners) {
                Vector3 point(corner.GetX(), corner.GetY(), corner.GetZ());
                radius = std::max(radius, (point - target).Magnitude());
            }
            
            Vector3 up = Vector3::Up;
            if (Mathf::Abs(Vector3::Dot(lightDir, up)) > 0.999f) {
                up = Vector3::Right;
            }
            
            Vector3 lightPosition = target - lightDir * (radius + 1.0f);
            Matrix4x4 lightView = Matrix4x4::LookAt(lightPosition, target, up);
            
            Bounds lightSpaceAABB(Vector3::Zero, Vector3::Zero);
            
            CreateLightSpaceAABB(lightSpaceAABB, volumeCorners, lightView, shadowDistance);
            
            return MakeLightProjection(lightSpaceAABB, shadowMapSize) * lightView;
        }
    }

    std::vector<Light> GraphicsContext::lights;
    std::vector<Matrix4x4> GraphicsContext::lightSpaces;
    std::vector<LightComponent> GraphicsContext::lights_ex;

    Color GraphicsContext::backgroundColor;

    GraphicsCommandList GraphicsContext::list;

    void GraphicsContext::Reset() {
        lights.clear();
        lights_ex.clear();
        list.ClearQueue();
    }

    void GraphicsContext::AddLight(LightComponent& lightCo) {
        lights_ex.push_back(lightCo);
        
        TransformComponent& transform = lightCo.GetGameObject()->GetTransform();
        
        Light& light = lightCo.GetLight();
        light.position = Vector4(transform.GetPosition().GetX(), transform.GetPosition().GetY(), transform.GetPosition().GetZ(), 1.0);
        light.direction = (Vector4)transform.GetEulerAngles();
        lights.push_back(light);
        lightSpaces.push_back(light.GetLightSpaceMatrix());
    }

    std::vector<LightComponent> GraphicsContext::GetLightsFor(const Matrix4x4& transform) {
        
        if (lights_ex.size() <= Light::LightsMax)
            return lights_ex;
        
        const Vector3 position = transform.GetPosition();
        
        std::vector<LightComponent> lights_temp = lights_ex;
        
        std::sort(lights_temp.begin(), lights_temp.end(), [position](LightComponent& lightA, LightComponent& lightB) {
            Vector3 positionA = lightA.GetGameObject()->GetTransform().GetPosition();
            Vector3 positionB = lightB.GetGameObject()->GetTransform().GetPosition();
            
            float sqrtMagA = (positionA - position).SqrMagnitude();
            float sqrtMagB = (positionB - position).SqrMagnitude();
            return sqrtMagA < sqrtMagB;
        });

        std::vector<LightComponent> results;
        results.resize(Light::LightsMax);
        
        auto last_it = std::next(lights_temp.begin(), std::min((size_t)Light::LightsMax, lights_temp.size()));
        std::transform(lights_temp.begin(), last_it, results.begin(), [](LightComponent& lightCo) {
            return lightCo;
        });
        
        return results;
    }

    Ref<CommandsContext> GraphicsContext::ProcessContext() {
        Ref<CommandsContext> context = CreateRef<CommandsContext>();
        Bounds sceneBounds;
        
        for(Ref<GraphicsCommand> command: GraphicsContext::list.GetCommands()) {
            Ref<DrawMeshCommand> drawMeshCommand = DynamicAs<DrawMeshCommand>(command);
            
            Ref<CameraComponent> camera = drawMeshCommand->GetCamera();
            Ref<Material> material = drawMeshCommand->GetMaterial();
            Ref<Shader> shader = Shader::GetShader(drawMeshCommand->GetMaterial()->GetShaderName());
            Ref<Mesh> mesh = drawMeshCommand->GetMesh();
            const Matrix4x4& worldMatrix = drawMeshCommand->GetTransform();
            uint32_t submeshIndex = drawMeshCommand->GetSubmeshIndex();
            bool castShadows = drawMeshCommand->GetCastShadows();
            
            GraphicsCommandEntity entity { mesh, worldMatrix, submeshIndex, castShadows };
            
            context->cameras.insert(camera);
            context->shaders.insert(shader);
            context->materials.insert(material);
            context->materialData[material->GetID()] = UniqueVoidPtr(material->GetDataPointer());
            
            std::set<UUID> materialsIds;
            
            if (context->shaderMaterials.count(shader->GetID()) > 0)
                materialsIds = context->shaderMaterials[shader->GetID()];
            
            materialsIds.insert(material->GetID());
            
            context->shaderMaterials[shader->GetID()] = materialsIds;
            
            if (drawMeshCommand->GetMesh() != nullptr && drawMeshCommand->GetMesh()->GetVertices().size() > 1)  {
                Bounds worldBounds = GetWorldBounds(drawMeshCommand->GetMesh()->GetBounds(), drawMeshCommand->GetTransform());

                if (castShadows) {
                    if (sceneBounds.GetMin() == Vector3::Zero &&
                        sceneBounds.GetMax() == Vector3::Zero) {
                        sceneBounds = worldBounds;
                    } else {
                        sceneBounds.Encapsulate(worldBounds);
                    }
                }
                
                context->entities.push_back(entity);
                context->materialObjects.insert({ material->GetID(), entity.id });
                
                std::vector<LightComponent> lights_for_mesh = GetLightsFor(worldMatrix);
                std::vector<UUID> lightIds(Light::LightsMax);
                
                std::transform(lights_for_mesh.begin(), lights_for_mesh.end(), lightIds.begin(), [](LightComponent& component){
                    return component.GetID();
                });
                
                for (const UUID& lightId: lightIds)
                    context->objectLights.insert({ entity.id, lightId });
            }
        }
        
        for(Ref<CameraComponent> cameraComponent: context->cameras) {
            TransformComponent& cameraTransform = cameraComponent->GetGameObject()->GetTransform();
            Ref<Camera> camera = cameraComponent->GetCamera();
            
            Vector3 focalPoint = camera->GetFocalPoint();
            float zoom = camera->GetZoom();
            Vector3 position;
            Quaternion orientation = cameraTransform.GetOrientation();
            
            if ((cameraComponent->GetGameObject()->GetFlags() & HideFlags::Editor) == HideFlags::Editor) {
                position = focalPoint - cameraTransform.GetPosition() * zoom;
            } else {
                position = cameraTransform.GetPosition();
            }
            
            Matrix4x4 viewMatrix = camera->CalculateView(position, orientation);
            Matrix4x4 skyboxViewMatrix = camera->CalculateView(Vector3::Zero, orientation);
            
            context->gpuCamera[cameraComponent->GetID()] = {
                viewMatrix,
                viewMatrix.GetInverse(),
                camera->GetProjection(),
                camera->GetProjection().GetInverse(),
                camera->GetViewProjection(viewMatrix),
                camera->GetViewProjection(viewMatrix).GetInverse(),
                cameraTransform.GetPosition(),
                0,
                camera->GetDisplaySize(),
                { 1 / camera->GetDisplaySize().GetX(), 1 / camera->GetDisplaySize().GetY() },
                camera->GetNearZ(),
                camera->GetFarZ(),
                Time::GetTotalTime(),
                Time::GetDeltaTime()
            };
            
            context->gpuSkyboxCamera[cameraComponent->GetID()] = {
                skyboxViewMatrix,
                skyboxViewMatrix.GetInverse(),
                camera->GetProjection(),
                camera->GetProjection().GetInverse(),
                camera->GetViewProjection(skyboxViewMatrix),
                camera->GetViewProjection(skyboxViewMatrix).GetInverse(),
                cameraTransform.GetPosition(),
                0,
                camera->GetDisplaySize(),
                { 1 / camera->GetDisplaySize().GetX(), 1 / camera->GetDisplaySize().GetY() },
                camera->GetNearZ(),
                camera->GetFarZ(),
                Time::GetTotalTime(),
                Time::GetDeltaTime()
            };
            
            for(LightComponent& lightComponent: GraphicsContext::lights_ex) {
                context->lights.insert(UnownedRef<LightComponent>(&lightComponent));
                
                std::tuple<UUID, UUID> key = std::make_tuple(cameraComponent->GetID(), lightComponent.GetID());
                
                Light& light = lightComponent.GetLight_Ex();
                
                Matrix4x4 lightSpaceMatrix = GetLightSpaceMatrix(light, sceneBounds, camera->GetProjection(), viewMatrix);
                context->lightSpaceMatrix.insert({ key, lightSpaceMatrix });
            }
        }
        
        context->shadowShader = Shader::GetShader("Shadow");
        context->sceneBounds = sceneBounds;
        return context;
    }

    bool CommandsContext::EntityContainsLight(const GraphicsCommandEntity& entity, UUID lightId) {
        auto range = objectLights.equal_range(entity.id);
        
        for (auto it = range.first; it != range.second; ++it) {
            if (it->second == lightId)
                return true;
        }
        return false;
    }
}
