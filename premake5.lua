workspace "Engine"
    architecture "x64"
    configurations
    {
        "Debug",
        "Release",
        "Dist"
    }
    startproject "Engine"

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

FMOD_PATH = "C:/Program Files (x86)/FMOD SoundSystem/FMOD Studio API Windows/api"
PackageDir = "build/vcpkg_installed/x64-windows"
PackageIncludeDir = "%{PackageDir}/include"

--[[IncludeDir =
{
	GLFW = "Vendor/glfw/include",
	GLEW = "Vendor/glew-2.1.0/include",
	GLM = "Vendor/GLM",
	Bullet = "Vendor/bullet3/src",
	FMOD = "C:/Program Files (x86)/FMOD SoundSystem/FMOD Studio API Windows/api",
	ImGui = "Vendor/imgui",
	Assimp = "C:/Program Files/Assimp/include"
}]]

LinkDir =
{
	FMOD_Studio = FMOD_PATH .. "/studio/lib/x64",
	FMOD_Core = FMOD_PATH .. "/core/lib/x64"	
}

LibraryDir = "%{PackageDir}/lib"
DebugLibraryDir = "%{PackageDir}/debug/lib"

BinDir = "%{PackageDir}/bin"
DebugBinDir = "%{PackageDir}/debug/bin"

DLLs =
{
	FMOD = LinkDir.FMOD_Core .. "fmod.dll",
	Assimp = BinDir .. "assimp-vc143-mtd.dll",
	Draco = BinDir .. "draco.dll",
	Glew = BinDir .. "glew32d.dll",
	GLFW = BinDir .. "glfw3.dll",
	Minizip = BinDir .. "minizip.dll",
	Poly2Tri = BinDir .. "poly2tri.dll",
	PugiXML = BinDir .. "pugixml.dll",
	ZLibd =  BinDir .."zlibd1.dll"
}

project "Engine"
    location "."
    kind "ConsoleApp"
    language "C++"

    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("obj/" .. outputdir .. "/%{prj.name}")

    files
	{
		"main.cpp",
		"%{prj.name}/**.h",
		"%{prj.name}/**.hpp",
		"%{prj.name}/**.cpp",

		PackageIncludeDir .. "**.h",
		PackageIncludeDir .. "**.c",
		PackageIncludeDir .. "**.cpp",
		PackageIncludeDir .. "**.hpp",
	}

    includedirs
	{
        ".",
		"Engine",
        "Engine/Core",
        "%{FMOD_PATH}/core/inc",
        "%{FMOD_PATH}/studio/inc",
        "%{PackageDir}/include",
	}

	libdirs
	{
		LinkDir.FMOD_Core,
		LinkDir.FMOD_Studio,
		--"Vendor/glew-2.1.0/lib/Release/x64"
	}

    links
	{
		"draco.lib",
		"glfw3dll.lib",
		"glm.lib",
		"GlU32.lib",
		"glfw3.dll",
		"kubazip.lib",
		"minizip.lib",
		"poly2tri.lib",
		"polyclipping.lib",
		"pugixml.lib",

		"fmod.dll",
		"fmod_vc.lib",
		"fmodL_vc.lib",
		"fmodstudio_vc.lib",
		"fmodstudioL_vc.lib",
		
		"opengl32.lib",
		"dwmapi.lib",
        "gdi32.lib"
	}

    defines
    {
        --"GLEW_STATIC",
        --"GLEW_BUILD"
    }

    filter "system:windows"
		cppdialect "C++17"
		staticruntime "On"
		systemversion "latest"

		defines
		{
			"GLFW_INCLUDE_NONE",
			
			--"SW_PLATFORM_WINDOWS",

			--"WIN32", 
			--"WIN32_LEAN_AND_MEAN",
			"_CRT_NON_CONFORMING_SWPRINTFS",
			"_CRT_SECURE_NO_WARNINGS",
			"_CRT_SECURE_NO_DEPRECATE",
			--"_DEBUG",
			--"_CONSOLE"
		}

		--[[postbuildcommands
		{
			("{COPY} " .. LinkDir.FMOD_Core .. "/fmod.dll" .. " ./bin/" .. outputdir .. "/Engine"),
			("{COPY} " .. DebugBinDir .. "/*.dll" .. " ./bin/" .. outputdir .. "/Engine"),
		}]]

	filter "configurations:Debug"
		defines { "SW_DEBUG", "SW_ENABLE_ASSERTS" }

	filter "configurations:Release"
		defines "SW_RELEASE"

	filter "configurations:Dist"
		defines "SW_DIST"

	filter "configurations:Debug"
		buildoptions "/MDd"
		symbols "On"
		libdirs { DebugLibraryDir }
		links
		{
			"assimp-vc143-mtd.lib",
			"Bullet3Common_Debug.lib",
			"BulletCollision_Debug.lib",
			"BulletDynamics_Debug.lib",
			"BulletInverseDynamics_Debug.lib",
			"BulletSoftBody_Debug.lib",
			"glew32d.lib",
			"glew32d.dll",
			"LinearMath_Debug.lib",
			"zlibd.lib"
		}

	filter "configurations:Release or Dist"
		buildoptions "/MD"
		optimize "On"
		libdirs { LibraryDir }
		links
		{
			"assimp-vc143-mt.lib",
			"Bullet3Common.lib",
			"BulletCollision.lib",
			"BulletDynamics.lib",
			"BulletInverseDynamics.lib",
			"BulletSoftBody.lib",
			"glew32.lib",
			"glew32.dll",
			"LinearMath.lib",
			"zlib.lib"
		}