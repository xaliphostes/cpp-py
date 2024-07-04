add_library(algo SHARED
    src/algos.cxx
)

target_include_directories(algo
    PUBLIC
    "${PROJECT_SOURCE_DIR}/lib/include"
)
