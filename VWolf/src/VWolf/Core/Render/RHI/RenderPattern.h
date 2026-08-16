//
//  RenderPattern.h
//  VWolf
//
//  Created by Enrique Moises on 8/17/26.
//

#pragma once

#include "Types.h"
#include "Object.h"

namespace VWolf::RHI  {
    class RenderPassAttachment {
    public:
        enum class Type : uint32_t
        {
            Color,
            Depth,
            Stencil
        };

        enum class LoadAction : uint32_t
        {
            DontCare = 0U,
            Load,
            Clear,
        };

        enum class StoreAction : uint32_t
        {
            DontCare = 0U,
            Store,
            Resolve,
        };
        
        RenderPassAttachment() = default;
        RenderPassAttachment(PixelFormat format,
                             LoadAction loadAction = LoadAction::Clear,
                             StoreAction storeAction = StoreAction::Store):
        format(format), loadAction(loadAction), storeAction(storeAction) {}
        
        virtual Type GetType() = 0;
        
        const PixelFormat& GetFormat() const { return format; }
        const LoadAction& GetLoadAction() const { return loadAction; }
        const StoreAction& GetStoreAction() const { return storeAction; }
    protected:
        PixelFormat format;
        LoadAction loadAction;
        StoreAction storeAction;
    private:
    };

    class RenderPassColorAttachment: public RenderPassAttachment {
    public:
        RenderPassColorAttachment() = default;
        RenderPassColorAttachment(PixelFormat format,
                                  LoadAction loadAction = LoadAction::Clear,
                                  StoreAction storeAction = StoreAction::Store):
        RenderPassAttachment(format, loadAction, storeAction) {}
        
        virtual Type GetType() override { return Type::Color; };
    protected:
    private:
    };

    class RenderPassDepthAttachment: public RenderPassAttachment {
    public:
        RenderPassDepthAttachment() = default;
        RenderPassDepthAttachment(PixelFormat format,
                                  LoadAction loadAction = LoadAction::Clear,
                                  StoreAction storeAction = StoreAction::Store):
        RenderPassAttachment(format, loadAction, storeAction) {}
        
        virtual Type GetType() override { return Type::Depth; };
    protected:
    private:
    };

    class RenderPassStencilAttachment: public RenderPassAttachment {
    public:
        RenderPassStencilAttachment() = default;
        RenderPassStencilAttachment(PixelFormat format,
                                    LoadAction loadAction = LoadAction::Clear,
                                    StoreAction storeAction = StoreAction::Store):
        RenderPassAttachment(format, loadAction, storeAction) {}
        
        virtual Type GetType() override { return Type::Stencil; };
    protected:
    private:
    };

    class RenderPatternSettings {
    public:
        RenderPatternSettings() = default;
        RenderPatternSettings(const std::vector<RenderPassColorAttachment> colorAttachments,
                              const std::optional<RenderPassDepthAttachment> depthAttachment,
                              const std::optional<RenderPassStencilAttachment> stencilAttachment):
        colorAttachments(colorAttachments), depthAttachment(depthAttachment), stencilAttachment(stencilAttachment) {}
        
        RenderPatternSettings(const RenderPatternSettings& settings):
        colorAttachments(settings.colorAttachments), depthAttachment(settings.depthAttachment), stencilAttachment(settings.stencilAttachment) {}
        
        const std::vector<RenderPassColorAttachment>& GetColorAttachments() const {
            return colorAttachments;
        }
        const std::optional<RenderPassDepthAttachment>& GetDepthAttachment() const {
            return depthAttachment;
        }
        const std::optional<RenderPassStencilAttachment>& GetStencilAttachment() const {
            return stencilAttachment;
        }
        
        RenderPatternSettings& operator=(const RenderPatternSettings& settings) {
            this->colorAttachments = settings.colorAttachments;
            this->depthAttachment = settings.depthAttachment;
            this->stencilAttachment = settings.stencilAttachment;
            
            return *this;
        }
    protected:
    private:
        std::vector<RenderPassColorAttachment> colorAttachments;
        std::optional<RenderPassDepthAttachment> depthAttachment;
        std::optional<RenderPassStencilAttachment> stencilAttachment;
    };

    class RenderPass;
    class RenderPassSettings;

    class RenderPattern: public Object {
    public:
        RenderPattern(const RenderPatternSettings& settings): settings(settings) {}
        
        virtual Ref<RenderPass> CreateRenderPass(const RenderPassSettings& settings) = 0;
        
        const RenderPatternSettings& GetSettings() const { return settings; }
    protected:
        RenderPatternSettings settings;
    private:
    };
}
