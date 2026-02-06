# CMake lowest version requirement
cmake_minimum_required(VERSION 3.10)

set(UPDATED_PROGRAM_VERSION "1.0.0")

#设置C编译器
set(CMAKE_C_COMPILER /root/sdk/opt/linux/x86-arm/aarch64-v01c01-linux-gnu-gcc/bin/aarch64-v01c01-linux-gnu-gcc)

#设置C++编译器
set(CMAKE_CXX_COMPILER /root/sdk/opt/linux/x86-arm/aarch64-v01c01-linux-gnu-gcc/bin/aarch64-v01c01-linux-gnu-g++)

string(TIMESTAMP BUILD_DATA_ "%Y%m%d")
string(TIMESTAMP BUILD_DATA_SIMPLE_ "%y%m%d")
string(TIMESTAMP BUILD_TIME "%H%M%S")
set(BUILD_DATA_BUILD_TIME "${BUILD_DATA_}-${BUILD_TIME}")
set(BUILD_DATA_SIMPLE_BUILD_TIME "${BUILD_DATA_SIMPLE_}.${BUILD_TIME}")
option(BUILD_TEST "build with test app" OFF)

# project information
project(ACL_DET_YOLO)
include(set_env.cmake)


# Compile options
add_compile_options(
        -std=c++11
        -fPIC
        -fstack-protector-all
        -D_FORTIFY_SOURCE=2 -O2
)

# Specify target generation path
#set(CMAKE_RUNTIME_OUTPUT_DIRECTORY  "../../../out")
set(CMAKE_CXX_FLAGS_DEBUG "-fPIC -O0 -g -Wall -Wno-sign-compare -Wno-unknown-pragmas -Wno-unused-value -Wno-unused-variable -Wno-unused-but-set-variable -Wno-unused-function")
set(CMAKE_CXX_FLAGS_RELEASE "-fPIC -O2 -Wall -s -Wno-sign-compare -Wno-unused-value -Wno-unknown-pragmas -Wno-unused-variable -Wno-unused-but-set-variable -Wno-unused-function" )
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-z,relro,-z,now,-z,noexecstack -fPIE -pie -Wno-sign-compare -Wno-unused-value -Wno-unknown-pragmas -Wno-unused-variable -Wno-unused-but-set-variable-Wno-unused-function")

set(CMAKE_SKIP_RPATH FALSE)
set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
set(CMAKE_INSTALL_RPATH "/tmp/app/exec" "/media/feature/lib" ".")

option(OPENCV "use opencv" OFF)

# Header path
set(OPENCV_INC $ENV{DDK_PATH}/atc/third_party/opencv/include/opencv4)


# add host lib path
if (target MATCHES "Simulator_Function" OR target MATCHES "Simulator_Instruction" OR target MATCHES "Simulator_Performance")
    set(ENV{NPU_HOST_LIB} $ENV{DDK_PATH}/toolkit/tools/sim/lib)
#    set(OPENCV_LIB $ENV{DDK_PATH}/atc/third_party_lib)
else()
    set(ENV{NPU_HOST_LIB} $ENV{DDK_PATH}/acllib/lib64/stub)
    if (CMAKE_CXX_COMPILER MATCHES "aarch64-v01c01-linux-musl-gcc")
        set(OPENCV_LIB ${PROJECT_SOURCE_DIR}/third_party/musl/opencv/lib)
    else()
        set(OPENCV_LIB ${PROJECT_SOURCE_DIR}/third_party/glibc/opencv/lib)
    endif()
endif()

#set(OPENCV_LIB ${PROJECT_SOURCE_DIR}/third_party/glibc/opencv/lib)

set(LIB_PATH $ENV{NPU_HOST_LIB}
        ${CMAKE_SOURCE_DIR}/third_party/glibc/spdlog/lib
        ${CMAKE_SOURCE_DIR}/third_party/glibc/vulcan/lib
        ${CMAKE_SOURCE_DIR}/third_party/glibc/jsoncpp/lib
        ${CMAKE_SOURCE_DIR}/third_party/glibc/opencv/lib)
MESSAGE(NPU_HOST_LIB=$ENV{NPU_HOST_LIB})
link_directories(${LIB_PATH})

add_subdirectory(src)



## version control
add_subdirectory(${CMAKE_SOURCE_DIR}/version_control)

if (BUILD_TEST)
    add_subdirectory(test)
endif ()






## version control
