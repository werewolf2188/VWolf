-- premake5.lua
workspace "VWolf"
   architecture "x64"
   startproject "VWolf"

   configurations
   {
      "Debug",
      "Release",
   }
	
   flags
   {
      "MultiProcessorCompile"
   }

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

-- Include directories relative to root folder (solution directory)
-- IncludeDir = {}
-- IncludeDir["GLFW"] = "vendor/GLFW/include"
-- IncludeDir["Glad"] = "vendor/Glad/include"
-- IncludeDir["ImGui"] = "vendor/imgui"

-- group "Dependencies"
-- 	include "vendor/GLFW"
-- 	include "vendor/Glad"
-- 	include "vendor/imgui"
-- group ""

project "VWolf"
   kind "ConsoleApp"
   language "C++"
   cppdialect "C++17"
   staticruntime "on"
   targetdir "bin/%{cfg.buildcfg}"

   files { 
	  "main.cpp"--,
	--   "src/**.h", 
	--   "src/**.cpp"
   }
   --removefiles { "vendor/imgui/**" }
  
   defines
   {
      "_CRT_SECURE_NO_WARNINGS"
   }

   -- includedirs
   -- {
	--    "%{IncludeDir.GLFW}",
	--    "%{IncludeDir.Glad}",
	--    "%{IncludeDir.ImGui}",
   -- }

   -- links 
   -- { 
   --    "GLFW",
	--   "Glad",
	--   "ImGui",
	--   "opengl32.lib"
   -- }

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