
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

include "Sunrise"

-- below is the project


project "SunriseWorldMeshGen"

	kind "ConsoleApp"
	language "C++"

	targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")



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

		"%{wks.location}/Sunrise/src",
		"%{wks.location}/Sunrise/vendor",
		"%{wks.location}/Sunrise/vendor/spdlog/include",

		"%{wks.location}/Sunrise/vendor/glm-master",
		"%{wks.location}/Sunrise/vendor/marl-main/marl-main/include",
		"%{wks.location}/Sunrise/vendor/stb",
		"%{wks.location}/Sunrise/vendor/mango-master/mango-master/include",
		"%{wks.location}/Sunrise/vendor/libigl/include",
		"%{wks.location}/Sunrise/vendor/HTTPRequest/include",
		"%{wks.location}/Sunrise/vendor/httplib/include",
		"%{wks.location}/Sunrise/vendor/rapidjson/include",
		"%{wks.location}/Sunrise/vendor/libguarded/src",
		"%{wks.location}/Sunrise/vendor/nlohmann/include",

		"%{wks.location}/Sunrise/vendor/bin/glfw/windows/glfw-3.3.2.bin.WIN64/glfw-3.3.2.bin.WIN64/lib-vc2019",
		
		"%{wks.location}/Sunrise/vendor/date/include",
		"%{wks.location}/Sunrise/vendor/entt/single_include",
	}	

	libdirs {
		"C:/VulkanSDK/1.2.154.1/Lib",
	}

	links {
		"Sunrise",
		"vulkan-1",
	}

	postbuildcommands {
		("{COPY} ../bin/" .. outputdir .. "/Sunrise/Sunrise.dLL ../bin/" .. outputdir .. "/%{prj.name}/"),
		("{COPY} ../bin/" .. outputdir .. "/Sunrise/Sunrise.pdb ../bin/" .. outputdir .. "/%{prj.name}/"),
		--("call ../Sunrise/src/Sunrise/Sunrise/graphcis/shaders/compileShaders.bat"),
		--("XCOPY /S /Y ../Sunrise/src/Sunrise/Sunrise/graphcis/shaders/ ../bin/" .. outputdir .. "/%{prj.name}/shaders"),
	}

	filter "system:windows"
		
		cppdialect "C++17"
		staticruntime "off"
		systemversion "latest"

		defines {
			"SR_PLATFORM_WINDOWS"
		}
		debugdir ("../bin/" .. outputdir .. "/%{prj.name}/")


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

