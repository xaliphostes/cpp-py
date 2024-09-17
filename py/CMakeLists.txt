cmake_minimum_required(VERSION 3.16)
project(pyalgo)

find_package(Python COMPONENTS Interpreter Development.Module REQUIRED)

include_directories(../cpp/lib/include/)

find_library(ALGO algo PATHS ../bin/ REQUIRED)

add_subdirectory(pybind11)
pybind11_add_module(pyalgo src/binder.cxx)

target_link_libraries(${PROJECT_NAME} PRIVATE ${ALGO}) # The PRIVATE keyword is important

install(TARGETS pyalgo DESTINATION .)
