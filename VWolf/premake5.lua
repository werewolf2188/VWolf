-- premake5.lua

project "VWolf"
   kind "StaticLib"
   language "C++"
   cppdialect "C++17"
   staticruntime "on"
   targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
   objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

   pchheader "vwpch.h"
   pchsource "src/vwpch.cpp"


   files { 
      "src/**.h", 
	  "src/**.cpp",
      "vendor/glm/glm/**.hpp",
	  "vendor/glm/glm/**.inl",
	  "vendor/stb_image/**.h",
	  "vendor/stb_image/**.cpp",
	  "vendor/entt/src/**.hpp"

   }
   removefiles { "vendor/imgui/**", "vendor/imguizmo/**" }
  
   defines
   {
      "_CRT_SECURE_NO_WARNINGS"
   }

   includedirs
   {
      "src",
      "%{IncludeDir.GLFW}",
	  "%{IncludeDir.Glad}",
      "%{IncludeDir.boost}",
	  "%{IncludeDir.ImGui}",
      "%{IncludeDir.glm}",
      "%{IncludeDir.stb_image}",
      "%{IncludeDir.entt}",
      "%{IncludeDir.yaml_cpp}",
   }

   libdirs 
   {
       "%{LibDir.boost}"
   }

	links 
	{ 
      "GLFW",
	  "Glad",
	  "ImGui",
	  "ImGuizmo",
	  "yaml-cpp",
	  "opengl32.lib"
	}
   
   filter { 'files:src/VWolf/Platform/ImGUI/backends/**.cpp' }
      flags { 'NoPCH' }

   filter "system:windows"
      systemversion "latest"

   includedirs
   {
      "vendor/d3dx12",
   }

   defines
   {
      "GLFW_INCLUDE_NONE"
   }

   filter "system:macosx"
   systemversion "latest"

   pchheader "src/vwpch.h"
   pchsource "src/vwpch.cpp"

    sysincludedirs
    {
	"src",
        "%{IncludeDir.boost}",
        "%{IncludeDir.GLFW}",
	"%{IncludeDir.Glad}",
        "%{IncludeDir.glm}",
        "%{IncludeDir.ImGui}",
	"%{IncludeDir.ImGuizmo}",
	"%{IncludeDir.yaml_cpp}"
    }

   defines
   {
      "GLFW_INCLUDE_NONE"
   }


   filter "configurations:Debug"
      defines { "DEBUG" }
      symbols "On"

   filter "configurations:Release"
      defines { "NDEBUG" }
      optimize "On"