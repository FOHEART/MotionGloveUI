/**
 * @file  ui_test_support.cpp
 * @brief 实现截图归档与结果汇总。
 * @ingroup h2tests
 *
 * @details 函数语义见 ui_test_support.h。
 *
 * 两个已踩过的坑写在这里备查：
 *  - QTextStream 插入窄字符串字面量时不会按 setCodec("UTF-8") 解释，
 *    含中文的片段会被当 Latin-1 再编码一次（二次编码）。因此本文件里
 *    所有含中文的文本一律用 QStringLiteral 或 QString::fromUtf8。
 *  - 失败截图只能写在 cleanup()：QVERIFY 失败会立刻 return，
 *    测试体内写的截图代码根本执行不到。
 */

#include "ui_test_support.h"

#include <QCoreApplication>
#include <QDir>
#include <QEvent>
#include <QFile>
#include <QFileInfo>
#include <QPixmap>
#include <QTextStream>
#include <QWidget>

#include <QtTest/QtTest>

ScreenshotArchive::ScreenshotArchive(QString caseName)
    : m_caseName(std::move(caseName))
{
}

void ScreenshotArchive::initTestCase()
{
    const QString baseDir = qEnvironmentVariable("UI_TEST_OUTPUT");
    const QString root = baseDir.isEmpty() ? QCoreApplication::applicationDirPath() + QStringLiteral("/ui-test-output")
                                           : baseDir;
    // 每个用例类一个子目录：同一次运行里多个类共用目录会互相清空对方的截图与汇总
    m_outputDir = root + QLatin1Char('/') + m_caseName;

    QDir dir(m_outputDir);
    if (!dir.exists())
    {
        dir.mkpath(QStringLiteral("."));
    }

    // 清场：不依赖上一次运行留下的状态
    const QStringList stale = dir.entryList(
        QStringList() << QStringLiteral("*.png") << QStringLiteral("index.html")
                      << QStringLiteral("summary.txt"),
        QDir::Files);
    for (const QString& name : stale)
    {
        dir.remove(name);
    }

    m_seq = 0;
    m_entries.clear();
}

QString ScreenshotArchive::nextFileName(const QString& step, const QString& suffix) const
{
    return QStringLiteral("%1-%2%3.png")
        .arg(m_seq, 2, 10, QLatin1Char('0'))
        .arg(step, suffix);
}

QString ScreenshotArchive::savePixmap(const QString& fileName, QWidget* widget)
{
    if (!widget)
    {
        return QString();
    }

    const QPixmap shot = widget->grab();
    if (shot.isNull())
    {
        return QString();
    }

    const QString path = QDir(m_outputDir).filePath(fileName);
    return shot.save(path, "PNG") ? fileName : QString();
}

QString ScreenshotArchive::capture(const QString& step, QWidget* window, QWidget* panel)
{
    // 先把排队的布局/绘制请求处理掉：刚加入的子控件此时还没有有效几何，
    // 直接 grab() 只会抓到一片纯色背景，截图为空但用例却是绿的。
    QCoreApplication::sendPostedEvents(nullptr, QEvent::LayoutRequest);
    QCoreApplication::processEvents();

    Entry entry;
    entry.label = step;
    entry.png = savePixmap(nextFileName(step, QString()), window);
    if (panel)
    {
        entry.panelPng = savePixmap(nextFileName(step, QStringLiteral("-panel")), panel);
    }

    m_entries.append(entry);
    ++m_seq;
    return entry.png.isEmpty() ? QString() : QDir(m_outputDir).filePath(entry.png);
}

void ScreenshotArchive::captureOnFailure(QWidget* window)
{
    const QString testName = QString::fromLatin1(QTest::currentTestFunction());
    const QString dataTag = QString::fromLatin1(QTest::currentDataTag() ? QTest::currentDataTag() : "");
    QString label = dataTag.isEmpty() ? testName : QStringLiteral("%1(%2)").arg(testName, dataTag);

    // 把测试体内刚登记的那条记录也标成失败，否则图库仍显示 PASS，与汇总数字打架
    if (!m_entries.isEmpty())
    {
        m_entries.last().failed = true;
    }

    Entry entry;
    entry.label = QStringLiteral("失败现场 - %1").arg(label);
    entry.failed = true;
    if (window)
    {
        entry.png = savePixmap(QStringLiteral("failed-%1.png").arg(testName), window);
    }
    m_entries.append(entry);
}

int ScreenshotArchive::failedCount() const
{
    int count = 0;
    for (const Entry& entry : m_entries)
    {
        if (entry.failed)
        {
            ++count;
        }
    }
    return count;
}

void ScreenshotArchive::finalize()
{
    const int failed = failedCount();
    const bool passed = (failed == 0);

    // ── 图库 ──────────────────────────────────────────────────────────────
    QString html;
    html += QStringLiteral("<!DOCTYPE html>\n<html lang=\"zh-CN\">\n<head>\n");
    html += QStringLiteral("<meta charset=\"utf-8\">\n");
    html += QStringLiteral("<title>%1 界面测试结果</title>\n").arg(m_caseName);
    html += QStringLiteral(
        "<style>\n"
        "body{background:#20232a;color:#e6e6e6;font-family:'Segoe UI',sans-serif;margin:24px}\n"
        "h1{font-size:20px;margin:0 0 4px}\n"
        ".meta{color:#9aa0a6;margin-bottom:20px}\n"
        ".grid{display:flex;flex-wrap:wrap;gap:16px}\n"
        ".card{background:#2b2e3d;border-radius:10px;padding:10px;width:340px}\n"
        ".card img{width:100%;border-radius:6px;background:#111}\n"
        ".label{font-size:13px;margin:8px 0 4px;word-break:break-all}\n"
        ".badge{display:inline-block;font-size:11px;padding:2px 8px;border-radius:8px}\n"
        ".pass{background:#1f6f3f;color:#d8ffe6}\n"
        ".fail{background:#7a1f1f;color:#ffdada}\n"
        "</style>\n</head>\n<body>\n");
    html += QStringLiteral("<h1>%1 界面测试结果</h1>\n").arg(m_caseName);
    html += QStringLiteral("<div class=\"meta\">用例数 %1 · 截图 %2 · 失败 %3 · 结果 %4</div>\n")
                .arg(m_entries.size())
                .arg(screenshotCount())
                .arg(failed)
                .arg(passed ? QStringLiteral("通过 / PASS") : QStringLiteral("失败 / FAIL"));
    html += QStringLiteral("<div class=\"grid\">\n");

    for (const Entry& entry : m_entries)
    {
        html += QStringLiteral("<div class=\"card\">\n");
        if (!entry.png.isEmpty())
        {
            html += QStringLiteral("<a href=\"%1\"><img src=\"%1\" alt=\"%2\"></a>\n")
                        .arg(entry.png, entry.label);
        }
        html += QStringLiteral("<div class=\"label\">%1 <span class=\"badge %2\">%3</span></div>\n")
                    .arg(entry.label,
                         entry.failed ? QStringLiteral("fail") : QStringLiteral("pass"),
                         entry.failed ? QStringLiteral("FAIL") : QStringLiteral("PASS"));
        if (!entry.panelPng.isEmpty())
        {
            html += QStringLiteral("<a href=\"%1\"><img src=\"%1\" alt=\"%2 局部\"></a>\n")
                        .arg(entry.panelPng, entry.label);
        }
        html += QStringLiteral("</div>\n");
    }

    html += QStringLiteral("</div>\n</body>\n</html>\n");

    QFile htmlFile(QDir(m_outputDir).filePath(QStringLiteral("index.html")));
    if (htmlFile.open(QIODevice::WriteOnly | QIODevice::Truncate))
    {
        QTextStream out(&htmlFile);
        out.setCodec("UTF-8");
        out << html;
    }

    // ── 机读汇总 ──────────────────────────────────────────────────────────
    QFile summaryFile(QDir(m_outputDir).filePath(QStringLiteral("summary.txt")));
    if (summaryFile.open(QIODevice::WriteOnly | QIODevice::Truncate))
    {
        QTextStream out(&summaryFile);
        out.setCodec("UTF-8");
        out << QStringLiteral("case=") << m_caseName << '\n';
        out << QStringLiteral("steps=") << m_entries.size() << '\n';
        out << QStringLiteral("screenshots=") << screenshotCount() << '\n';
        out << QStringLiteral("failed=") << failed << '\n';
        out << QStringLiteral("result=") << (passed ? QStringLiteral("pass") : QStringLiteral("fail")) << '\n';
    }
}
