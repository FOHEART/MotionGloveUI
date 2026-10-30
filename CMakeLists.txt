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

# Require C++14: std::make_unique and other C++14 features are used throughout the codebase.
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
# Build as shared library by default. Allow parent projects/CI to override.
if(NOT DEFINED MOTIONGLOVE_UI_BUILD_SHARED)
  option(MOTIONGLOVE_UI_BUILD_SHARED "Build MotionGloveUI as a shared library" ON)
else()
  message(STATUS "MOTIONGLOVE_UI_BUILD_SHARED provided by caller: ${MOTIONGLOVE_UI_BUILD_SHARED}")
endif()

# Only keep Debug/Release in multi-config generators (e.g. Visual Studio)
if(CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "" FORCE)
endif()


# ########################  3rd party dependencies  #########################
include(FetchContent)
include(cmake/CPM.cmake)

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

include(cmake/qt.cmake)
qt_standard_project_setup()

# Ensure Qt5 imported targets are available for linking
find_package(Qt5 REQUIRED COMPONENTS Core Network Gui Widgets Test)

# GitHub Actions runners typically do not have an SSH deploy key configured.
# Default to HTTPS cloning and allow opting into SSH with -DXSTYPES_FETCH_VIA_SSH=ON.
# GitHub Actions 运行环境通常未配置 SSH Deploy Key。
# 默认使用 HTTPS 拉取；可通过 -DXSTYPES_FETCH_VIA_SSH=ON 切换为 SSH。
option(XSTYPES_FETCH_VIA_SSH "Fetch xstypes_x via SSH (requires configured SSH key)" OFF)
if (XSTYPES_FETCH_VIA_SSH)
  set(CARBONUI_GIT_REPOSITORY "git@github.com:FOHEART/CarbonUI.git")
  set(H2SDK_GIT_REPOSITORY "git@github.com:FOHEART/MotionGloveLLSDK.git")
  set(GLOVESTEAMDRIVER_GIT_REPOSITORY "git@github.com:FOHEART/GloveSteamDriver.git")
else()
  set(CARBONUI_GIT_REPOSITORY "https://github.com/FOHEART/CarbonUI.git")
  set(H2SDK_GIT_REPOSITORY "https://github.com/FOHEART/MotionGloveLLSDK.git")
  set(GLOVESTEAMDRIVER_GIT_REPOSITORY "https://github.com/FOHEART/GloveSteamDriver.git")
endif()

CPMAddPackage(
    NAME CarbonUI
    GIT_REPOSITORY ${CARBONUI_GIT_REPOSITORY}
    GIT_TAG main
     # Prevent update attempts that require deleting the existing source dir.
    # This improves reliability on Windows/CI where files can be temporarily locked.
    # 防止更新时尝试删除现有源码目录。
    # 这可提升 Windows/CI 环境可靠性（文件可能被临时锁定）。
    # Prevent update attempts that require deleting the existing source dir.
    # This improves reliability on Windows/CI where files can be temporarily locked.
    UPDATE_DISCONNECTED 1)

CPMAddPackage(
    NAME MotionGloveLLSDK
    GIT_REPOSITORY ${H2SDK_GIT_REPOSITORY}
    GIT_TAG main
     # Prevent update attempts that require deleting the existing source dir.
    # This improves reliability on Windows/CI where files can be temporarily locked.
    # 防止更新时尝试删除现有源码目录。
    # 这可提升 Windows/CI 环境可靠性（文件可能被临时锁定）。
    # Prevent update attempts that require deleting the existing source dir.
    # This improves reliability on Windows/CI where files can be temporarily locked.
    UPDATE_DISCONNECTED 1)

if(WIN32)
  CPMAddPackage(
      NAME GloveSteamDriver
      GIT_REPOSITORY ${GLOVESTEAMDRIVER_GIT_REPOSITORY}
    GIT_TAG main
  
     # Prevent update attempts that require deleting the existing source dir.
    # This improves reliability on Windows/CI where files can be temporarily locked.
    # 防止更新时尝试删除现有源码目录。
    # 这可提升 Windows/CI 环境可靠性（文件可能被临时锁定）。
    # Prevent update attempts that require deleting the existing source dir.
    # This improves reliability on Windows/CI where files can be temporarily locked.
    UPDATE_DISCONNECTED 1)
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

# ########################  pure logic core  #########################
# src/core 只依赖 Qt5::Core，无 QWidget 调用，单独编译成静态库，
# 供库本体与 tests/ 共同链接 —— 保证被测代码与出货代码是同一份源码、同一套编译选项。
list(FILTER PROJECT_SOURCES EXCLUDE REGEX "/src/core/")
list(FILTER PROJECT_HEADERS EXCLUDE REGEX "/src/core/")

file(GLOB_RECURSE CORE_SOURCES CONFIGURE_DEPENDS
  "${CMAKE_CURRENT_SOURCE_DIR}/src/core/*.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/core/*.cpp"
)
file(GLOB_RECURSE CORE_HEADERS CONFIGURE_DEPENDS
  "${CMAKE_CURRENT_SOURCE_DIR}/src/core/*.h"
)

add_library(MotionGloveUICore STATIC ${CORE_SOURCES} ${CORE_HEADERS})
target_include_directories(MotionGloveUICore PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src")
target_link_libraries(MotionGloveUICore PUBLIC Qt5::Core)
set_target_properties(MotionGloveUICore PROPERTIES POSITION_INDEPENDENT_CODE ON)

if(MSVC)
  target_compile_options(MotionGloveUICore PRIVATE /utf-8 /MP /Zc:wchar_t)
else()
  target_compile_options(MotionGloveUICore PRIVATE -finput-charset=UTF-8 -fexec-charset=UTF-8)
endif()

if(MOTIONGLOVE_UI_BUILD_SHARED)
  add_library(${PROJECT_NAME} SHARED
    ${PROJECT_SOURCES}
  )
else()
  add_library(${PROJECT_NAME} STATIC
    ${PROJECT_SOURCES}
  )
endif()

# Headers are kept for IDE visibility only (no autogen scanning — all AUTO* are OFF).
target_sources(${PROJECT_NAME} PRIVATE ${PROJECT_HEADERS})


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

# UIC-generated headers (ui_*.h) are produced into the build tree by qt5_wrap_ui.
# Add the binary dir so `#include "ui_xxx.h"` works.
target_include_directories(${PROJECT_NAME}
  PRIVATE
    "${CMAKE_CURRENT_BINARY_DIR}"
)
# Macros from the original .vcxproj
if(MSVC)
target_compile_definitions(${PROJECT_NAME}
  PRIVATE
     _WIN32 WIN64 QT_DLL
    $<$<BOOL:${MOTIONGLOVE_UI_BUILD_SHARED}>:LIBH2UI_EXPORTS>
    $<$<NOT:$<BOOL:${MOTIONGLOVE_UI_BUILD_SHARED}>>:LIBH2UI_STATIC_LIB>
    $<$<CONFIG:Release>:QT_NO_DEBUG;NDEBUG>
)
else()
target_compile_definitions(${PROJECT_NAME}
  PRIVATE
     QT_DLL
    $<$<BOOL:${MOTIONGLOVE_UI_BUILD_SHARED}>:LIBH2UI_EXPORTS>
    $<$<NOT:$<BOOL:${MOTIONGLOVE_UI_BUILD_SHARED}>>:LIBH2UI_STATIC_LIB>
    $<$<CONFIG:Release>:QT_NO_DEBUG;NDEBUG>
)
endif()

if (MSVC)
  # Add MSVC-specific macro to suppress CRT secure warnings
  target_compile_options(${PROJECT_NAME} PRIVATE /utf-8 /MP /Zc:wchar_t /D_UNICODE /DUNICODE /D_CRT_SECURE_NO_WARNINGS)
else()
  target_compile_options(${PROJECT_NAME} PRIVATE -finput-charset=UTF-8 -fexec-charset=UTF-8 -D_UNICODE -DUNICODE)
endif()


file(GLOB_RECURSE PROJECT_UI_FILES CONFIGURE_DEPENDS
  "${CMAKE_CURRENT_SOURCE_DIR}/ui/*.ui"
)

# Remove Windows-only .ui files on non-Windows platforms
if(NOT WIN32)
  list(FILTER PROJECT_UI_FILES EXCLUDE REGEX ".*/steamvr/h2SteamVRMonitorWidget\\.ui$")
  list(FILTER PROJECT_UI_FILES EXCLUDE REGEX ".*/steamvr/libH2SteamDriverUI\\.ui$")
endif()

# ── Manual Qt code generation (AUTO* are all OFF) ──────────────────────────

# 1. UIC: generate ui_*.h from every .ui file
qt5_wrap_ui(PROJECT_UI_HEADERS ${PROJECT_UI_FILES})

# 2. MOC: generate moc_*.cpp for every header that contains Q_OBJECT / Q_GADGET
set(PROJECT_MOC_HEADERS
  # include/
  "${CMAKE_CURRENT_SOURCE_DIR}/include/h2Widget.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/include/h2GloveDeviceTreeContainerWidget.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/include/h2GloveDeviceTreeWidget.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/include/h2SettingsDialog.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/include/h2StreamConfigWidget.h"
  # ui/ (flat)
  "${CMAKE_CURRENT_SOURCE_DIR}/ui/h2CSVConfig.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/ui/h2CsvRecord.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/ui/h2NetBroadcastWidget.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/ui/h2IndexSimuWidget.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/ui/h2Notusable.h"
  # ui/finger/
  "${CMAKE_CURRENT_SOURCE_DIR}/ui/finger/h2FingerAdjustEnhancedWidget.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/ui/finger/h2FingerAdjustFreeModeWidget.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/ui/finger/h2FingerAdjustLockYawWidget.h"
  # ui/gesture/
  "${CMAKE_CURRENT_SOURCE_DIR}/ui/gesture/h2GestureCompare.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/ui/gesture/h2GestureContainer.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/ui/gesture/h2GestureDisplay.h"
  # ui/joystick/
  "${CMAKE_CURRENT_SOURCE_DIR}/ui/joystick/h2FingerClose.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/ui/joystick/h2JoystickABXY.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/ui/joystick/h2JoystickCali.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/ui/joystick/h2JoyStickPadWidget.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/ui/joystick/h2JoystickWidget.h"
  # ui/skeleton/
  "${CMAKE_CURRENT_SOURCE_DIR}/ui/skeleton/h2SkeletonWidget.h"
  # inc/
  "${CMAKE_CURRENT_SOURCE_DIR}/inc/h2GloveSepMgr.h"
)

# Windows-only headers: Q_OBJECT is inside #ifdef _WIN32 in these files
if(WIN32)
  list(APPEND PROJECT_MOC_HEADERS
    "${CMAKE_CURRENT_SOURCE_DIR}/include/libH2SteamDriverUI.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/ui/steamvr/h2SteamVRMonitorWidget.h"
  )
endif()

qt5_wrap_cpp(PROJECT_MOC_SOURCES
  ${PROJECT_MOC_HEADERS}
  TARGET ${PROJECT_NAME}
)

# 3. RCC: compile Qt resource files (.qrc) into C++ source
qt5_add_resources(PROJECT_RCC_SOURCES
  "${CMAKE_CURRENT_SOURCE_DIR}/img.qrc"
)

# Wire all generated sources into the target
target_sources(${PROJECT_NAME} PRIVATE
  ${PROJECT_UI_HEADERS}
  ${PROJECT_MOC_SOURCES}
  ${PROJECT_RCC_SOURCES}
)

# Keep .ui files visible in the IDE
target_sources(${PROJECT_NAME} PRIVATE ${PROJECT_UI_FILES})

# ───────────────────────────────────────────────────────────────────────────

# Link Qt
target_link_libraries(${PROJECT_NAME} PRIVATE
  Qt5::Core Qt5::Network Qt5::Gui Qt5::Widgets 
)
# Link the pure logic core layer
target_link_libraries(${PROJECT_NAME} PRIVATE MotionGloveUICore)

# Link the CarbonUI target provided by CPM to the project executable
target_link_libraries(${PROJECT_NAME} PRIVATE CarbonUI MotionGloveLLSDK)

if(WIN32)
  target_link_libraries(${PROJECT_NAME} PRIVATE GloveSteamDriver)
endif()

# ########################  tests  #########################
# 作为上级工程的子项目被引入时默认关闭，避免把 GoogleTest 下载进别人的构建。
if(NOT DEFINED MOTIONGLOVE_UI_BUILD_TESTS)
  if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    set(_h2_tests_default ON)
  else()
    set(_h2_tests_default OFF)
  endif()
  option(MOTIONGLOVE_UI_BUILD_TESTS
         "Build MotionGloveUI unit tests (GoogleTest + CTest)" ${_h2_tests_default})
  unset(_h2_tests_default)
endif()

if(MOTIONGLOVE_UI_BUILD_TESTS)
  enable_testing()
  add_subdirectory(tests)
endif()
