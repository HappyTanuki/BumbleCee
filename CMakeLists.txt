cmake_minimum_required (VERSION 3.16)
set(BOT_NAME "BumbleCee")
project(${BOT_NAME} LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(BOOST_EXCLUDE_LIBRARIES nowide)
set(BUILD_TESTING ON)

set(DPP_BUILD_TEST OFF)

enable_testing()

if(WIN32)
    set(OPENSSL_ROOT_DIR "C:/Program Files/OpenSSL-Win64")
    set(OPENSSL_INCLUDE_DIR "${OPENSSL_ROOT_DIR}/include")

    if(CMAKE_BUILD_TYPE STREQUAL Debug)
        set(OPENSSL_CRYPTO_LIBRARY "${OPENSSL_ROOT_DIR}/lib/VC/x64/MTd/libcrypto.lib")
        set(OPENSSL_SSL_LIBRARY "${OPENSSL_ROOT_DIR}/lib/VC/x64/MTd/libssl.lib")
    else()
        set(OPENSSL_CRYPTO_LIBRARY "${OPENSSL_ROOT_DIR}/lib/VC/x64/MT/libcrypto.lib")
        set(OPENSSL_SSL_LIBRARY "${OPENSSL_ROOT_DIR}/lib/VC/x64/MT/libssl.lib")
    endif()
endif()

find_package(OpenSSL REQUIRED)

include(FetchContent)

set(BUILD_SHARED_LIBS_OLD ${BUILD_SHARED_LIBS})

FetchContent_Declare(
    Boost
    URL "https://github.com/boostorg/boost/releases/download/boost-1.89.0/boost-1.89.0-cmake.7z"
    DOWNLOAD_EXTRACT_TIMESTAMP ON
    EXCLUDE_FROM_ALL
)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "Do not build SHARED libraries" FORCE)
message(STATUS "Fetching and making available Boost...")
FetchContent_MakeAvailable(Boost)

FetchContent_Declare(
    dpp
    GIT_REPOSITORY "https://github.com/brainboxdotcc/DPP.git"
    GIT_TAG "v10.1.3"
    GIT_SHALLOW ON
)
set(BUILD_SHARED_LIBS ON CACHE BOOL "Build SHARED libraries" FORCE)
message(STATUS "Fetching and making available dpp...")
FetchContent_MakeAvailable(dpp)

set(BUILD_SHARED_LIBS ${BUILD_SHARED_LIBS_OLD} CACHE BOOL "Type of libraries to build" FORCE)

# -------------------------------------------------------------
# 플랫폼별 FFmpeg 바이너리 다운로드 및 링크
# -------------------------------------------------------------
if(CMAKE_HOST_SYSTEM_NAME STREQUAL "Windows")
    set(FFMPEG_URL "https://github.com/BtbN/FFmpeg-Builds/releases/download/latest/ffmpeg-master-latest-win64-lgpl-shared.zip")
    set(FFMPEG_ARCHIVE_NAME "ffmpeg-windows")
    set(FFMPEG_LIB_DIR "lib")
    set(FFMPEG_INCLUDE_DIR "include")
elseif(CMAKE_HOST_SYSTEM_NAME STREQUAL "Linux")
    set(FFMPEG_URL "https://github.com/BtbN/FFmpeg-Builds/releases/download/latest/ffmpeg-master-latest-linux64-lgpl-shared.tar.xz")
    set(FFMPEG_ARCHIVE_NAME "ffmpeg-linux")
    set(FFMPEG_LIB_DIR "lib")
    set(FFMPEG_INCLUDE_DIR "include")
else()
    message(FATAL_ERROR "Unsupported platform for FFmpeg precompiled binaries.")
endif()

FetchContent_Declare(
    ffmpeg
    URL ${FFMPEG_URL}
    DOWNLOAD_EXTRACT_TIMESTAMP ON
    SOURCE_DIR ${CMAKE_BINARY_DIR}/${FFMPEG_ARCHIVE_NAME}
)
message(STATUS "Fetching and making available ffmpeg...")
FetchContent_MakeAvailable(ffmpeg)

set(FFMPEG_LIBRARY_PATH ${ffmpeg_SOURCE_DIR}/${FFMPEG_LIB_DIR})
set(FFMPEG_INCLUDE_PATH ${ffmpeg_SOURCE_DIR}/${FFMPEG_INCLUDE_DIR})

find_library(AVCODEC_LIBRARY NAMES avcodec PATHS ${FFMPEG_LIBRARY_PATH} NO_DEFAULT_PATH)
message(STATUS "AVCODEC_LIBRARY = ${AVCODEC_LIBRARY}")
find_library(AVFORMAT_LIBRARY NAMES avformat PATHS ${FFMPEG_LIBRARY_PATH} NO_DEFAULT_PATH)
message(STATUS "AVFORMAT_LIBRARY = ${AVFORMAT_LIBRARY}")
find_library(AVUTIL_LIBRARY NAMES avutil PATHS ${FFMPEG_LIBRARY_PATH} NO_DEFAULT_PATH)
message(STATUS "AVUTIL_LIBRARY = ${AVUTIL_LIBRARY}")
find_library(SWSCALE_LIBRARY NAMES swscale PATHS ${FFMPEG_LIBRARY_PATH} NO_DEFAULT_PATH)
message(STATUS "SWSCALE_LIBRARY = ${SWSCALE_LIBRARY}")
find_library(SWRESAMPLE_LIBRARY NAMES swresample PATHS ${FFMPEG_LIBRARY_PATH} NO_DEFAULT_PATH)
message(STATUS "SWRESAMPLE_LIBRARY = ${SWRESAMPLE_LIBRARY}")

if(NOT AVCODEC_LIBRARY OR NOT AVFORMAT_LIBRARY OR NOT AVUTIL_LIBRARY OR NOT SWRESAMPLE_LIBRARY)
    message(FATAL_ERROR "FFmpeg 라이브러리를 찾을 수 없습니다. 다운로드 경로를 확인해주세요.")
endif()

file(GLOB_RECURSE SOURCES "src/*.cpp" "src/*.cxx" "src/*.cc")

add_library(${BOT_NAME}_lib ${SOURCES})

target_include_directories(${BOT_NAME}_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_include_directories(${BOT_NAME}_lib PUBLIC ${OpenSSL_INCLUDE_DIRS})
target_include_directories(${BOT_NAME}_lib PUBLIC ${FFMPEG_INCLUDE_PATH})

target_precompile_headers(${BOT_NAME}_lib PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include/precomp.h")

target_link_libraries(${BOT_NAME}_lib PUBLIC dpp)
target_link_libraries(${BOT_NAME}_lib PUBLIC Boost::filesystem)
target_link_libraries(${BOT_NAME}_lib PUBLIC Boost::process)
target_link_libraries(${BOT_NAME}_lib PUBLIC Boost::log)
target_link_libraries(${BOT_NAME}_lib PUBLIC Boost::beast)
target_link_libraries(${BOT_NAME}_lib PUBLIC OpenSSL::Crypto)
target_link_libraries(${BOT_NAME}_lib PUBLIC OpenSSL::SSL)
target_link_libraries(${BOT_NAME}_lib PUBLIC ${AVUTIL_LIBRARY})
target_link_libraries(${BOT_NAME}_lib PUBLIC ${AVCODEC_LIBRARY})
target_link_libraries(${BOT_NAME}_lib PUBLIC ${AVFORMAT_LIBRARY})
target_link_libraries(${BOT_NAME}_lib PUBLIC ${SWSCALE_LIBRARY})
target_link_libraries(${BOT_NAME}_lib PUBLIC ${SWRESAMPLE_LIBRARY})

if(WIN32)
    target_link_libraries(${BOT_NAME}_lib PUBLIC ws2_32)
endif()
if(UNIX AND NOT APPLE)
    set_target_properties(${BOT_NAME}_lib PROPERTIES
        BUILD_WITH_INSTALL_RPATH TRUE
        INSTALL_RPATH "$ORIGIN"
        SKIP_BUILD_RPATH FALSE
        BUILD_RPATH "$ORIGIN"
    )
endif()

add_executable(${BOT_NAME} ${SOURCES})

target_link_libraries(${BOT_NAME} PUBLIC ${BOT_NAME}_lib)

file(GLOB FFMPEG_SHARED
    "${CMAKE_BINARY_DIR}/${FFMPEG_ARCHIVE_NAME}/bin/*.dll"   # Windows
    "${CMAKE_BINARY_DIR}/${FFMPEG_ARCHIVE_NAME}/lib/*.so*"    # Linux
)

foreach(ffmpeg_shared ${FFMPEG_SHARED})
    add_custom_command(TARGET ${BOT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${ffmpeg_shared}"
                "$<TARGET_FILE_DIR:${BOT_NAME}>"
        COMMENT "Copying ${ffmpeg_shared} to output directory"
    )
endforeach()

add_custom_command(TARGET ${BOT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "$<TARGET_FILE:dpp>"
    "$<TARGET_FILE_DIR:${BOT_NAME}>"
    COMMENT "Copying dpp DLL/so files to output directory"
)

add_subdirectory(tests)