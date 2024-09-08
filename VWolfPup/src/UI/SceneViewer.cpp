//
//  SceneViewer.cpp
//  VWolfPup
//
//  Created by Enrique Ricalde on 4/16/23.
//

#include "SceneViewer.h"
#include <imgui/imgui.h>
#include <ImGuizmo/ImGuizmo.h>

namespace VWolfPup {
    SceneViewer::SceneViewer(VWolf::Ref<VWolf::Camera> camera, VWolf::DriverType driverType, uint32_t width, uint32_t height):
    camera(camera), renderTexture(VWolf::Texture::LoadRenderTexture(width, height)), driverType(driverType), View("Scene") { }
    SceneViewer::~SceneViewer() {
        
    }

    void SceneViewer::SetInContainer() {
        GetContainer()->GetRoot()->Install(this);
    }

    void SceneViewer::OnGui() {
        bool noMove = VWolf::Input::IsKeyPressed(VWolf::KeyCode::LeftShift) ||
        VWolf::Input::IsKeyPressed(VWolf::KeyCode::LeftAlt) ||
        VWolf::Input::IsKeyPressed(VWolf::KeyCode::LeftControl);
        ImGui::Begin(title.c_str(), nullptr, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse |
                     (noMove ? ImGuiWindowFlags_NoMove: 0));
        isHovering = ImGui::IsWindowHovered();
        ImVec2 windowSize = ImGui::GetWindowSize();
        renderTexture->Resize((uint32_t)windowSize.x, (uint32_t)windowSize.y);
        if (driverType == VWolf::DriverType::OpenGL)
            ImGui::Image(renderTexture->GetHandler(), windowSize, ImVec2(0, 1), ImVec2(1, 0));
        else
            ImGui::Image(renderTexture->GetHandler(), windowSize);
        ImGui::SetCursorPos(ImVec2(10, 30));
        
        if (!isPlaying) {
            if (ImGui::Button("Translate")) {
                operation = (uint32_t)ImGuizmo::OPERATION::TRANSLATE;
            }
            if (ImGui::Button("Rotate")) {
                operation = (uint32_t)ImGuizmo::OPERATION::ROTATE;
            }
            if (ImGui::Button("Scale")) {
                operation = (uint32_t)ImGuizmo::OPERATION::SCALE;
            }
            if (selectedObject && operation) {
                ImGuizmo::SetOrthographic(false);
                ImGuizmo::SetDrawlist();
                auto viewportMinRegion = ImGui::GetWindowContentRegionMin();
                auto viewportMaxRegion = ImGui::GetWindowContentRegionMax();
                auto viewportOffset = ImGui::GetWindowPos();
                ImVec2 m_ViewportBounds[2];
                m_ViewportBounds[0] = { viewportMinRegion.x + viewportOffset.x, viewportMinRegion.y + viewportOffset.y };
                m_ViewportBounds[1] = { viewportMaxRegion.x + viewportOffset.x, viewportMaxRegion.y + viewportOffset.y };
                
                ImGuizmo::SetRect(m_ViewportBounds[0].x, m_ViewportBounds[0].y, m_ViewportBounds[1].x - m_ViewportBounds[0].x, m_ViewportBounds[1].y - m_ViewportBounds[0].y);
                
                VWolf::Matrix4x4 projection = camera->GetProjection();
                VWolf::Matrix4x4 view = camera->GetViewMatrix();
                VWolf::TransformComponent& tComponent = selectedObject->GetTransform();
                tComponent.Apply();
                VWolf::Matrix4x4& transform = tComponent.GetWorldMatrix();
                if (((ImGuizmo::OPERATION)operation) == ImGuizmo::OPERATION::ROTATE) {
                    VWolf::Vector3 rota = VWolf::Vector3::Zero;
                    transform = VWolf::Matrix4x4::TRS(selectedObject->GetTransform().GetPosition(),
                                                      VWolf::Quaternion::Euler(rota.GetX(), rota.GetY(), rota.GetZ()),
                                                      selectedObject->GetTransform().GetLocalScale());
                }
                
                ImGuizmo::Manipulate(view.Unsafe_AddressOf(), projection.Unsafe_AddressOf(),
                                     (ImGuizmo::OPERATION)operation, ImGuizmo::WORLD, transform.Unsafe_AddressOf(),
                                     nullptr, nullptr);
                if (ImGuizmo::IsUsing())            {
                    VWolf::Vector3 translation, rotation, scale;

                    ImGuizmo::DecomposeMatrixToComponents(transform.Unsafe_AddressOf(), &translation.GetX(), &rotation.GetX(), &scale.GetX());

                    if (((ImGuizmo::OPERATION)operation) == ImGuizmo::OPERATION::TRANSLATE)
                        selectedObject->GetTransform().SetPosition(translation);
                    if (((ImGuizmo::OPERATION)operation) == ImGuizmo::OPERATION::ROTATE)
                        selectedObject->GetTransform().SetEulerAngles(selectedObject->GetTransform().GetEulerAngles() + rotation);
                    if (((ImGuizmo::OPERATION)operation) == ImGuizmo::OPERATION::SCALE)
                        selectedObject->GetTransform().SetLocalScale(scale);
                }
            }
        }
        ImGui::End();
    }
}

