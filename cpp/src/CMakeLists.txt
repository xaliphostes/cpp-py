add_library(algo SHARED
    algos.cxx
)

target_include_directories(algo
    PUBLIC
    "${PROJECT_SOURCE_DIR}/src/include"
)
