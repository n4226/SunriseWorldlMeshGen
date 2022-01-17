
workspace "SunriseWorldMeshGen"
	architecture "x64"
		

	configurations {
		"Debug",
		"Release",
		"Dist"
	}


	flags
	{
		"MultiProcessorCompile"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

-- Include directories relative to root folder (solution directory)
IncludeDir = {}


group "Dependencies"

group ""


-- below is the project


project "SunriseWorldMeshGen"
	
	mainProjDir = "SunriseWorldMeshGen"
	sunriseLocation = "%{wks.location}/extern/Sunrise"

	kind "ConsoleApp"
	language "C++"

	targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")


	pchheader "mgpch.h"
	pchsource "src/mgpch.cpp"

	files {
		"src/**.h",
		"src/**.cpp",

		"src/**.vert",
		"src/**.frag",
		"src/**.comp",
		
		"%{wks.location}/Sunrise/src/**.vert",
		"%{wks.location}/Sunrise/src/**.frag",
		"%{wks.location}/Sunrise/src/**.comp"
	}

	includedirs {
		"C:/VulkanSDK/1.2.154.1/Include",

		"%{wks.location}/src",
		"%{wks.location}/extern/shapelib/",
		"%{sunriseLocation}/src", "%{sunriseLocation}/src/Sunrise",
		"%{sunriseLocation}/vendor",
		"%{sunriseLocation}/vendor/spdlog/include",
		
		"%{sunriseLocation}/vendor/glm-master",
		"%{sunriseLocation}/vendor/marl-main/marl-main/include",
		"%{sunriseLocation}/vendor/stb",
		"%{sunriseLocation}/vendor/mango-master/mango-master/include",
		"%{sunriseLocation}/vendor/libigl/include",
		"%{sunriseLocation}/vendor/HTTPRequest/include",
		"%{sunriseLocation}/vendor/httplib/",
		"%{sunriseLocation}/vendor/rapidjson/include",
		"%{sunriseLocation}/vendor/libguarded/src",
		"%{sunriseLocation}/vendor/nlohmann/include",
		"%{sunriseLocation}/vendor/earcut-hpp/include/mapbox",
		"%{sunriseLocation}/vendor/asio-1.18.1/include",
		
		"%{sunriseLocation}/vendor/bin/glfw/windows/glfw-3.3.2.bin.WIN64/glfw-3.3.2.bin.WIN64/lib-vc2019",
		
		"%{sunriseLocation}/vendor/date/include",
		"%{sunriseLocation}/vendor/entt/single_include",
	}	
	
	libdirs {
		"C:/VulkanSDK/1.2.154.1/Lib",
		"%{sunriseLocation}/vendor/marl-main/marl-main/Debug",
		"%{wks.location}/extern/shapelib/build/dll/Debug",
	}

	links {
		"Sunrise",
		"vulkan-1",
		"shp",
		"marl",
	}

	postbuildcommands {
		("{COPY} %{wks.location}/bin/" .. outputdir .. "/Sunrise/Sunrise.dll %{wks.location}/bin/" .. outputdir .. "/%{prj.name}/"),
		("{COPY} %{wks.location}/bin/" .. outputdir .. "/Sunrise/Sunrise.pdb %{wks.location}/bin/" .. outputdir .. "/%{prj.name}/"),
		--("call ../Sunrise/src/Sunrise/Sunrise/graphcis/shaders/compileShaders.bat"),
		--("XCOPY /S /Y ../Sunrise/src/Sunrise/Sunrise/graphcis/shaders/ ../bin/" .. outputdir .. "/%{prj.name}/shaders"),
		
		-- shapelib dll copy
		("{COPY} %{wks.location}/extern/shapelib/build/dll/Debug/shp.dll %{wks.location}/bin/" .. outputdir .. "/%{prj.name}/"),

	}

	filter "system:windows"
		
		cppdialect "C++17"
		staticruntime "off"
		systemversion "latest"

		defines {
			"SR_PLATFORM_WINDOWS"
		}
		debugdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}/")


	filter "configurations:Debug"
		defines "SR_DEBUG"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		defines "SR_RELEASE"
		runtime "Release"
		optimize "on"

	filter "configurations:Dist"
		defines "SR_DIST"
		runtime "Release"
		optimize "on"

	-- GLSL Shader Compile Pipeline

	filter 'files:**.vert or files:**.frag or files:**.comp'
	   -- A message to display while this build step is running (optional)
	   buildmessage 'Compiling %{file.relpath} TO SPIR-V'

	   -- One or more commands to run (required)
	   buildcommands {
		  ("C:/VulkanSDK/1.2.154.1/Bin/glslangValidator.exe -V %{file.relpath} -o ../bin/" .. outputdir .. "/%{prj.name}/shaders/%{file.name}.spv")
	   }

	   -- One or more outputs resulting from the build (required)
	   buildoutputs {("../bin/" .. outputdir .. "/%{prj.name}/shaders/%{file.name}.spv")}

	   -- One or more additional dependencies for this build command (optional)
	   --buildinputs { 'path/to/file1.ext', 'path/to/file2.ext' }


include "extern/Sunrise"