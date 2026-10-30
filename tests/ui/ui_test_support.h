/**
 * @file  ui_test_support.h
 * @brief QtTest 界面用例的截图归档与结果汇总工具。
 * @ingroup h2tests
 * @version 1.0.0
 *
 * @details
 * 按团队约定，界面用例除了断言之外还要留下可回看的证据：
 *  - `NN-<步骤>.png`：整窗截图，序号前缀保证顺序；
 *  - `NN-<步骤>-panel.png`：同一时刻的局部控件截图；
 *  - `failed-<用例>.png`：仅失败时生成，记录失败现场；
 *  - `index.html`：图库，卡片带 PASS / FAIL 徽标；
 *  - `summary.txt`：机读汇总，脚本判据用 `result=pass` 与 `failed=0`。
 *
 * 输出目录优先取环境变量 `UI_TEST_OUTPUT`（ctest 注入），
 * 缺省回落到测试 exe 同级的 `ui-test-output`，两种入口都能直接跑。
 *
 * 只在 Qt 事件循环可用的前提下使用；不做线程同步。
 */
#pragma once

#include <QString>
#include <QVector>

class QWidget;

/**
 * @brief 单个用例类的截图归档器。
 *
 * @details 生命周期约定：
 *  - initTestCase() 清空上一次的 png / index.html / summary.txt，防止新旧结果混杂；
 *  - 每个步骤调用 capture() 记录一张图；
 *  - cleanup() 里若 `QTest::currentTestFailed()` 为真则调用 captureOnFailure()；
 *  - cleanupTestCase() 里调用 finalize() 写出图库与汇总。
 */
class ScreenshotArchive
{
public:
    /**
     * @brief 构造归档器。
     * @param[in] caseName 用例类名，写入 summary.txt 与页面标题，不可为空串。
     */
    explicit ScreenshotArchive(QString caseName);

    /**
     * @brief 解析输出目录并清空上一次的产物。
     * @post 目录存在；历史 png / index.html / summary.txt 已被删除。
     * @note 必须在 initTestCase() 中调用，且早于任何 capture()。
     */
    void initTestCase();

    /**
     * @brief 截图并登记一条记录。
     * @param[in] step   步骤名，用作文件名主体，**不要包含路径分隔符**。
     * @param[in] window 整窗控件，**不可为 nullptr**；用 QWidget::grab() 取图。
     * @param[in] panel  可选局部控件，**可为 nullptr**；非空时额外保存 `-panel.png`。
     * @return 保存的整窗图片路径；grab() 失败时返回空串。
     */
    QString capture(const QString& step, QWidget* window, QWidget* panel = nullptr);

    /**
     * @brief 失败现场截图：生成 `failed-<用例>.png`，并把上一条记录标记为失败。
     * @param[in] window 整窗控件，**可为 nullptr**；为空时只做标记不落图。
     * @note 只能在 cleanup() 中调用：QVERIFY 失败会立刻 return，
     *       测试体内写的截图代码根本执行不到。
     */
    void captureOnFailure(QWidget* window);

    /**
     * @brief 写出 index.html 与 summary.txt。
     * @post `summary.txt` 中 `result=pass` 当且仅当失败数为 0。
     */
    void finalize();

    /** @brief 输出目录绝对路径。@return 目录路径。 */
    QString outputDir() const { return m_outputDir; }

    /** @brief 已登记的截图条目数（含失败现场图）。@return 条目数，取值 [0, INT_MAX]。 */
    int screenshotCount() const { return m_entries.size(); }

    /** @brief 被标记为失败的条目数。@return 失败数，取值 [0, screenshotCount()]。 */
    int failedCount() const;

private:
    /** @brief 单条截图记录。 */
    struct Entry
    {
        QString label;       ///< 图库卡片上显示的步骤名。
        QString png;         ///< 整窗图片文件名；空表示 grab 失败。
        QString panelPng;    ///< 局部控件图片文件名；空表示未提供。
        bool failed = false; ///< 该步骤是否被标记为失败。
    };

    /**
     * @brief 生成带两位序号前缀的文件名。
     * @param[in] step   步骤名。
     * @param[in] suffix 追加在名字后的后缀，可为空串。
     * @return 形如 `03-foo-panel.png` 的文件名。
     */
    QString nextFileName(const QString& step, const QString& suffix) const;

    /**
     * @brief 抓取并保存控件位图。
     * @param[in] fileName 目标文件名（不含目录）。
     * @param[in] widget   待抓取控件，**可为 nullptr**。
     * @return 成功返回文件名，失败返回空串。
     */
    QString savePixmap(const QString& fileName, QWidget* widget);

    QString m_caseName;       ///< 用例类名。
    QString m_outputDir;      ///< 解析后的输出目录。
    int m_seq = 0;            ///< 下一个序号。
    QVector<Entry> m_entries; ///< 已登记记录。
};
