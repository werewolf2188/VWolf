-- premake5.lua

project "VWolfPup"
   kind "ConsoleApp"
   language "C++"
   cppdialect "C++17"
   staticruntime "on"
   targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
   objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")


   files { 
      "src/**.h", 
      "src/**.hpp", 
	   "src/**.cpp"
   }

   includedirs
   {
	  "%{wks.location}/VWolf/src",
	  "%{wks.location}/VWolf/vendor",
     "%{wks.location}/VWolfPup/src/ThirdParty",
      "%{IncludeDir.entt}",
      "%{IncludeDir.glm}",
	"%{IncludeDir.yaml_cpp}",	
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

   prebuildcommands
   {
	"{RMDIR} %{wks.location}/bin/" .. outputdir .. "/%{prj.name}/shaders",
	"{COPYDIR} src/shaders %{wks.location}/bin/" .. outputdir .. "/%{prj.name}/shaders",
	"{RMDIR} %{wks.location}/bin/" .. outputdir .. "/%{prj.name}/assets",
	"{COPYDIR} src/assets %{wks.location}/bin/" .. outputdir .. "/%{prj.name}/assets"
   }

   filter "system:macosx"
   systemversion "latest"

    sysincludedirs
    {
	"%{wks.location}/VWolf/src",
	 "%{wks.location}/VWolf/vendor",
    "%{wks.location}/VWolfPup/src/ThirdParty",
         "%{IncludeDir.glm}",
	 "%{IncludeDir.entt}",
	"%{IncludeDir.yaml_cpp}",
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