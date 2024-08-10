-- premake5.lua

project "VWolfTest"
   kind "ConsoleApp"
   language "C++"
   cppdialect "C++17"
   staticruntime "on"
   targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
   objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")


   files {
	   "**.cpp"
   }

   includedirs
   {
	   "%{wks.location}/VWolf/src",
	   "%{wks.location}/VWolf/vendor",
      "%{IncludeDir.entt}",
      "%{IncludeDir.glm}",
	   "%{IncludeDir.yaml_cpp}",
      "%{IncludeDir.boost}",
   }

   libdirs 
   {
       "%{LibDir.boost}"
   }

   links 
   {
      "VWolf"
   }

   filter "system:macosx"
   systemversion "latest"

   includedirs
   {
      "%{IncludeDir.metal_cpp}"
   }

    sysincludedirs
    {
	   "%{wks.location}/VWolf/src",
	   "%{wks.location}/VWolf/vendor",
      "%{IncludeDir.glm}",
	   "%{IncludeDir.entt}",
	   "%{IncludeDir.yaml_cpp}",
      "%{IncludeDir.metal_cpp}",
      "%{IncludeDir.boost}",
    }

   links 
   { 
      "Cocoa.framework", 
      "CoreVideo.framework", 
      "IOKit.framework", 
      "OpenGL.framework", 
      "MetalKit.framework", 
      "AppKit.framework", 
      "Metal.framework", 
      "QuartzCore.framework", 
      "GameController.framework"
   }

   defines 
   {
      "_LIBCPP_ENABLE_CXX17_REMOVED_UNARY_BINARY_FUNCTION"
   }

   filter "system:windows"
      systemversion "latest"

   filter "configurations:Debug"
      defines { "DEBUG" }
      symbols "On"

   filter "configurations:Release"
      defines { "NDEBUG" }
      optimize "On"