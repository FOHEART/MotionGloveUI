# CMakeList.txt : CMake project for libh2UI, include source and define
# project specific logic here.
#
cmake_minimum_required(VERSION 3.14)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT
      "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug>:EditAndContinue>,$<$<CONFIG:Debug>:ProgramDatabase>>")
endif()

set(PROJECT_NAME "MotionGloveUI")
project(${PROJECT_NAME} LANGUAGES CXX)

# Only keep Debug/Release in multi-config generators (e.g. Visual Studio)
if(CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "" FORCE)
endif()


# ########################  3rd party dependencies  #########################
include(FetchContent)
include(cmake/CPM.cmake)

# Ensure transitive deps don't build optional examples/tools
set(CSERIALPORT_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

# Try to auto-detect Qt5 via find_package (so we can derive Qt5_DIR from the discovered Qt5Core)
# This allows users to rely on find_package instead of hardcoding Qt5 paths.
find_package(Qt5Core QUIET)
if(TARGET Qt5::Core OR Qt5Core_FOUND)
  # Qt5Core_DIR is provided by the config package and points to the directory containing Qt5Config.cmake
  if(DEFINED Qt5Core_DIR)
    set(Qt5_DIR "${Qt5Core_DIR}" CACHE PATH "Detected Qt5 CMake directory from Qt5Core")
    message(STATUS "Detected Qt5_DIR from Qt5Core: ${Qt5_DIR}")
  else()
    message(STATUS "Qt5Core found but Qt5Core_DIR not defined; leaving Qt5_DIR as-is: ${Qt5_DIR}")
  endif()
else()
  message(STATUS "Qt5Core not found by initial probe; will use Qt5_DIR (if set) as a hint when locating Qt5 components")
endif()

# Prefer user/CI-provided hints. Only set a fallback when Qt5_DIR is not already defined.
if(NOT DEFINED Qt5_DIR OR Qt5_DIR STREQUAL "")
  if(WIN32)
    # set(Qt5_DIR "D:/Qt5/5.15.2/msvc2019_64/lib/cmake/Qt5" CACHE PATH "Path to Qt5Config.cmake")
  else()
    # set(Qt5_DIR "/opt/Qt/5.14.2/gcc_64/lib/cmake/Qt5" CACHE PATH "Path to Qt5Config.cmake")
  endif()
endif()


include(cmake/qt.cmake)
qt_standard_project_setup()

include(cmake/skipAutoMoc.cmake)


# Ensure Qt5 imported targets are available for linking
find_package(Qt5 REQUIRED COMPONENTS Core Network Gui Widgets)


CPMAddPackage(
    NAME CarbonUI
    GITHUB_REPOSITORY FOHEART/CarbonUI
    GIT_TAG main)

CPMAddPackage(
    NAME h2sdk
    GITHUB_REPOSITORY FOHEART/h2sdk
    GIT_TAG main)

if(WIN32)
  CPMAddPackage(
      NAME GloveSteamDriver
      GITHUB_REPOSITORY FOHEART/GloveSteamDriver
      GIT_TAG main)
endif()

# Collect all sources from ./src and ./ui (including subfolders)
file(GLOB_RECURSE PROJECT_SOURCES CONFIGURE_DEPENDS
  "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cc"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cxx"
  "${CMAKE_CURRENT_SOURCE_DIR}/ui/*.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/ui/*.cc"
  "${CMAKE_CURRENT_SOURCE_DIR}/ui/*.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/ui/*.cxx"
)

# Qt resource files (.qrc)
set(PROJECT_QRC_FILES
  "${CMAKE_CURRENT_SOURCE_DIR}/img.qrc"
)

file(GLOB_RECURSE PROJECT_HEADERS CONFIGURE_DEPENDS
  "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/*.hpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/inc/*.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/inc/*.hpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/include/*.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/ui/*.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/ui/*.hpp"
)

add_library(${PROJECT_NAME} SHARED
  ${PROJECT_SOURCES}
  ${PROJECT_HEADERS}
  ${PROJECT_QRC_FILES}
)

# Apply skip rules for transitive dependency headers that may be picked up by AUTOMOC.
skip_automoc_in_deps()

# Ensure AUTOUIC can locate .ui files referenced by generated ui_*.h includes
# (e.g. headers under `inc/` including `ui_<Form>.h`).
set_property(TARGET ${PROJECT_NAME} PROPERTY AUTOUIC_SEARCH_PATHS
  "${CMAKE_CURRENT_SOURCE_DIR}/ui"
)


# Keep folder tree in IDEs that support it
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES
  ${PROJECT_SOURCES} ${PROJECT_HEADERS}
)

# Output to ./lib/Debug and ./lib/Release
set_target_properties(${PROJECT_NAME} PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/lib"
  LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/lib"
  ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/lib"
  RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_CURRENT_SOURCE_DIR}/lib/Debug"
  LIBRARY_OUTPUT_DIRECTORY_DEBUG "${CMAKE_CURRENT_SOURCE_DIR}/lib/Debug"
  ARCHIVE_OUTPUT_DIRECTORY_DEBUG "${CMAKE_CURRENT_SOURCE_DIR}/lib/Debug"
  RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_CURRENT_SOURCE_DIR}/lib/Release"
  LIBRARY_OUTPUT_DIRECTORY_RELEASE "${CMAKE_CURRENT_SOURCE_DIR}/lib/Release"
  ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${CMAKE_CURRENT_SOURCE_DIR}/lib/Release"
)

# Exposed headers: ./include and project root
# Internal headers: ./inc
target_include_directories(${PROJECT_NAME}
  PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
  PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/inc"
)

# UIC-generated headers (ui_*.h) are produced into the build tree.
# Add the autogen include directories so `#include "ui_xxx.h"` works.
# Note: With Qt5 `qt5_wrap_ui`, headers can be generated directly under
# `${CMAKE_CURRENT_BINARY_DIR}`.
target_include_directories(${PROJECT_NAME}
  PRIVATE
    "${CMAKE_CURRENT_BINARY_DIR}"
    "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}_autogen/include"
)

# Macros from the original .vcxproj
if(MSVC)
target_compile_definitions(${PROJECT_NAME}
  PRIVATE
    UNICODE _WIN32 WIN64 QT_DLL  
    LIBH2UI_EXPORTS
    $<$<CONFIG:Release>:QT_NO_DEBUG;NDEBUG>
)
else()
target_compile_definitions(${PROJECT_NAME}
  PRIVATE
    UNICODE  QT_DLL  
    LIBH2UI_EXPORTS
    $<$<CONFIG:Release>:QT_NO_DEBUG;NDEBUG>
)
endif()

if(MSVC)
  target_compile_options(${PROJECT_NAME} PRIVATE /MP /Zc:wchar_t)
endif()

# Qt
# (CMAKE_AUTOMOC/AUTORCC/AUTOUIC are set above and on the target)

file(GLOB_RECURSE PROJECT_UI_FILES CONFIGURE_DEPENDS
  "${CMAKE_CURRENT_SOURCE_DIR}/ui/*.ui"
)




# Link Qt early so moc/uic/rcc tools are available and automoc can resolve includes.
target_link_libraries(${PROJECT_NAME} PRIVATE Qt5::Core Qt5::Network Qt5::Gui Qt5::Widgets)
# Link the CarbonUI target provided by CPM to the project executable
target_link_libraries(${PROJECT_NAME} PRIVATE CarbonUI)

qt5_wrap_ui(PROJECT_UI_HEADERS ${PROJECT_UI_FILES})

# Keep .ui files for IDE visibility.
target_sources(${PROJECT_NAME} PRIVATE ${PROJECT_UI_FILES})

# Third-party includes/libs inferred from libh2UI.vcxproj
# GloveSteamDriver is provided via CPM; do not use relative-path includes or link dirs.
set(PROJECT_THIRDPARTY_INCLUDE_DIRS "")

target_include_directories(${PROJECT_NAME} PRIVATE ${PROJECT_THIRDPARTY_INCLUDE_DIRS})

# No link_directories needed for CPM-provided dependencies


target_link_libraries(${PROJECT_NAME} PRIVATE
  h2sdk::h2sdk
)

if(WIN32)
  target_link_libraries(${PROJECT_NAME} PRIVATE GloveSteamDriver)
endif()


# TODO: Add tests and install targets if needed.
