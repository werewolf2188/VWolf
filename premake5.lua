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

boostversion = "boost_1_79_0"
programfiles = "C:/Program Files" -- os.getenv("ProgramFiles") -- Returning C:\Program Files (x86)

-- Include directories relative to root folder (solution directory)
IncludeDir = {}
IncludeDir["GLFW"] = "%{wks.location}/VWolf/vendor/GLFW/include"
IncludeDir["Glad"] = "%{wks.location}/VWolf/vendor/Glad/include"
IncludeDir["boost"] = "%{programfiles}/boost/%{boostversion}"
IncludeDir["ImGui"] = "%{wks.location}/VWolf/vendor/imgui"
IncludeDir["glm"] = "%{wks.location}/VWolf/vendor/glm"

LibDir = {}
LibDir["boost"] = "%{programfiles}/boost/%{boostversion}/stage/lib"

group "Dependencies"
   include "VWolf/vendor/GLFW"
   include "VWolf/vendor/Glad"
   include "VWolf/vendor/imgui"
group ""

include "VWolf"
include "Sandbox"