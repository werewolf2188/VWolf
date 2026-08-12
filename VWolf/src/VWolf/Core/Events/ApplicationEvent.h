#pragma once
#include "Event.h"

#include "VWolf/Core/Render/Texture.h"

#include <sstream>

namespace VWolf {
	class WindowCloseEvent : public Event
	{
	public:
		WindowCloseEvent() = default;

		EVENT_CLASS_TYPE(WindowClose)
		// EVENT_CLASS_CATEGORY(EventCategoryApplication)
	};

	class WindowResizeEvent : public Event
	{
	public:
		WindowResizeEvent(unsigned int width, unsigned int height)
			: m_Width(width), m_Height(height) {}

		unsigned int GetWidth() const { return m_Width; }
		unsigned int GetHeight() const { return m_Height; }

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "WindowResizeEvent: " << m_Width << ", " << m_Height;
			return ss.str();
		}

		EVENT_CLASS_TYPE(WindowResize)
		// EVENT_CLASS_CATEGORY(EventCategoryApplication)
	private:
		unsigned int m_Width, m_Height;
	};

    class WindowDragDropEvent: public Event {
    public:
        WindowDragDropEvent(int pathCount, const char** paths): paths(paths, paths + pathCount) {
            
        }
        
        const std::vector<std::string>& GetPaths() { return paths; }
        
        std::string ToString() const override
        {
            std::stringstream ss;
            ss << "WindowDragDropEvent: path count - " << paths.size() << ", paths - [";
            for (int index = 0; index < paths.size(); index++) {
                if (index == paths.size() - 1)
                    ss << paths[index] << "]";
                else
                    ss << paths[index] << ", ";
            }
            return ss.str();
        }
        EVENT_CLASS_TYPE(WindowDrapDrop)
    private:
        std::vector<std::string> paths;
    };

	class AppTickEvent : public Event
	{
	public:
		AppTickEvent() = default;

		EVENT_CLASS_TYPE(AppTick)
		// EVENT_CLASS_CATEGORY(EventCategoryApplication)
	};

	class AppUpdateEvent : public Event
	{
	public:
		AppUpdateEvent() = default;

		EVENT_CLASS_TYPE(AppUpdate)
		// EVENT_CLASS_CATEGORY(EventCategoryApplication)
	};

	class AppRenderEvent : public Event
	{
	public:
        AppRenderEvent(long long frame, Ref<RenderTexture> render_texture):
        m_frame(frame), m_render_texture(render_texture) {};
    public:
        const long long& GetRenderFrame() const { return m_frame; }
        const Ref<RenderTexture> GetRenderTexture() const { return m_render_texture; }
		EVENT_CLASS_TYPE(AppRender)
    private:
        long long m_frame;
        Ref<RenderTexture> m_render_texture;
		// EVENT_CLASS_CATEGORY(EventCategoryApplication)
	};
}
