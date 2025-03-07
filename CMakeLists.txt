cmake_minimum_required (VERSION 3.6)

INCLUDE_DIRECTORIES(include . ${Boost_INCLUDE_DIR})

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
set(BOT_NAME "BumbleCee")

project(${BOT_NAME})
aux_source_directory("src" coresrc)
aux_source_directory("src/Audio" commands)
aux_source_directory("src/Commands" commands)
aux_source_directory("src/Queue" settings)
aux_source_directory("src/Settings" settings)
aux_source_directory("src/Utils" settings)
add_executable(${BOT_NAME} ${coresrc} ${commands} ${settings})

set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(THREADS_PREFER_PTHREAD_FLAG TRUE)
add_definitions( -DBOOST_ALL_NO_LIB )
set( Boost_USE_STATIC_LIBS ON )
find_package(Threads REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(Boost REQUIRED)

target_include_directories(${BOT_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${OPENSSL_INCLUDE_DIR}
    /usr/include/opus
)

target_link_libraries(${BOT_NAME}
    dpp
    opus
    ogg
    oggz
    ${CMAKE_THREAD_LIBS_INIT}
    ${OPENSSL_CRYPTO_LIBRARY} 
    ${OPENSSL_SSL_LIBRARY}
    ${Boost_LIBRARIES}
)

link_directories(/usr/lib)