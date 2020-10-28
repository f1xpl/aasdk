cmake_minimum_required(VERSION 3.5.1)
project(aasdk LANGUAGES CXX)

option(AASDK_TEST "Build Unit Test Cases" OFF)
option(AASDK_CODE_COVERAGE "Build with Code Coverage" OFF)

set(base_directory ${CMAKE_CURRENT_SOURCE_DIR})
set(sources_directory ${base_directory}/src)

set(include_directory ${base_directory}/include)
set(include_ut_directory ${base_directory}/include_ut)

SET(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/cmake_modules/")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS_INIT} -Wall -pedantic")

set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)

add_definitions(-DBOOST_ALL_DYN_LINK)

if(WIN32)
    set(WINSOCK2_LIBRARIES "ws2_32")
endif(WIN32)

add_subdirectory(aasdk_proto)

find_package(Boost REQUIRED COMPONENTS system log OPTIONAL_COMPONENTS unit_test_framework)
find_package(libusb-1.0 REQUIRED)
find_package(Protobuf REQUIRED)
find_package(OpenSSL REQUIRED)

set(AASDK_PROTO_INCLUDE_DIRS ${CMAKE_CURRENT_BINARY_DIR})

include_directories(${AASDK_PROTO_INCLUDE_DIRS}
                        ${Boost_INCLUDE_DIRS}
                        ${LIBUSB_1_INCLUDE_DIRS}
                        ${PROTOBUF_INCLUDE_DIR}
                        ${OPENSSL_INCLUDE_DIR}
                        ${GTEST_INCLUDE_DIRS}
                        ${GMOCK_INCLUDE_DIRS}
                        ${include_directory}
                        ${include_ut_directory})

file(GLOB_RECURSE source_files ${sources_directory}/*.cpp)
file(GLOB_RECURSE include_files ${include_directory}/*.hpp)
file(GLOB_RECURSE tests_source_files ${sources_directory}/*.ut.cpp)
file(GLOB_RECURSE tests_include_files ${include_ut_directory}/*.hpp)

list(REMOVE_ITEM source_files ${tests_source_files})

add_library(aasdk SHARED
                ${source_files}
                ${include_files})

add_dependencies(aasdk aasdk_proto)
target_link_libraries(aasdk
                        aasdk_proto
                        Boost::system
                        Boost::log
                        protobuf::libprotobuf-lite
                        OpenSSL::SSL
                        OpenSSL::Crypto
                        ${LIBUSB_1_LIBRARIES}
                        ${WINSOCK2_LIBRARIES})

install(TARGETS aasdk LIBRARY DESTINATION lib)
install(DIRECTORY include/f1x/aasdk DESTINATION include/f1x)

if(AASDK_TEST)
    include(ExternalGtest)
    add_executable(aasdk_ut
                    ${tests_source_files}
                    ${tests_include_files})

    add_dependencies(aasdk_ut aasdk googletest)

    target_include_directories(aasdk_ut PUBLIC ${EXT_STAGING_DIR}/include)
    target_link_directories(aasdk_ut PUBLIC ${EXT_STAGING_DIR}/lib)
    target_link_libraries(aasdk_ut
                            aasdk
                            Boost::unit_test_framework
                            gmock${GTEST_DEBUG_POSTFIX}
                            gmock_main${GTEST_DEBUG_POSTFIX}
                            gtest${GTEST_DEBUG_POSTFIX}
                            gtest_main${GTEST_DEBUG_POSTFIX}
                            Threads::Threads)

    if(AASDK_CODE_COVERAGE)
        include(CodeCoverage)
        append_coverage_compiler_flags()
        setup_target_for_coverage(NAME aasdk_coverage EXECUTABLE aasdk_ut DEPENDENCIES aasdk_ut)
    endif(AASDK_CODE_COVERAGE)

    install(TARGETS aasdk_ut LIBRARY DESTINATION share/aasdk)
    
endif(AASDK_TEST)
