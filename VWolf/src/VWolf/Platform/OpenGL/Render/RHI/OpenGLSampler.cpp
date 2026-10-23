//
//  OpenGLSampler.cpp
//  VWolf
//
//  Created by Enrique Moises on 10/21/26.
//

#include "vwpch.h"
#include "OpenGLSampler.h"

namespace VWolf::RHI {

    OpenGLSampler::OpenGLSampler(Weak<OpenGLRenderContext> context, const SamplerSettings& settings):
    Sampler(settings), context(context) {
        Initialize();
    }

    OpenGLSampler::~OpenGLSampler() {
        GLThrowIfFailed(glDeleteSamplers(1, &m_samplerID));
    }

    static const GLuint GetFilter(const SamplerMinMag& filter) {
        switch (filter) {
            case SamplerMinMag::Nearest: return GL_NEAREST;
            case SamplerMinMag::Linear: return GL_LINEAR;
        }
    }

    static const GLuint GetFilter(const SamplerMip& filter) {
        switch (filter) {
            case SamplerMip::NotMipmapped: return -1;
            case SamplerMip::Nearest: return GL_NEAREST_MIPMAP_NEAREST;
            case SamplerMip::Linear: return GL_LINEAR_MIPMAP_LINEAR;
        }
    }

    static const GLuint GetAddressMode(const SamplerMode& mode) {
        switch (mode) {
            case SamplerMode::ClampToEdge: return GL_CLAMP_TO_EDGE;
            case SamplerMode::ClampToZero: return GL_CLAMP_TO_EDGE;
            case SamplerMode::ClampToBorderColor: return GL_CLAMP_TO_BORDER;
            case SamplerMode::Repeat: return GL_REPEAT;
            case SamplerMode::RepeatMirror: return GL_MIRRORED_REPEAT;
        }
    }

    void OpenGLSampler::Bind(uint32_t base) {
        GLThrowIfFailed(glBindSampler(base, m_samplerID));
    }

    void OpenGLSampler::Unbind(uint32_t base) {
        GLThrowIfFailed(glBindSampler(base, 0));
    }

    ResourceView OpenGLSampler::GetResourceView() {
        return ResourceView(shared_from_this(), {});
    }

    void OpenGLSampler::Initialize() {
        GLThrowIfFailed(glGenSamplers(1, &m_samplerID));
        GLThrowIfFailed(glSamplerParameteri(m_samplerID, GL_TEXTURE_MIN_FILTER, GetFilter(settings.GetFilter().GetMin())));
        GLThrowIfFailed(glSamplerParameteri(m_samplerID, GL_TEXTURE_MAG_FILTER, GetFilter(settings.GetFilter().GetMag())));

        GLThrowIfFailed(glSamplerParameteri(m_samplerID, GL_TEXTURE_MIN_LOD, settings.GetLod().GetMin()));
        GLThrowIfFailed(glSamplerParameteri(m_samplerID, GL_TEXTURE_MAX_LOD, settings.GetLod().GetMax()));
        GLThrowIfFailed(glSamplerParameteri(m_samplerID, GL_TEXTURE_LOD_BIAS, settings.GetLod().GetBias()));
        
        GLThrowIfFailed(glSamplerParameteri(m_samplerID, GL_TEXTURE_WRAP_S, GetAddressMode(settings.GetAddress().GetS())));
        GLThrowIfFailed(glSamplerParameteri(m_samplerID, GL_TEXTURE_WRAP_T, GetAddressMode(settings.GetAddress().GetT())));
        GLThrowIfFailed(glSamplerParameteri(m_samplerID, GL_TEXTURE_WRAP_R, GetAddressMode(settings.GetAddress().GetR())));
    }
}
