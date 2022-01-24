
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

mainProjDir = "SunriseWorldMeshGen"

project "SunriseWorldMeshGen"
	

	-- not sure if these two lines should be commented out
	--mainProjDir = "SunriseWorldMeshGen"
	sunriseLocation = "%{wks.location}/extern/Sunrise"

	kind "ConsoleApp"
	language "C++"
	cppdialect "C++20"
	staticruntime "off"


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
		
		"%{sunriseLocation}/src/**.vert",
		"%{sunriseLocation}/src/**.frag",
		"%{sunriseLocation}/src/**.comp"
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
		"%{sunriseLocation}/vendor/asio-1.18.1/include",
		"%{sunriseLocation}/vendor/eigen"
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
		--("{COPY} %{wks.location}/bin/" .. outputdir .. "/Sunrise/Sunrise.dll %{wks.location}/bin/" .. outputdir .. "/%{prj.name}/"),
		--("{COPY} %{wks.location}/bin/" .. outputdir .. "/Sunrise/Sunrise.pdb %{wks.location}/bin/" .. outputdir .. "/%{prj.name}/"),
		
		-- shapelib dll copy
		("{COPY} %{wks.location}/extern/shapelib/build/dll/Debug/shp.dll %{wks.location}/bin/" .. outputdir .. "/%{prj.name}/"),

	}

	filter "system:windows"
		systemversion "latest"

		defines {
			"SR_PLATFORM_WINDOWS"
		}
		debugdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}/")

	filter "system:macosx"

		defines {
			"SR_PLATFORM_MACOS"
		}
		xcodebuildsettings { ["ALWAYS_SEARCH_USER_PATHS"] = "YES" }

		pchheader "src/mgpch.h" 

		includedirs {
			"/usr/local/include",
			"%{sunriseLocation}/vendor/bin/glfw/macos/glfw-3.3.6.bin.MACOS/include",
		}

		libdirs {
			"/usr/local/lib",
			"%{sunriseLocation}/vendor/bin/glfw/macos/glfw-3.3.6.bin.MACOS/lib-universal",
		}


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

	filter {'files:**.vert or files:**.frag or files:**.comp', 'system:windows' }
		glslCompilerLoc = "C:/VulkanSDK/1.2.154.1/Bin/glslangValidator.exe"
	   -- A message to display while this build step is running (optional)
	   buildmessage 'Compiling %{file.relpath} TO SPIR-V - windows'
		
	   buildinputs {("%{sunriseLocation}src/Sunrise/graphics/shaders/headers/**.h")}

	   -- One or more commands to run (required)
	   buildcommands {
		  -- (glslCompilerLoc .. (" -V %{file.relpath} -o %{wks.location}/bin/" .. outputdir .. "/%{prj.name}/shaders/%{file.name}.spv -g"))
		  ("C:/VulkanSDK/1.2.154.1/Bin/glslangValidator.exe -V %{file.relpath} -o %{wks.location}/bin/" .. outputdir .. "/%{prj.name}/shaders/%{file.name}.spv -g")
	   }

	   -- One or more outputs resulting from the build (required)
	   buildoutputs {("../bin/" .. outputdir .. "/%{prj.name}/shaders/%{file.name}.spv")}

	   -- One or more additional dependencies for this build command (optional)
	   --buildinputs { 'path/to/file1.ext', 'path/to/file2.ext' }


include "extern/Sunrise"