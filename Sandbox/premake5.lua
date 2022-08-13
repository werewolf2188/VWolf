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
   --removefiles { "vendor/imgui/**" }
  
   defines
   {
      "_CRT_SECURE_NO_WARNINGS"
   }

   includedirs
   {
      "src",
	  "%{wks.location}/VWolf/src",
	--    "%{wks.location}/VWolf/vendor",
	--    "%{IncludeDir.GLFW}",
	--    "%{IncludeDir.Glad}",
	--    "%{IncludeDir.ImGui}",
   }

   links 
   {
      "VWolf"   
    --   "GLFW",
	--   "Glad",
	--   "ImGui",
	--   "opengl32.lib"
   }

   filter "system:windows"
      systemversion "latest"

	--   defines
	--   {
   --       "GLFW_INCLUDE_NONE"
	--   }

   filter "configurations:Debug"
      defines { "DEBUG" }
      symbols "On"

   filter "configurations:Release"
      defines { "NDEBUG" }
      optimize "On"