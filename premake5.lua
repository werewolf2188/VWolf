-- premake5.lua
workspace "VWolf"
   architecture "x64"
   startproject "Sandbox"

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
IncludeDir = {}
IncludeDir["GLFW"] = "%{wks.location}/VWolf/vendor/GLFW/include"
IncludeDir["Glad"] = "%{wks.location}/VWolf/vendor/Glad/include"
-- IncludeDir["ImGui"] = "vendor/imgui"

group "Dependencies"
   include "VWolf/vendor/GLFW"
   include "VWolf/vendor/Glad"
-- 	include "vendor/imgui"
group ""

include "VWolf"
include "Sandbox"