assert(os.getenv("SOURCE_SDK"), "SOURCE_SDK environmental variable not set")

local SOURCE_SDK = os.getenv("SOURCE_SDK") .. "/mp/src"


solution "gmsv_FLASH_win32"
	language "C++"
	location ( os.get() .."-".. _ACTION )
	targetdir ( "build/" .. os.get() .. "/" )

	includedirs {
		"include",
		SOURCE_SDK .. "/game",
		SOURCE_SDK .. "/game/shared",
		SOURCE_SDK .. "/game/client",
		SOURCE_SDK .. "/game/client/game_controls",
		SOURCE_SDK .. "/game/server",
		SOURCE_SDK .. "/common",
		SOURCE_SDK .. "/public/tier0",
		SOURCE_SDK .. "/public/tier1",
		SOURCE_SDK .. "/public"
	}

	libdirs {
		"include",
		SOURCE_SDK .. "/lib/public"
	}

	links {
		"tier0",
		"tier1",
		"vstdlib",
		"Shlwapi"
	}
	
	configurations
	{ 
		"Release"
	}
	
	configuration "Release"
		defines { "NDEBUG" }
		optimize "On"

	configuration "vs*"
		linkoptions {
			"/NODEFAULTLIB:libcmt"
		}

	project "gmsv_FLASH_win32"
		defines { "GMMODULE" }
		kind "SharedLib"
		
		files {
			"src/**.cpp",
			"src/**.h"
		}