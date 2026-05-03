project "zlib-lib"
	targetdir ("../bin/" .. outputdir .. "/%{prj.name}")
	objdir ("../bin-int/" .. outputdir .. "/%{prj.name}")
	language    "C"
	kind        "StaticLib"
	defines     { "N_FSEEKO" }
	warnings    "off"
	includedirs { "zlib" }

	files
	{
    	"zlib/adler32.c",
    	"zlib/compress.c",
    	"zlib/crc32.c",
    	"zlib/deflate.c",
    	"zlib/gzclose.c",
    	"zlib/gzlib.c",
    	"zlib/gzread.c",
    	"zlib/gzwrite.c",
    	"zlib/inflate.c",
    	"zlib/infback.c",
    	"zlib/inftrees.c",
    	"zlib/inffast.c",
    	"zlib/trees.c",
    	"zlib/uncompr.c",
    	"zlib/zutil.c",
    	"zlib/*.h"
	}

	filter "system:windows"
		defines { "_WINDOWS" }

	filter "system:not windows"
		defines { 'HAVE_UNISTD_H' }
		removefiles
    	{
    	    "zlib/contrib/minizip/iowin32.c",
    	    "zlib/contrib/minizip/iowin32.h"
    	}

project "BitPouncePack"
    kind "StaticLib"
    language "C++"
	cppdialect "C++23"

	targetdir ("../bin/" .. outputdir .. "/%{prj.name}")
	objdir ("../bin-int/" .. outputdir .. "/%{prj.name}")

    files { "src/**cpp", "src/**cpp" }

    includedirs { "src", "zlib" }
	links { "zlib-lib" }

    filter "system:windows"
        staticruntime "On"
		systemversion "latest"
    filter "system:linux"
		cppdialect "C++23"
		systemversion "latest"

    filter "configurations:Debug"
		defines "BPPACK_DEBUG"
		symbols "On"

	filter "configurations:Release"
		defines "BPPACK_RELEASE"
		optimize "On"

	filter "configurations:Dist"
		defines "BPPACK_DIST"
		optimize "On"