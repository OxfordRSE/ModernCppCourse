cmake_minimum_required(VERSION 3.16)
project(ModernCppCourse LANGUAGES CXX)


########################################################################################################################
### Check compiler is either GNU or MSVC, and that version is recent enough ############################################
########################################################################################################################
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    if (CMAKE_CXX_COMPILER_VERSION VERSION_LESS "9.2")
        message(FATAL_ERROR "Detected GNU compiler version is too old: found v${CMAKE_CXX_COMPILER_VERSION} but 9.2 or newer is required.")
    endif ()
    # GCC's standard library requires Intel's TBB for parallel algorithms
    find_library(TBB NAMES libtbb.so)
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    if (MSVC_VERSION VERSION_LESS "1921")
        message(FATAL_ERROR "Detected MSVC compiler version is too old: found v${MSVC_VERSION} but v1921 or newer (Visual Studio 2019 version 16.1) is required.")
    endif ()
else ()
    message(FATAL_ERROR "Detected ${CMAKE_CXX_COMPILER_ID}, but GNU or MSVC compiler is required.")
endif ()

set(CMAKE_CXX_STANDARD 20)


########################################################################################################################
### Add executable for checkpoints 0, ..., 7, linking TBB if required, and test fixtures for CI ########################
########################################################################################################################
add_compile_definitions(COURSE_DIR="${CMAKE_CURRENT_SOURCE_DIR}")
foreach (NUM RANGE 7)
    add_executable(checkpoint_${NUM} checkpoint_${NUM}/main.cpp)
    if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        target_link_libraries(checkpoint_${NUM} ${TBB})
    endif ()
    add_test(NAME test_checkpoint_${NUM} COMMAND checkpoint_${NUM})
endforeach ()
enable_testing()


########################################################################################################################
### Optional target to compile the slideshow, which requires Pandoc and LaTeX ##########################################
########################################################################################################################
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

    find_package(Python3 COMPONENTS Interpreter)
    add_custom_target(
            test_slides COMMAND
            Python3::Interpreter ${SLIDES_DIR}/test_slides.py ${CMAKE_CXX_COMPILER}
    )

endif ()
