workspace "Merlin"
	architecture "x64"
	startproject "merlin.example"

	configurations
	{
		"Debug",
		"Release"
	}
	
	flags
	{
		"MultiProcessorCompile"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"
solutiondir = path.getabsolute("./")

IncludeDir = {}
IncludeDir["self"] = "include"
IncludeDir["glfw"] = "vendor/glfw/include"
IncludeDir["glad"] = "vendor/glad/include"
IncludeDir["imgui"] = "vendor/imgui"
IncludeDir["glm"] = "vendor/glm"
IncludeDir["stb_image"] = "vendor/stb_image"
IncludeDir["tinyfiledialogs"] = "vendor/tinyfiledialogs"
IncludeDir["tinyxml2"] = "vendor/tinyxml2"
IncludeDir["ImGuiTextEditor"] = "vendor/ImGuiTextEditor"
IncludeDir["assimp"] = "vendor/assimp/include"

LinksDir = (solutiondir .. "lib")

-- Projects
group "Examples"
	include "examples/merlin.example"
	include "examples/merlin.example2D"
	include "examples/merlin.compute"
	include "examples/merlin.graphics"