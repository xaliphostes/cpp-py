add_library(algo SHARED
    src/point.cxx
    src/triangle.cxx
)

target_include_directories(algo
    PUBLIC
    "${PROJECT_SOURCE_DIR}/lib/include"
)
