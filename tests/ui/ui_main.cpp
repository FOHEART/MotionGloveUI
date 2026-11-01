/**
 * @file  ui_main.cpp
 * @brief QtTest 界面测试的自定义入口。
 * @ingroup h2tests
 *
 * @details
 * 为什么不用 `QTEST_MAIN`：
 *  - 需要在构造 QApplication 之前调用 `QStandardPaths::setTestModeEnabled(true)`，
 *    把配置目录指向测试沙箱，避免读到真实用户配置导致用例时红时绿；
 *  - 需要给无头环境兜底 `QT_QPA_PLATFORM=offscreen`（ctest 也会设，直接运行时可兜底）；
 *  - 需要在一次进程里跑多个测试类，并按类名拆分日志文件：
 *    共用同一个 `-o` 文件会被后跑的类截断，出现「exit=1 却看不到谁挂」。
 *
 * 用法：
 *  - `h2ui_ui_tests`            跑全部测试类；
 *  - `h2ui_ui_tests widget`     只跑 h2GloveDeviceTreeWidgetUiTest；
 *  - `h2ui_ui_tests container`  只跑 h2GloveDeviceTreeContainerWidgetUiTest；
 *  - `h2ui_ui_tests opacity`    只跑 h2WidgetSkeletonOpacityUiTest；
 *  - `h2ui_ui_tests joint`      只跑 h2WidgetJointOpacityUiTest；
 *  - `h2ui_ui_tests mocapmode`  只跑 h2WidgetMocapModePanelUiTest；
 *  - `h2ui_ui_tests snapshot`   只跑 h2GloveConfigSnapshotUiTest；
 *  - `h2ui_ui_tests streamformat` 只跑 h2StreamConfigWidgetUiTest。
 */

#include "h2GloveDeviceTreeContainerWidget_ui_test.h"
#include "h2GloveDeviceTreeWidget_ui_test.h"
#include "h2GloveConfigSnapshot_ui_test.h"
#include "h2StreamConfigWidget_ui_test.h"
#include "h2Widget_jointOpacity_ui_test.h"
#include "h2Widget_mocapModePanel_ui_test.h"
#include "h2Widget_skeletonOpacity_ui_test.h"

#include <QApplication>
#include <QDir>
#include <QStandardPaths>
#include <QStringList>

#include <QtTest/QtTest>

namespace
{

/**
 * @brief 运行单个测试类并按类名拆分日志。
 * @param[in] testCase  测试类实例，**不可为 nullptr**。
 * @param[in] className 类名，用于日志文件名与失败提示。
 * @param[in] baseArgs  透传给 QtTest 的参数（已去掉类名选择参数）。
 * @return 失败用例数；0 表示全绿。
 */
int runClass(QObject* testCase, const QString& className, const QStringList& baseArgs)
{
    const QString envDir = qEnvironmentVariable("UI_TEST_OUTPUT");
    const QString outDir = envDir.isEmpty()
                               ? QCoreApplication::applicationDirPath() + QStringLiteral("/ui-test-output")
                               : envDir;
    QDir().mkpath(outDir);

    QStringList args = baseArgs;
    args << QStringLiteral("-o")
         << QDir(outDir).filePath(QStringLiteral("results.%1.txt").arg(className)) + QStringLiteral(",txt");

    return QTest::qExec(testCase, args);
}

} // namespace

/**
 * @brief 进程入口：设置测试环境、按类名分发并聚合退出码。
 * @param[in] argc 命令行参数个数。
 * @param[in] argv 命令行参数数组，**不可为 nullptr**；
 *                 第二个位置参数可选，取值 `widget` 或 `container`，用于只跑单个测试类。
 * @return 0 表示所有用例通过；非 0 表示至少一条用例失败。
 * @note 必须先关掉与真实用户配置的耦合，再构造 QApplication，见函数体注释。
 */
int main(int argc, char** argv)
{
    // 与真实用户配置隔离（见指南 9.10）
    QStandardPaths::setTestModeEnabled(true);

    // 直接运行时也要能离屏跑；ctest 会显式设置同一个变量
    if (qEnvironmentVariableIsEmpty("QT_QPA_PLATFORM"))
    {
        qputenv("QT_QPA_PLATFORM", "offscreen");
    }

    // 离屏插件用 freetype 字体库，未指定字体目录时会刷屏警告且拿不到字形
    if (qEnvironmentVariableIsEmpty("QT_QPA_FONTDIR"))
    {
        qputenv("QT_QPA_FONTDIR", "C:/Windows/Fonts");
    }

    QStringList args;
    for (int i = 0; i < argc; ++i)
    {
        args << QString::fromLocal8Bit(argv[i]);
    }

    // 解析可选的类名选择参数（唯一位置参数）
    QString selectedClass;
    QStringList filteredArgs;
    filteredArgs << args.value(0);
    for (int i = 1; i < args.size(); ++i)
    {
        const QString arg = args.at(i);
        if (!arg.startsWith(QLatin1Char('-')) && selectedClass.isEmpty())
        {
            selectedClass = arg;
            continue;
        }
        filteredArgs << arg;
    }

    QApplication app(argc, argv);

    int failed = 0;
    if (selectedClass.isEmpty() || selectedClass == QStringLiteral("widget"))
    {
        h2GloveDeviceTreeWidgetUiTest testCase;
        failed += runClass(&testCase, QStringLiteral("h2GloveDeviceTreeWidgetUiTest"), filteredArgs);
    }
    if (selectedClass.isEmpty() || selectedClass == QStringLiteral("container"))
    {
        h2GloveDeviceTreeContainerWidgetUiTest testCase;
        failed += runClass(&testCase, QStringLiteral("h2GloveDeviceTreeContainerWidgetUiTest"), filteredArgs);
    }
    if (selectedClass.isEmpty() || selectedClass == QStringLiteral("opacity"))
    {
        h2WidgetSkeletonOpacityUiTest testCase;
        failed += runClass(&testCase, QStringLiteral("h2WidgetSkeletonOpacityUiTest"),
                          filteredArgs);
    }
    if (selectedClass.isEmpty() || selectedClass == QStringLiteral("snapshot"))
    {
        h2GloveConfigSnapshotUiTest testCase;
        failed += runClass(&testCase, QStringLiteral("h2GloveConfigSnapshotUiTest"),
                          filteredArgs);
    }
    if (selectedClass.isEmpty() || selectedClass == QStringLiteral("mocapmode"))
    {
        h2WidgetMocapModePanelUiTest testCase;
        failed += runClass(&testCase, QStringLiteral("h2WidgetMocapModePanelUiTest"),
                          filteredArgs);
    }
    if (selectedClass.isEmpty() || selectedClass == QStringLiteral("joint"))
    {
        h2WidgetJointOpacityUiTest testCase;
        failed += runClass(&testCase, QStringLiteral("h2WidgetJointOpacityUiTest"),
                          filteredArgs);
    }
    if (selectedClass.isEmpty() || selectedClass == QStringLiteral("streamformat"))
    {
        h2StreamConfigWidgetUiTest testCase;
        failed += runClass(&testCase, QStringLiteral("h2StreamConfigWidgetUiTest"),
                          filteredArgs);
    }

    return failed == 0 ? 0 : 1;
}
