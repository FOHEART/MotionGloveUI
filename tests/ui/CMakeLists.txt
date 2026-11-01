# QtTest 界面用例：控件点击 + 截图归档。
#
# 分层依据见《TDD 与 GoogleTest 实践指南》第八章：
#  - 需要 QWidget 事件分发/布局 → QtTest，而不是 GoogleTest；
#  - 用例逐个点击控件并归档整窗截图，失败时在 cleanup() 补拍现场；
#  - 输出 index.html 图库 + summary.txt 机读汇总。

qt5_wrap_cpp(UI_TEST_MOC_SOURCES
  h2GloveDeviceTreeWidget_ui_test.h
  h2GloveDeviceTreeContainerWidget_ui_test.h
  h2Widget_skeletonOpacity_ui_test.h
  h2Widget_jointOpacity_ui_test.h
  h2Widget_mocapModePanel_ui_test.h
)

add_executable(h2ui_ui_tests
  ui_main.cpp
  ui_test_support.cpp
  h2GloveDeviceTreeWidget_ui_test.cpp
  h2GloveDeviceTreeContainerWidget_ui_test.cpp
  h2Widget_skeletonOpacity_ui_test.cpp
  h2Widget_jointOpacity_ui_test.cpp
  h2Widget_mocapModePanel_ui_test.cpp
  ${UI_TEST_MOC_SOURCES}
)

target_include_directories(h2ui_ui_tests PRIVATE
  "${CMAKE_CURRENT_SOURCE_DIR}"
  "${PROJECT_SOURCE_DIR}/include"
)

# 被测控件由 MotionGloveUI 导出；QtTest 提供 QTest 断言与事件注入
# h2Widget.h 公开包含 h2sdk.h，因此编译期还需要 MotionGloveLLSDK 的头文件目录
target_link_libraries(h2ui_ui_tests PRIVATE
  MotionGloveUI
  MotionGloveLLSDK
  Qt5::Widgets
  Qt5::Test
)

set_target_properties(h2ui_ui_tests PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/bin"
  RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_CURRENT_BINARY_DIR}/bin"
  RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_CURRENT_BINARY_DIR}/bin"
)

if(MSVC)
  # 与被测目标同一套编译选项，否则中文注释在 MSVC 下会按 GBK 解析
  target_compile_options(h2ui_ui_tests PRIVATE /utf-8 /MP /Zc:wchar_t)
else()
  target_compile_options(h2ui_ui_tests PRIVATE -finput-charset=UTF-8 -fexec-charset=UTF-8)
endif()

# ── 运行期依赖部署 ────────────────────────────────────────────────────────
# 1) Qt 运行时与平台插件（含 offscreen）：windeployqt 会把 Qt5Core/Gui/Widgets/Test
#    以及 plugins/platforms 一起放到 exe 同级，无需依赖 PATH。
if(NOT DEFINED QT5_BIN_DIR)
  set(QT5_BIN_DIR "${H2_QT_BIN_DIR}")
endif()
run_windeployqt(h2ui_ui_tests)

# windeployqt 按直接导入推断依赖，会漏掉间接使用的 Qt5Network（由 MotionGloveUI.dll 引入），
# 并且默认只部署 qwindows 平台插件；离屏用例需要 qoffscreen。这里显式补齐。
file(GLOB H2_OFFSCREEN_PLUGINS "${H2_QT_BIN_DIR}/../plugins/platforms/qoffscreen*.dll")

add_custom_command(TARGET h2ui_ui_tests POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:h2ui_ui_tests>/platforms"
  COMMAND ${CMAKE_COMMAND} -E copy_if_different
          "$<TARGET_FILE:Qt5::Network>" "$<TARGET_FILE_DIR:h2ui_ui_tests>"
  COMMENT "Copying Qt5Network next to h2ui_ui_tests")

foreach(_plugin IN LISTS H2_OFFSCREEN_PLUGINS)
  add_custom_command(TARGET h2ui_ui_tests POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${_plugin}" "$<TARGET_FILE_DIR:h2ui_ui_tests>/platforms"
    COMMENT "Copying offscreen platform plugin: ${_plugin}")
endforeach()

# 2) 被测库与其依赖的 SDK / 驱动 DLL（target 文件路径由构建系统给出）
#
# 用独立的 always-run 目标而不是 h2ui_ui_tests 的 POST_BUILD：
# 只有 MotionGloveUI.dll 变化、测试源码没变时，测试 exe 不会重新链接，
# POST_BUILD 就不会执行，测试会静默加载上一次的旧 DLL（看着全绿，其实测的是旧代码）。
#
# 目标目录写成变量而不是 $<TARGET_FILE_DIR:h2ui_ui_tests>：后者会让本目标反向依赖
# 测试 exe，与 add_dependencies 构成循环。
set(H2_UI_TEST_BIN_DIR "${CMAKE_CURRENT_BINARY_DIR}/bin")

add_custom_target(h2ui_ui_test_runtime ALL
  COMMAND ${CMAKE_COMMAND} -E make_directory "${H2_UI_TEST_BIN_DIR}"
  COMMAND ${CMAKE_COMMAND} -E copy_if_different
          "$<TARGET_FILE:MotionGloveUI>" "${H2_UI_TEST_BIN_DIR}"
  COMMAND ${CMAKE_COMMAND} -E copy_if_different
          "$<TARGET_FILE:MotionGloveLLSDK>" "${H2_UI_TEST_BIN_DIR}"
  COMMENT "Copying MotionGloveUI / MotionGloveLLSDK next to h2ui_ui_tests"
  VERBATIM)
add_dependencies(h2ui_ui_tests h2ui_ui_test_runtime)

if(WIN32 AND TARGET GloveSteamDriver)
  add_custom_command(TARGET h2ui_ui_test_runtime POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "$<TARGET_FILE:GloveSteamDriver>" "${H2_UI_TEST_BIN_DIR}"
    COMMENT "Copying GloveSteamDriver next to h2ui_ui_tests")
endif()

# ── ctest 注册 ────────────────────────────────────────────────────────────
# 一个测试类一条 ctest 记录，失败时能直接定位到类；
# ENVIRONMENT 注入离屏平台与截图输出目录，RESOURCE_LOCK 串行化共用桌面资源的 GUI 用例。
set(H2_UI_TEST_OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/ui-test-output")

add_test(NAME Ui.DeviceTreeWidget COMMAND h2ui_ui_tests widget)
add_test(NAME Ui.DeviceTreeContainer COMMAND h2ui_ui_tests container)
add_test(NAME Ui.SkeletonOpacity COMMAND h2ui_ui_tests opacity)
add_test(NAME Ui.JointOpacity COMMAND h2ui_ui_tests joint)
add_test(NAME Ui.MocapModePanel COMMAND h2ui_ui_tests mocapmode)

set_tests_properties(
  Ui.DeviceTreeWidget
  Ui.DeviceTreeContainer
  Ui.SkeletonOpacity
  Ui.JointOpacity
  Ui.MocapModePanel
  PROPERTIES
    ENVIRONMENT "QT_QPA_PLATFORM=offscreen;QT_QPA_FONTDIR=C:/Windows/Fonts;UI_TEST_OUTPUT=${H2_UI_TEST_OUTPUT}"
    RESOURCE_LOCK gui_desktop
    TIMEOUT 180
)
