project(MTwister C)

add_library(MTwister src/mtwister.c)

target_include_directories(MTwister PUBLIC
                           ${CMAKE_CURRENT_SOURCE_DIR}/include
)