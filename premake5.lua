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

boostversion = iif(os.is("windows"), "boost_1_79_0", "boost_1_80_0")
programfiles = iif(os.is("windows"), "C:/Program Files", "/Users/vn50tzy/Projects/") -- os.getenv("ProgramFiles") -- Returning C:\Program Files (x86)
boost = iif(os.is("windows"), "%{programfiles}/boost/%{boostversion}", "%{programfiles}/%{boostversion}") 

-- Include directories relative to root folder (solution directory)
IncludeDir = {}
IncludeDir["GLFW"] = "%{wks.location}/VWolf/vendor/GLFW/include"
IncludeDir["Glad"] = "%{wks.location}/VWolf/vendor/Glad/include"
IncludeDir["boost"] = boost
IncludeDir["ImGui"] = "%{wks.location}/VWolf/vendor/imgui"
IncludeDir["glm"] = "%{wks.location}/VWolf/vendor/glm"

LibDir = {}
LibDir["boost"] = "%{boost}/stage/lib"

group "Dependencies"
   include "VWolf/vendor/GLFW"
   include "VWolf/vendor/Glad"
   include "VWolf/vendor/imgui"
group ""

include "VWolf"
include "Sandbox"