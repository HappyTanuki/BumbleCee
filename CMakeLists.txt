cmake_minimum_required (VERSION 3.16)
set(BOT_NAME "BumbleCee")
project(${BOT_NAME} LANGUAGES CXX)

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(BOOST_EXCLUDE_LIBRARIES nowide)

if(WIN32)
    set(OPENSSL_ROOT_DIR "C:/Program Files/OpenSSL-Win64")
    set(OPENSSL_INCLUDE_DIR "${OPENSSL_ROOT_DIR}/include")

    if(CMAKE_BUILD_TYPE STREQUAL Debug)
        set(OPENSSL_CRYPTO_LIBRARY "${OPENSSL_ROOT_DIR}/lib/VC/x64/MDd/libcrypto.lib")
        set(OPENSSL_SSL_LIBRARY "${OPENSSL_ROOT_DIR}/lib/VC/x64/MDd/libssl.lib")
    else()
        set(OPENSSL_CRYPTO_LIBRARY "${OPENSSL_ROOT_DIR}/lib/VC/x64/MD/libcrypto.lib")
        set(OPENSSL_SSL_LIBRARY "${OPENSSL_ROOT_DIR}/lib/VC/x64/MD/libssl.lib")
    endif()
endif()

find_package(OpenSSL REQUIRED)

include(FetchContent)

FetchContent_Declare(
    Boost
    URL "https://github.com/boostorg/boost/releases/download/boost-1.89.0/boost-1.89.0-cmake.7z"
    DOWNLOAD_EXTRACT_TIMESTAMP ON
    CMAKE_ARGS
        -DBOOST_USE_STATIC_LIBS=OFF       # DLL 사용
        -DBOOST_USE_STATIC_RUNTIME=OFF    # /MD, /MDd 사용
)
message(STATUS "Fetching and making available Boost...")
FetchContent_MakeAvailable(Boost)

FetchContent_Declare(
    dpp
    GIT_REPOSITORY "https://github.com/brainboxdotcc/DPP.git"
    GIT_TAG "v10.1.3"
    GIT_SHALLOW ON
    CMAKE_ARGS -DCMAKE_CXX_STANDARD=${CMAKE_CXX_STANDARD} -DCMAKE_CXX_STANDARD_REQUIRED=ON
)
message(STATUS "Fetching and making available dpp...")
FetchContent_MakeAvailable(dpp)

# -------------------------------------------------------------
# 플랫폼별 FFmpeg 바이너리 다운로드 및 링크
# -------------------------------------------------------------
if(CMAKE_HOST_SYSTEM_NAME STREQUAL "Windows")
    set(FFMPEG_URL "https://github.com/BtbN/FFmpeg-Builds/releases/download/latest/ffmpeg-master-latest-win64-lgpl-shared.zip")
    set(FFMPEG_SHA256HASH "SHA256=c95a9d4e030f694f33c85a7611204383a0bca906514a08d83d3858496b122f76")
    set(FFMPEG_ARCHIVE_NAME "ffmpeg-windows")
    set(FFMPEG_LIB_DIR "lib")
    set(FFMPEG_INCLUDE_DIR "include")
elseif(CMAKE_HOST_SYSTEM_NAME STREQUAL "Linux")
    set(FFMPEG_URL "https://github.com/BtbN/FFmpeg-Builds/releases/download/latest/ffmpeg-master-latest-linux64-lgpl-shared.tar.xz")
    set(FFMPEG_SHA256HASH "SHA256=ebf6b197ffe52d798504895b4c2b84114ad5d2b406ac76e18374b44d1184ade3")
    set(FFMPEG_ARCHIVE_NAME "ffmpeg-linux")
    set(FFMPEG_LIB_DIR "lib")
    set(FFMPEG_INCLUDE_DIR "include")
else()
    message(FATAL_ERROR "Unsupported platform for FFmpeg precompiled binaries.")
endif()

FetchContent_Declare(
    ffmpeg
    URL ${FFMPEG_URL}
    URL_HASH ${FFMPEG_SHA256HASH}
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

if(NOT AVCODEC_LIBRARY OR NOT AVFORMAT_LIBRARY OR NOT AVUTIL_LIBRARY)
    message(FATAL_ERROR "FFmpeg 라이브러리를 찾을 수 없습니다. 다운로드 경로를 확인해주세요.")
endif()

#file(GLOB_RECURSE CORES "src/*.cpp" "src/*.cxx" "src/*.cc")
#file(GLOB_RECURSE AUDIO_SOURCES "src/Audio/*.cpp" "src/Audio/*.cxx" "src/Audio/*.cc")
#file(GLOB_RECURSE COMMANDS_SOURCES "src/Commands/*.cpp" "src/Commands/*.cxx" "src/Commands/*.cc")
#file(GLOB_RECURSE QUEUE_SOURCES "src/Queue/*.cpp" "src/Queue/*.cxx" "src/Queue/*.cc")
#file(GLOB_RECURSE SETTINGS_SOURCES "src/Settings/*.cpp" "src/Settings/*.cxx" "src/Settings/*.cc")
#file(GLOB_RECURSE UTILS_SOURCES "src/Utils/*.cpp" "src/Utils/*.cxx" "src/Utils/*.cc")

# Combine all found source files into a single list
#set(ALL_SOURCE_FILES
#    ${CORES}
#    ${AUDIO_SOURCES}
#    ${COMMANDS_SOURCES}
#    ${QUEUE_SOURCES}
#    ${SETTINGS_SOURCES}
#    ${UTILS_SOURCES}
#)

add_executable(${BOT_NAME} src/main.cpp)

target_link_libraries(${BOT_NAME} PRIVATE dpp)
target_link_libraries(${BOT_NAME} PRIVATE Boost::filesystem)
target_link_libraries(${BOT_NAME} PRIVATE Boost::process)
target_link_libraries(${BOT_NAME} PRIVATE Boost::log)
target_link_libraries(${BOT_NAME} PRIVATE Boost::beast)
target_link_libraries(${BOT_NAME} PRIVATE OpenSSL::Crypto)
target_link_libraries(${BOT_NAME} PRIVATE OpenSSL::SSL)
target_link_libraries(${BOT_NAME} PRIVATE ${AVUTIL_LIBRARY})
target_link_libraries(${BOT_NAME} PRIVATE ${AVCODEC_LIBRARY})
target_link_libraries(${BOT_NAME} PRIVATE ${AVFORMAT_LIBRARY})
target_link_libraries(${BOT_NAME} PRIVATE ${SWSCALE_LIBRARY})

if(WIN32)
    target_link_libraries(${BOT_NAME} PRIVATE ws2_32)
endif()
if(UNIX AND NOT APPLE AND CMAKE_BUILD_TYPE STREQUAL "Release")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        BUILD_WITH_INSTALL_RPATH TRUE
        INSTALL_RPATH "$ORIGIN"
        SKIP_BUILD_RPATH FALSE
        BUILD_RPATH "$ORIGIN"
    )
endif()

target_include_directories(${BOT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_include_directories(${BOT_NAME} PRIVATE ${OpenSSL_INCLUDE_DIRS})
target_include_directories(${BOT_NAME} PRIVATE ${FFMPEG_INCLUDE_PATH})

add_custom_command(TARGET ${BOT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "$<TARGET_FILE_DIR:Boost::filesystem>"
    "$<TARGET_FILE_DIR:${BOT_NAME}>"
    COMMENT "Copying Boost.Filesystem DLL/so files to output directory"
)

add_custom_command(TARGET ${BOT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "$<TARGET_FILE_DIR:Boost::process>"
    "$<TARGET_FILE_DIR:${BOT_NAME}>"
    COMMENT "Copying Boost.Process DLL/so files to output directory"
)

add_custom_command(TARGET ${BOT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_BINARY_DIR}/${FFMPEG_ARCHIVE_NAME}/bin"
    "$<TARGET_FILE_DIR:${BOT_NAME}>"
    COMMENT "Copying FFMpeg_AVCODEC DLL/so files to output directory"
)