//
//  AudioSourceComponent.hpp
//  VWolf
//
//  Created by Enrique Ricalde on 7/31/24.
//

#pragma once

#include "BaseComponent.h"
#include "VWolf/Core/Math/VMath.h"

struct ma_engine;
struct ma_sound;

namespace VWolf {
    class TransformComponent;

    class AudioSourceComponent: public Component {
    public:
        AudioSourceComponent();
        AudioSourceComponent(AudioSourceComponent& audioSource);
        AudioSourceComponent(AudioSourceComponent&& audioSource);
        ~AudioSourceComponent();
    public:
        virtual void OnInspector() override;
        virtual Component* Copy(entt::entity& handle, entt::registry& registry) override;
    public:
        void SetAudioFile(std::filesystem::path path) { this->audioFilePath = path; }
        std::filesystem::path GetAudioFile() { return this->audioFilePath; }
        bool GetLoop() const { return mLoop; }
        bool& GetLoop() { return mLoop; }
        void SetLoop(bool loop) { mLoop = loop; }
#ifdef VWOLF_CORE
    public:
        void Prepare(TransformComponent& listener, TransformComponent& sourceTransform);
        void Update(TransformComponent& listener, TransformComponent& sourceTransform);
        void End();
#endif
    private:
        void Initialize();
    private:
        std::filesystem::path audioFilePath;
        std::string fileTemp;
        bool mLoop;

        ma_engine* engine = nullptr;
        ma_sound* sound = nullptr;

        Vector3Float listenerPosition, listenerDirection;
    VWOLF_COMPONENT_INSPECTOR_DEFINE(AudioSourceComponent);
    };
}
