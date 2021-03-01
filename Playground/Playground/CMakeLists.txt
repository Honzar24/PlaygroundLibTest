set(PROJECT_NAME Playground)

################################################################################
# Source groups
################################################################################
set(Header_Files
    "Macros.h"
)
source_group("Header Files" FILES ${Header_Files})

set(Header_Files__Compression
    "Compression/PNGLoader.h"
)
source_group("Header Files\\Compression" FILES ${Header_Files__Compression})

set(Header_Files__Compression__3rdParty
    "Compression/3rdParty/lodepng.h"
)
source_group("Header Files\\Compression\\3rdParty" FILES ${Header_Files__Compression__3rdParty})

set(Header_Files__FileUtils
    "FileUtils/FileMacros.h"
    "FileUtils/IFile.h"
    "FileUtils/RawFile.h"
)
source_group("Header Files\\FileUtils" FILES ${Header_Files__FileUtils})

set(Header_Files__RasterData
    "RasterData/ColorSpace.h"
    "RasterData/Image2d.h"
    "RasterData/ImageLoader.h"
    "RasterData/ImageUtils.h"
)
source_group("Header Files\\RasterData" FILES ${Header_Files__RasterData})

set(Header_Files__Utils
    "Utils/IDataLoader.h"
    "Utils/Logger.h"
    "Utils/Random.h"
)
source_group("Header Files\\Utils" FILES ${Header_Files__Utils})

set(Source_Files__Compression
    "Compression/PNGLoader.cpp"
)
source_group("Source Files\\Compression" FILES ${Source_Files__Compression})

set(Source_Files__Compression__3rdParty
    "Compression/3rdParty/lodepng.cpp"
)
source_group("Source Files\\Compression\\3rdParty" FILES ${Source_Files__Compression__3rdParty})

set(Source_Files__FileUtils
    "FileUtils/IFile.cpp"
    "FileUtils/RawFile.cpp"
)
source_group("Source Files\\FileUtils" FILES ${Source_Files__FileUtils})

set(Source_Files__RasterData
    "RasterData/ColorSpace.cpp"
    "RasterData/Image2d.cpp"
    "RasterData/ImageLoader.cpp"
    "RasterData/ImageUtils.cpp"
)
source_group("Source Files\\RasterData" FILES ${Source_Files__RasterData})

set(Source_Files__Utils
    "Utils/Logger.cpp"
)
source_group("Source Files\\Utils" FILES ${Source_Files__Utils})

set(ALL_FILES
    ${Header_Files}
    ${Header_Files__Compression}
    ${Header_Files__Compression__3rdParty}
    ${Header_Files__FileUtils}
    ${Header_Files__RasterData}
    ${Header_Files__Utils}
    ${Source_Files__Compression}
    ${Source_Files__Compression__3rdParty}
    ${Source_Files__FileUtils}
    ${Source_Files__RasterData}
    ${Source_Files__Utils}
)

set(Header_dirs
    ${CMAKE_CURRENT_SOURCE_DIR}/Compression
    ${CMAKE_CURRENT_SOURCE_DIR}/Compression/3rdParty
    ${CMAKE_CURRENT_SOURCE_DIR}/FileUtils
    ${CMAKE_CURRENT_SOURCE_DIR}/RasterData
    ${CMAKE_CURRENT_SOURCE_DIR}/Utils
)

################################################################################
# Target
################################################################################
add_library(${PROJECT_NAME} ${ALL_FILES})

target_include_directories(${PROJECT_NAME} PUBLIC ${Header_dirs})

use_props(${PROJECT_NAME} "${CMAKE_CONFIGURATION_TYPES}" "${DEFAULT_CXX_PROPS}")
set(ROOT_NAMESPACE Playground)

set_target_properties(${PROJECT_NAME} PROPERTIES
    VS_GLOBAL_KEYWORD "Win32Proj"
)
################################################################################
# Target name
################################################################################
if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        TARGET_NAME_DEBUG   "${PROJECT_NAME}d"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        TARGET_NAME_DEBUG   "${PROJECT_NAME}d"
    )
endif()
################################################################################
# Output directory
################################################################################
if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}/${CMAKE_VS_PLATFORM_NAME}/$<CONFIG>/"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        OUTPUT_DIRECTORY_DEBUG   "${CMAKE_SOURCE_DIR}/${CMAKE_VS_PLATFORM_NAME}/$<CONFIG>/"
        OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}/${CMAKE_VS_PLATFORM_NAME}/$<CONFIG>/"
    )
endif()
if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        INTERPROCEDURAL_OPTIMIZATION_RELEASE "TRUE"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        INTERPROCEDURAL_OPTIMIZATION_RELEASE "TRUE"
    )
endif()
################################################################################
# MSVC runtime library
################################################################################
get_property(MSVC_RUNTIME_LIBRARY_DEFAULT TARGET ${PROJECT_NAME} PROPERTY MSVC_RUNTIME_LIBRARY)
if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
    string(CONCAT "MSVC_RUNTIME_LIBRARY_STR"
        $<$<CONFIG:Debug>:
            MultiThreadedDebug
        >
        $<$<CONFIG:Release>:
            MultiThreaded
        >
        $<$<NOT:$<OR:$<CONFIG:Debug>,$<CONFIG:Release>>>:${MSVC_RUNTIME_LIBRARY_DEFAULT}>
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
    string(CONCAT "MSVC_RUNTIME_LIBRARY_STR"
        $<$<CONFIG:Debug>:
            MultiThreadedDebug
        >
        $<$<CONFIG:Release>:
            MultiThreaded
        >
        $<$<NOT:$<OR:$<CONFIG:Debug>,$<CONFIG:Release>>>:${MSVC_RUNTIME_LIBRARY_DEFAULT}>
    )
endif()
set_target_properties(${PROJECT_NAME} PROPERTIES MSVC_RUNTIME_LIBRARY ${MSVC_RUNTIME_LIBRARY_STR})

################################################################################
# Compile definitions
################################################################################
if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
    target_compile_definitions(${PROJECT_NAME} PRIVATE
        "$<$<CONFIG:Debug>:"
            "_DEBUG"
        ">"
        "$<$<CONFIG:Release>:"
            "NDEBUG"
        ">"
        "_CONSOLE;"
        "NOMINMAX;"
        "UNICODE;"
        "_UNICODE"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
    target_compile_definitions(${PROJECT_NAME} PRIVATE
        "$<$<CONFIG:Debug>:"
            "_DEBUG"
        ">"
        "$<$<CONFIG:Release>:"
            "NDEBUG"
        ">"
        "WIN32;"
        "_CONSOLE;"
        "NOMINMAX;"
        "UNICODE;"
        "_UNICODE"
    )
endif()

################################################################################
# Compile and link options
################################################################################
if(MSVC)
    if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
        target_compile_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:Debug>:
                /Od
            >
            $<$<CONFIG:Release>:
                /O2;
                /Oi;
                /Gy;
                /FAs
            >
            /permissive-;
            /std:c++17;
            /sdl-;
            /arch:AVX;
            /W3;
            ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT};
            ${DEFAULT_CXX_EXCEPTION_HANDLING};
            /Y-
        )
    elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
        target_compile_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:Debug>:
                /Od
            >
            $<$<CONFIG:Release>:
                /O2;
                /Oi;
                /Gy
            >
            /permissive-;
            /std:c++17;
            /sdl-;
            /arch:AVX2;
            /W3;
            ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT};
            ${DEFAULT_CXX_EXCEPTION_HANDLING};
            /Y-
        )
    endif()
    if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
        target_link_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:Debug>:
                /DEBUG:FULL;
                /INCREMENTAL
            >
            $<$<CONFIG:Release>:
                /DEBUG;
                /OPT:REF;
                /OPT:ICF;
                /INCREMENTAL:NO
            >
            /SUBSYSTEM:CONSOLE
        )
    elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
        target_link_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:Debug>:
                /DEBUG:FULL;
                /INCREMENTAL
            >
            $<$<CONFIG:Release>:
                /DEBUG;
                /OPT:REF;
                /OPT:ICF;
                /INCREMENTAL:NO
            >
            /SUBSYSTEM:CONSOLE
        )
    endif()
endif()

