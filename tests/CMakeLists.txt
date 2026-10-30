# 单元测试构建脚本：GoogleTest + CTest。
#
# 分层依据见《TDD 与 GoogleTest 实践指南》：
# 本源只覆盖纯逻辑层（src/core，仅依赖 Qt5::Core），
# 需要 QWidget 事件分发的界面用例另建 QtTest 目标。

include(GoogleTest)

# ── GoogleTest 获取 ────────────────────────────────────────────────────────
option(MOTIONGLOVE_UI_USE_SYSTEM_GTEST "使用已安装的 GoogleTest，而不是 FetchContent 下载" OFF)
set(H2_GTEST_REPOSITORY "https://github.com/google/googletest.git" CACHE STRING "GoogleTest 仓库地址")
set(H2_GTEST_TAG "v1.14.0" CACHE STRING "GoogleTest 版本")

if(MOTIONGLOVE_UI_USE_SYSTEM_GTEST)
  find_package(GTest 1.11 REQUIRED)
  set(H2_GTEST_TARGET GTest::gtest)
  set(H2_GTEST_MAIN_TARGET GTest::gtest_main)
else()
  # MSVC 下 GoogleTest 默认 /MT，与 Qt 的 /MD 混用会导致 CRT 冲突（LNK4098 / 堆交叉释放）。
  set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
  set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
  set(BUILD_GMOCK OFF CACHE BOOL "" FORCE)

  FetchContent_Declare(googletest
    GIT_REPOSITORY "${H2_GTEST_REPOSITORY}"
    GIT_TAG        "${H2_GTEST_TAG}"
    GIT_SHALLOW    TRUE
    GIT_PROGRESS   TRUE)
  FetchContent_MakeAvailable(googletest)
  set(H2_GTEST_TARGET gtest)
  set(H2_GTEST_MAIN_TARGET gtest_main)
endif()

# ── 用例目标 ──────────────────────────────────────────────────────────────
add_executable(h2ui_unit_tests
  unit/h2NetAddress_test.cpp
  unit/h2RatioConversion_test.cpp
  unit/h2TextWrap_test.cpp
)

target_link_libraries(h2ui_unit_tests PRIVATE
  MotionGloveUICore
  ${H2_GTEST_TARGET}
  ${H2_GTEST_MAIN_TARGET}
)

set_target_properties(h2ui_unit_tests PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/bin"
  RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_CURRENT_BINARY_DIR}/bin"
  RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_CURRENT_BINARY_DIR}/bin"
)

if(MSVC)
  # 与被测目标保持同一套编译选项：中文注释需 /utf-8，否则 MSVC 按 GBK 解析
  target_compile_options(h2ui_unit_tests PRIVATE /utf-8 /MP /Zc:wchar_t)
else()
  target_compile_options(h2ui_unit_tests PRIVATE -finput-charset=UTF-8 -fexec-charset=UTF-8)
endif()

# ── Qt5Core.dll 运行期定位 ────────────────────────────────────────────────
# 测试 exe 动态链接 Qt，构建机 PATH 里通常没有 Qt 的 bin 目录。
get_target_property(_h2_qt_core_dll Qt5::Core IMPORTED_LOCATION_DEBUG)
if(NOT _h2_qt_core_dll)
  get_target_property(_h2_qt_core_dll Qt5::Core IMPORTED_LOCATION_RELEASE)
endif()
if(_h2_qt_core_dll)
  get_filename_component(H2_QT_BIN_DIR "${_h2_qt_core_dll}" DIRECTORY)
else()
  get_filename_component(H2_QT_BIN_DIR "${Qt5_DIR}/../../../bin" ABSOLUTE)
endif()

# 把 Qt5Core 运行时复制到测试 exe 同级。
# 用例发现（gtest_discover_tests）会在 ctest 阶段直接执行被测 exe 列出用例，
# 那条调用链不经过测试属性里的环境变量，因此必须保证 DLL 就在 exe 旁边。
if(WIN32 AND _h2_qt_core_dll)
  add_custom_command(TARGET h2ui_unit_tests POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${_h2_qt_core_dll}" "$<TARGET_FILE_DIR:h2ui_unit_tests>"
    COMMENT "Copying Qt5Core runtime next to h2ui_unit_tests")
endif()

set(_h2_test_properties)
if(CMAKE_VERSION VERSION_GREATER_EQUAL "3.22")
  # path_list_prepend 由 ctest 处理，避免手写分号转义
  list(APPEND _h2_test_properties ENVIRONMENT_MODIFICATION
       "PATH=path_list_prepend:${H2_QT_BIN_DIR}")
else()
  list(APPEND _h2_test_properties ENVIRONMENT "PATH=${H2_QT_BIN_DIR}\\;$ENV{PATH}")
endif()

# DISCOVERY_MODE PRE_TEST：用例清单在 ctest 时才询问被测 exe，
# 构建阶段不要求 Qt DLL 已在 PATH 上。
gtest_discover_tests(h2ui_unit_tests
  TEST_PREFIX "Unit."
  PROPERTIES ${_h2_test_properties} TIMEOUT 120
  DISCOVERY_MODE PRE_TEST
)

# ── 界面层（QtTest）：控件点击 + 截图归档 ────────────────────────────────
add_subdirectory(ui)
