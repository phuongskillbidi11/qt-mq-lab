cmake_minimum_required(VERSION 3.21)

project(qtmqlab LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

enable_testing()

if(WIN32)
    add_compile_definitions(NOMINMAX WIN32_LEAN_AND_MEAN)
endif()

add_subdirectory(external/base)
add_subdirectory(external/AMQP-CPP)

find_package(Qt5 5.15 REQUIRED COMPONENTS Core Network Widgets)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

use_base_feature(mq UI LINK amqpcpp)

add_executable(qtmqlab WIN32 app/main.cpp)
target_link_libraries(qtmqlab PRIVATE feature_mq)

# Without this, a fresh configure produces a tree that builds with exit 0 and then dies at
# 0xc0000135 (STATUS_DLL_NOT_FOUND) the moment anything runs — including ctest and F5 in Visual
# Studio. Putting Qt on PATH instead is the trap docs/gotchas.md warns about: it works on the
# machine that set it and nowhere else. The base's self-tests link Qt too and land in their own
# directory, so they need their own copy.
if(WIN32)
    get_target_property(_qmake_location Qt5::qmake IMPORTED_LOCATION)
    get_filename_component(_qt_bin_dir "${_qmake_location}" DIRECTORY)
    find_program(WINDEPLOYQT_EXECUTABLE windeployqt HINTS "${_qt_bin_dir}")

    if(NOT WINDEPLOYQT_EXECUTABLE)
        message(FATAL_ERROR
            "windeployqt not found next to qmake at ${_qt_bin_dir}. Without it the build "
            "succeeds and every executable then fails at startup with 0xc0000135.")
    endif()

    add_custom_command(TARGET qtmqlab POST_BUILD
        COMMAND "${WINDEPLOYQT_EXECUTABLE}" --no-translations --dir "$<TARGET_FILE_DIR:qtmqlab>"
                "$<TARGET_FILE:qtmqlab>"
        COMMAND "${WINDEPLOYQT_EXECUTABLE}" --no-translations
                --dir "${CMAKE_BINARY_DIR}/external/base" "$<TARGET_FILE:qtmqlab>"
        COMMENT "Deploying Qt runtime next to the executables and the base self-tests"
        VERBATIM)
endif()
