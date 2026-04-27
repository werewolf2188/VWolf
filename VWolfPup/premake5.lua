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

   excludes { "src/ThirdParty/efsw/**.hpp", "src/ThirdParty/efsw/**.h", "src/ThirdParty/efsw/**.cpp" }

   includedirs
   {
	  "%{wks.location}/VWolf/src",
	  "%{wks.location}/VWolf/vendor",
     "%{wks.location}/VWolfPup/src/ThirdParty",
      "%{IncludeDir.entt}",
      "%{IncludeDir.glm}",
	   "%{IncludeDir.yaml_cpp}",
      "%{IncludeDir.efsw}",
      "%{IncludeDir.boost}"
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
      "VWolf",
      "efsw"
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

   includedirs
   {
      "%{IncludeDir.metal_cpp}",
      "%{IncludeDir.metal_shaderconverter}"
   }

    sysincludedirs
    {
	"%{wks.location}/VWolf/src",
	 "%{wks.location}/VWolf/vendor",
    "%{wks.location}/VWolfPup/src/ThirdParty",
         "%{IncludeDir.glm}",
	 "%{IncludeDir.entt}",
	"%{IncludeDir.yaml_cpp}",
   "%{IncludeDir.metal_cpp}",
   "%{IncludeDir.metal_shaderconverter}",
   "%{IncludeDir.efsw}",
   "%{IncludeDir.boost}"
    }

   links { "Cocoa.framework", "CoreVideo.framework", "IOKit.framework", "OpenGL.framework", "MetalKit.framework", "AppKit.framework", "Metal.framework", "QuartzCore.framework", "GameController.framework", "dxcompiler", "metalirconverter" }

   libdirs 
   {
      "%{wks.location}/bin/" .. outputdir .. "/%{prj.name}"
   }

   linkoptions { "-rpath @executable_path/../VWolfPup" }

   prebuildcommands {	  
	  "{COPYFILE} %{wks.location}/VWolf/vendor/DirectXShaderCompiler/bin/osx/libdxcompiler.dylib %{wks.location}/bin/" .. outputdir .. "/%{prj.name}/libdxcompiler.dylib",
	  "{COPYFILE} %{wks.location}/VWolf/vendor/DirectXShaderCompiler/bin/osx/libdxil.dylib %{wks.location}/bin/" .. outputdir .. "/%{prj.name}/libdxil.dylib",
     "{COPYFILE} %{wks.location}/VWolf/vendor/metal-shaderconverter/lib/libmetalirconverter.dylib %{wks.location}/bin/" .. outputdir .. "/%{prj.name}/libmetalirconverter.dylib"
   }

   filter "system:windows"
      systemversion "latest"
	  buildoptions { "/Zc:preprocessor" }
	  
   prebuildcommands {	  
	  "{COPYFILE} %{wks.location}/VWolf/vendor/DirectXShaderCompiler/bin/x64/dxcompiler.dll %{wks.location}/bin/" .. outputdir .. "/%{prj.name}/dxcompiler.dll",
	  "{COPYFILE} %{wks.location}/VWolf/vendor/DirectXShaderCompiler/bin/x64/dxil.dll %{wks.location}/bin/" .. outputdir .. "/%{prj.name}/dxil.dll"
   }

   filter "configurations:Debug"
      defines { "DEBUG" }
      symbols "On"

   filter "configurations:Release"
      defines { "NDEBUG" }
      optimize "On"