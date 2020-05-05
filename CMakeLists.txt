cmake_minimum_required(VERSION 3.16)
project(ModernCppCourse LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)

add_executable(checkpoint_0 checkpoint_0/main.cpp)
add_executable(checkpoint_1 checkpoint_1/main.cpp)
add_executable(checkpoint_2 checkpoint_2/main.cpp)
add_executable(checkpoint_3 checkpoint_3/main.cpp)
add_executable(checkpoint_4 checkpoint_4/main.cpp)
add_executable(checkpoint_5 checkpoint_5/main.cpp)
add_executable(checkpoint_6 checkpoint_6/main.cpp)
add_executable(checkpoint_7 checkpoint_7/main.cpp)

# Currently need to link in TBB for parallel STL
find_library(TBB NAMES libtbb.so)
target_link_libraries(checkpoint_0 ${TBB})
target_link_libraries(checkpoint_1 ${TBB})
target_link_libraries(checkpoint_2 ${TBB})
target_link_libraries(checkpoint_3 ${TBB})
target_link_libraries(checkpoint_4 ${TBB})
target_link_libraries(checkpoint_5 ${TBB})
target_link_libraries(checkpoint_6 ${TBB})
target_link_libraries(checkpoint_7 ${TBB})

option(SLIDES "Add target for making slides" OFF)
if (SLIDES)
    find_program(PANDOC pandoc)
    find_path(SLIDES_DIR ModernCppCourse.md PATHS ${CMAKE_CURRENT_SOURCE_DIR}/slides NO_DEFAULT_PATH)
    set(SLIDES_MD ${SLIDES_DIR}/ModernCppCourse.md)
    set(SLIDES_PDF ${SLIDES_DIR}/ModernCppCourse.pdf)
    add_custom_target(
            slides COMMAND
            ${PANDOC} -f markdown -t beamer --highlight-style ${SLIDES_DIR}/pygments_custom.theme
            ${SLIDES_MD} -V theme:metropolis -V aspectratio:169 -o ${SLIDES_PDF}
    )

    find_package (Python3 COMPONENTS Interpreter)
    add_custom_target(
            test_slides COMMAND
            Python3::Interpreter ${SLIDES_DIR}/test_slides.py ${CMAKE_CXX_COMPILER}
    )

endif ()
