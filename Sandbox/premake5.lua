-- premake5.lua

project "Sandbox"
   kind "ConsoleApp"
   language "C++"
   cppdialect "C++17"
   staticruntime "on"
   targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
   objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")


   files { 
      "src/**.h", 
	  "src/**.cpp"
   }

   includedirs
   {
	  "%{wks.location}/VWolf/src",
	  "%{wks.location}/VWolf/vendor",
      "%{IncludeDir.glm}",
	--    "%{IncludeDir.GLFW}",
	--    "%{IncludeDir.Glad}",
	--    "%{IncludeDir.ImGui}",
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

    sysincludedirs
    {
	"%{wks.location}/VWolf/src",
	 "%{wks.location}/VWolf/vendor",
         "%{IncludeDir.glm}",
    }

   links { "Cocoa.framework", "CoreVideo.framework", "IOKit.framework", "OpenGL.framework" }

   filter "system:windows"
      systemversion "latest"

   filter "configurations:Debug"
      defines { "DEBUG" }
      symbols "On"

   filter "configurations:Release"
      defines { "NDEBUG" }
      optimize "On"