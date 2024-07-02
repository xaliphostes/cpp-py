cmake_minimum_required(VERSION 3.4...3.18)
project(pyalgo)

include_directories(../cpp/src/include/)

find_library(ALGO algo PATHS ../bin/ REQUIRED)

add_subdirectory(pybind11)
pybind11_add_module(pyalgo src/binder.cxx)

target_link_libraries(${PROJECT_NAME} PRIVATE ${ALGO}) # The PRIVATE keyword is important

MESSAGE("version is ${EXAMPLE_VERSION_INFO}")
target_compile_definitions(pyalgo PRIVATE VERSION_INFO=${EXAMPLE_VERSION_INFO})
