set(PROJECT_NAME Playground)

set(Header_Files
    "Macros.h"
)

set(Header_Files__Compression
    "Compression/PNGLoader.h"
)

set(Header_Files__Compression__3rdParty
    "Compression/3rdParty/lodepng.h"
)

set(Header_Files__FileUtils
    "FileUtils/FileMacros.h"
    "FileUtils/IFile.h"
    "FileUtils/RawFile.h"
)

set(Header_Files__RasterData
    "RasterData/ColorSpace.h"
    "RasterData/Image2d.h"
    "RasterData/ImageLoader.h"
    "RasterData/ImageUtils.h"
)

set(Header_Files__Utils
    "Utils/IDataLoader.h"
    "Utils/Logger.h"
    "Utils/Random.h"
)

set(Source_Files__Compression
    "Compression/PNGLoader.cpp"
)

set(Source_Files__Compression__3rdParty
    "Compression/3rdParty/lodepng.cpp"
)

set(Source_Files__FileUtils
    "FileUtils/IFile.cpp"
    "FileUtils/RawFile.cpp"
)

set(Source_Files__RasterData
    "RasterData/ColorSpace.cpp"
    "RasterData/Image2d.cpp"
    "RasterData/ImageLoader.cpp"
    "RasterData/ImageUtils.cpp"
)

set(Source_Files__Utils
    "Utils/Logger.cpp"
)

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
        ${CMAKE_SOURCE_DIR}
        Compression
        Compression/3rdParty
        FileUtils
        RasterData
        Utils
)

################################################################################
# Target
################################################################################
add_library(${PROJECT_NAME} ${ALL_FILES})

target_include_directories(${PROJECT_NAME} PUBLIC ${Header_dirs})