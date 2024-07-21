//
//  CameraComponent.h
//  VWolf
//
//  Created by Enrique Ricalde on 6/28/24.
//

#pragma once

enum class CameraConstantKeys {
    ViewportWidth, ViewportHeight, FOV, AspectRatio, NearClip, FarClip, Zoom, IsOrthographic
};

static std::map<CameraConstantKeys, const char*> cameraKeys = {
    { CameraConstantKeys::ViewportWidth, "ViewportWidth" },
    { CameraConstantKeys::ViewportHeight, "ViewportHeight" },
    { CameraConstantKeys::FOV, "FOV" },
    { CameraConstantKeys::AspectRatio, "AspectRatio" },
    { CameraConstantKeys::NearClip, "NearClip" },
    { CameraConstantKeys::FarClip, "FarClip" },
    { CameraConstantKeys::Zoom, "Zoom" },
    { CameraConstantKeys::IsOrthographic, "IsOrthographic" },
};

namespace YAML {
    template<>
    struct convert<VWolf::CameraComponent>
    {
        static bool decode(const Node& node, VWolf::CameraComponent& rhs)
        {
            if (!node.IsMap())
                return false;
            
            rhs.SetViewportWidth(node[cameraKeys[CameraConstantKeys::ViewportWidth]].as<float>());
            rhs.SetViewportHeight(node[cameraKeys[CameraConstantKeys::ViewportHeight]].as<float>());
            rhs.SetFOV(node[cameraKeys[CameraConstantKeys::FOV]].as<float>());
            rhs.SetAspectRatio(node[cameraKeys[CameraConstantKeys::AspectRatio]].as<float>());
            rhs.SetNearClip(node[cameraKeys[CameraConstantKeys::NearClip]].as<float>());
            rhs.SetFarClip(node[cameraKeys[CameraConstantKeys::FarClip]].as<float>());
            rhs.SetZoom(node[cameraKeys[CameraConstantKeys::Zoom]].as<float>());
            rhs.SetOrthographic(node[cameraKeys[CameraConstantKeys::IsOrthographic]].as<bool>());
            return true;
        }
    };
}

namespace VWolf {
    YAML::Emitter& operator<<(YAML::Emitter& out, VWolf::CameraComponent& v)
    {
        out << YAML::BeginMap;
        out << YAML::Key << cameraKeys[CameraConstantKeys::ViewportWidth] << YAML::Value << v.GetViewportWidth();
        out << YAML::Key << cameraKeys[CameraConstantKeys::ViewportHeight] << YAML::Value << v.GetViewportHeight();
        out << YAML::Key << cameraKeys[CameraConstantKeys::FOV] << YAML::Value << v.GetFOV();
        out << YAML::Key << cameraKeys[CameraConstantKeys::AspectRatio] << YAML::Value << v.GetAspectRatio();
        out << YAML::Key << cameraKeys[CameraConstantKeys::NearClip] << YAML::Value << v.GetNearClip();
        out << YAML::Key << cameraKeys[CameraConstantKeys::FarClip] << YAML::Value << v.GetFarClip();
        out << YAML::Key << cameraKeys[CameraConstantKeys::Zoom] << YAML::Value << v.GetZoom();
        out << YAML::Key << cameraKeys[CameraConstantKeys::IsOrthographic] << YAML::Value << v.IsOrthographic();
        out << YAML::EndMap;
        return out;
    }
}
