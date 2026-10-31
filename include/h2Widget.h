/**
 * @file  h2Widget.h
 * @brief 主界面控件 h2Widget 的公开接口。
 * @ingroup h2ui
 * @version 1.0.0
 *
 * @details
 * 含 Q_OBJECT，需 moc 生成元对象代码。
 * 函数级参数与返回值说明见各函数自身的注释块。
 */

#pragma once
#include "libh2ui_global.h"

#ifdef _WIN32
#include <windows.h>

#else

#endif
#include <sys/stat.h>
#include <QtWidgets/QWidget>
#include <QtCore/QTimer>
#include <QtCore/QProcess>
#include <QtCore/QThread>
#include <QtMultimedia/QMediaPlayer>
#include <QtMultimediaWidgets/QVideoWidget>
#include "h2sdk.h"
namespace Ui { class h2Widget; }

/**
 * @brief 主界面控件：把设备状态、手指/摇杆/手势等子面板组装成一个完整界面。
 *
 * @details
 * 本类是对外唯一的主入口控件，内部组合了手指角度调节、摇杆姿态、摇杆校准、
 * 手势显示、骨骼树、CSV 录制与 SteamVR 监控等子控件，并负责：
 *  - 按手套在线状态使能/停用相关面板（updateUIFromGloveOnlineStatus）；
 *  - 周期刷新设备状态（onH2MonitorTimer 拉取电量、固件版本、在线状态）；
 *  - 读写套装配置（readGloveConfigFileToUI / onGloveCaptureModeChanged）；
 *  - 中英文切换（setTabPageLanguage / retranslateUI）；
 *  - 可选地嵌入外部 Unity 进程窗口（m_unityExeName / unityProcess）。
 *
 * @note 所有接口都只能在 UI 线程调用。设备读写通过 MotionGloveLLSDK 同步完成。
 */
class LIBH2UI_API h2Widget : public QWidget
{
    Q_OBJECT

public:
    /**
     * @brief 构造主界面：创建全部子面板、启动监控定时器并回填当前配置。
     * @param[in] parent 父控件，**可为 nullptr**。
     */
    h2Widget(QWidget* parent = Q_NULLPTR);

    /** @brief 析构：停止定时器并释放子面板、Unity 进程与界面对象。 */
    ~h2Widget();

    /** @brief 读取当前演员名。@return 演员名，未设置时为空串。 */
    std::string getActorName() const { return m_actorName; }

    /**
     * @brief 设置演员名。
     * @param[in] val 演员名，UTF-8 编码；允许空串。
     */
    void setActorName(std::string val) { m_actorName = val; }

    /** @brief 读取当前套装序列号。@return 序列号原始值，0 表示未绑定设备。 */
    unsigned int getSn() const { return m_sn; }

    /**
     * @brief 设置当前套装序列号。
     * @param[in] val 序列号原始值，0 表示未绑定；超出有效设备号范围不会校验。
     */
    void setSn(unsigned int val) { m_sn = val; }
    
    /**
     * @brief 子面板语言切换：统一切换三个手指调节面板的文案。
     * @param[in] lang 语言代码，取值 `"en"` 或 `"zh"`；其他值按中文处理。
     */
    void setFingerWidgetsLanguage(const QString& lang);
    
    /**
     * @brief 页签语言切换：切换主界面页签与 CSV 录制面板的文案。
     * @param[in] lang 语言代码，取值 `"en"` 或 `"zh"`；同时记录到 m_language。
     * @post 调用后立即执行一次 retranslateUI()。
     */
    void setTabPageLanguage(const QString& lang);

    /**
     * @brief 读取「骨骼」页签透明度滑块的当前值。
     *
     * @details
     * 供宿主界面在创建 3D 视图后同步初始状态使用：滑块默认值来自 .ui，
     * 若宿主另有默认值，二者必须对齐，否则用户要拖动一次滑块才看得到预期画面。
     * 滑块缺失（例如精简构建裁掉了「骨骼」页签）时返回 100（完全不透明）。
     *
     * @return 透明度百分比，单位 ％，取值 [0,100]；100 表示完全不透明。
     */
    int skeletonOpacityPercent() const;

    /**
     * @brief 读取「骨骼」页签关节点滑块的当前值。
     *
     * @details 关节点（腕关节、各指节球、指尖红球）与骨骼段是两条独立的透明度通道，
     * 本接口只返回关节点通道的值。滑块缺失时返回 100（完全不透明）。
     *
     * @return 关节点透明度百分比，单位 ％，取值 [0,100]；100 表示完全不透明。
     */
    int jointOpacityPercent() const;

    /**
     * @brief 查询「骨骼」页签里「骨骼连线」（八面锥）开关是否勾选。
     *
     * @details 复选框缺失（精简构建裁掉该页）时返回 false，即默认不绘制，
     * 与 GLWidget 的默认状态一致，避免宿主界面在启动时凭空打开一层几何。
     *
     * @return true 表示用户要求绘制八面锥骨骼。
     */
    bool boneLinesVisible() const;

    /**
     * @brief 读取「骨骼」页签里骨骼连线透明度滑块的当前值。
     *
     * @return 不透明度百分比，单位 ％，取值 [0,100]；滑块缺失时返回 100。
     */
    int boneLineOpacityPercent() const;

signals:
    /**
     * @brief 骨骼模型透明度滑块数值变化。
     *
     * @param[in] percent 透明度百分比，单位 ％，取值 [0,100]；0 完全透明、100 完全不透明。
     *
     * @note 由「骨骼」页签的滑块发出，主界面据此刷新 3D 视图的骨骼透明度。
     */
    void skeletonOpacityChanged(int percent);

    /**
     * @brief 关节点透明度滑块数值变化。
     *
     * @param[in] percent 关节点透明度百分比，单位 ％，取值 [0,100]；
     *                    0 完全透明、100 完全不透明。
     *
     * @note 与 skeletonOpacityChanged 是两条独立通道：本信号只影响关节点
     *       （腕关节、指节球、指尖红球），不影响骨骼段。
     */
    void jointOpacityChanged(int percent);

    /**
     * @brief 「骨骼连线」（八面锥）开关状态变化。
     *
     * @param[in] visible true 表示需要在 3D 视图里绘制关节之间的八面锥骨架。
     *
     * @note 与骨骼透明度、关节点透明度都是独立通道：八面锥是第三种几何，
     *       默认关闭，只有用户显式勾选才绘制。
     */
    void boneLinesVisibleChanged(bool visible);

    /**
     * @brief 骨骼连线透明度滑块数值变化。
     *
     * @param[in] percent 不透明度百分比，单位 ％，取值 [0,100]。
     *
     * @note 只影响八面锥骨架，不影响骨骼模型与关节点球体。
     */
    void boneLineOpacityChanged(int percent);

public slots:

    /**
     * @brief 手套采集模式改变时保存到当前套装配置。
     * @param[in] index 模式索引，直接转换为 h2GloveMode_TypeDef；越界值不校验。
     * @note 序列号为 0（未绑定设备）时直接返回，不写配置。
     */
    void onGloveCaptureModeChanged(int index);

    /**
     * @brief 监控定时器回调：刷新设备在线状态、电量与固件版本等界面信息。
     * @note 由 h2MonitorTimer 周期触发，频率由 h2MonitorTimerHz 决定（默认 1 Hz）。
     */
    void onH2MonitorTimer();

    /**
     * @brief 一次性设置当前套装序列号与演员名，并同步给 CSV 录制面板。
     * @param[in] suitNumber3E 套装序列号原始值。
     * @param[in] actorName    演员名，UTF-8 编码。
     */
 void setSNAndAN(const unsigned int& suitNumber3E,
        const std::string& actorName);
    //end
private slots:
    /** @brief 「手势配置」按钮：弹出并刷新手势容器窗口。 */
    void on_PB_gestureConfig_clicked();
    /** @brief 「左手 Index 模拟调试」按钮：弹出左手 Index 仿真对话框。 */
    void on_PB_indexSimuDebug_LH_clicked();
    /** @brief 「右手 Index 模拟调试」按钮。@note 当前为空实现。 */
    void on_PB_indexSimuDebug_RH_clicked();

    /** @brief 「串口打开/关闭」按钮：按当前状态切换串口开关。 */
    void on_PB_uartOpen_clicked();
    /** @brief 「串口扫描」按钮：重新枚举可用串口并刷新下拉框。 */
    void on_PB_uartDetect_clicked();
    
    /** @brief 按 m_language 刷新主界面所有静态文案。 */
    void retranslateUI();

    /** @brief 骨骼透明度滑块数值变化：刷新数值文案并转发给主界面。 */
    void onSkeletonOpacitySliderMoved(int percent);

    /** @brief 关节点透明度滑块数值变化：刷新数值文案并转发给主界面。 */
    void onJointOpacitySliderMoved(int percent);

    /** @brief 骨骼连线复选框状态变化：转发给主界面。 */
    void onBoneLinesToggled(bool checked);

    /** @brief 骨骼连线透明度滑块数值变化：刷新数值文案并转发给主界面。 */
    void onBoneLineOpacitySliderMoved(int percent);

private:
    /** @brief uic 生成的界面对象，由析构函数负责 delete。 */
    Ui::h2Widget* ui = nullptr;
    /** @brief 左手手指角度调节（自由模式）面板。 */
    QWidget* WIG_fingerAdjustFreeMode_LH = 0;
    /** @brief 右手手指角度调节（自由模式）面板。 */
    QWidget* WIG_fingerAdjustFreeMode_RH = 0;
    /** @brief 左手手指角度调节（锁定偏航）面板。 */
    QWidget* WIG_fingerAdjustLockYaw_LH = 0;
    /** @brief 右手手指角度调节（锁定偏航）面板。 */
    QWidget* WIG_fingerAdjustLockYaw_RH = 0;
    /** @brief 左手手指角度调节（增强模式）面板。 */
    QWidget* WIG_fingerAdjustEnhanced_LH = 0;
    /** @brief 右手手指角度调节（增强模式）面板。 */
    QWidget* WIG_fingerAdjustEnhanced_RH = 0;

    /** @brief 左手摇杆姿态面板。 */
    QWidget* WIG_Joystick_LH = 0;
    /** @brief 右手摇杆姿态面板。 */
    QWidget* WIG_Joystick_RH = 0;

    /** @brief 左手摇杆校准面板。 */
    QWidget* WIG_jsCali_LH = 0;
    /** @brief 右手摇杆校准面板。 */
    QWidget* WIG_jsCali_RH = 0;

    /** @brief 左手手势显示面板。 */
    QWidget* WIG_h2GestureDisplay_LH = 0;
    /** @brief 右手手势显示面板。 */
    QWidget* WIG_h2GestureDisplay_RH = 0;

	/** @brief SteamVR 监控面板；仅在启用 SteamVR 监控时创建。 */
	QWidget* WIG_steamVRMonitor = 0;

	/** @brief Unity 宿主的窗口管理器对象。 */
	QObject* m_h2UnityWindowMgr = 0;

	/** @brief 骨骼树面板。 */
	QWidget* WIG_h2SkeletonTree = 0;
	/** @brief CSV 录制面板。 */
	QWidget* WIG_h2CsvRecord = 0;

    /** @brief 当前演员名，UTF-8 编码。 */
    std::string m_actorName;
    /** @brief 当前套装序列号；0 表示未绑定任何设备。 */
    unsigned int m_sn=0;
    /** @brief 当前手套采集模式，与 SDK 的 h2GloveMode_TypeDef 取值一致。 */
    h2GloveMode_TypeDef m_MocapGloveMode;

    /** @brief 周期性拉取设备状态的定时器。 */
    QTimer* h2MonitorTimer = nullptr;
    /** @brief 监控定时器的触发频率，单位 Hz，默认 1。 */
    unsigned int h2MonitorTimerHz = 1;

    /** @brief 需要嵌入的外部 Unity 可执行文件名（不含路径与扩展名）。 */
    std::string m_unityExeName = "foheart_hmax_u3d_port";

    /** @brief 是否显示 Index 模拟调试按钮。 */
    bool m_isIndexSimuDebug = false;

    /** @brief 左手摇杆校准面板当前可见性，用于避免重复的布局操作。 */
    bool m_leftJsCaliVisible = true;
    /** @brief 右手摇杆校准面板当前可见性。 */
    bool m_rightJsCaliVisible = true;
    /** @brief 左手摇杆面板当前可见性。 */
    bool m_leftJoystickVisible = true;
    /** @brief 右手摇杆面板当前可见性。 */
    bool m_rightJoystickVisible = true;
    /** @brief 手势调试弹出窗口，首次点击时创建并复用。 */
    QWidget* m_gestureContainer = 0;

    /** @brief 串口当前打开状态，0 表示未打开。 */
    unsigned int uartOpenStatus = 0;

    /** @brief 承载外部 Unity 进程的对象；为空表示未启动。 */
    QProcess* unityProcess = nullptr;
    /** @brief 从 Unity 进程截取到的窗口句柄包装。 */
    QWindow* m_unityPortWindow = nullptr;
    /** @brief 用于把 Unity 窗口嵌入本界面的容器控件。 */
    QWidget* m_unityPortWidget = nullptr;

    /** @brief 是否启用 SteamVR 监控面板。 */
    bool m_isSteamVRMonitorEnabled = false;
    
    /** @brief 当前界面语言，取值 `"en"` 或 `"zh"`，默认中文。 */
    QString m_language = "zh";
private:
    /** @brief 创建并布局全部子面板、应用 CarbonUI 样式、绑定信号槽。 */
    void initUI();
    /**
     * @brief 从配置文件读出设置并回填到界面控件。
     * @note 只读取、不写回；设备不可用时保持界面原状。
     */
    void readGloveConfigFileToUI(void);

    /** @brief 枚举系统串口并刷新串口下拉框。 */
    void updateUartPorts(void);

    /**
     * @brief 按左右手在线状态使能或停用相关面板。
     * @param[in] isLeftHandOnline  true 表示左手在线。
     * @param[in] isRightHandOnline true 表示右手在线。
     * @note 离线一侧的面板会被禁用，避免用户对不存在的设备下发设置。
     */
    void  updateUIFromGloveOnlineStatus(bool isLeftHandOnline, bool isRightHandOnline);
    /**
     * @brief 取本模块（DLL/EXE）所在目录，用于定位 Resources 下的图片。
     * @return 目录绝对路径，结尾不带分隔符；非 Windows 平台返回空串。
     */
    std::string GetLocalDllDirectory();

    /**
     * @brief 显示或隐藏左手摇杆校准面板。
     * @param[in] isVisible true 表示显示。
     * @note 与 m_leftJsCaliVisible 比较，状态未变化时不做任何布局操作。
     */
    void setLeftJoystickCaliWidgetVisible(bool isVisible);
    /** @brief 显示或隐藏右手摇杆校准面板。@param[in] isVisible true 表示显示。 */
    void setRightJoystickCaliWidgetVisible(bool isVisible);

    /** @brief 显示或隐藏左手摇杆姿态面板。@param[in] isVisible true 表示显示。 */
    void setLeftJoystickWidgetVisible(bool isVisible);
    /** @brief 显示或隐藏右手摇杆姿态面板。@param[in] isVisible true 表示显示。 */
    void setRightJoystickWidgetVisible(bool isVisible);

    /**
     * @brief 用滑块当前值刷新骨骼透明度的数值文案。
     * @note 滑块或文案控件缺失时直接返回，不抛异常。
     */
    void refreshSkeletonOpacityValueText();

    /**
     * @brief 用滑块当前值刷新关节点透明度的数值文案。
     * @note 滑块或文案控件缺失时直接返回，不抛异常。
     */
    void refreshJointOpacityValueText();

    /** @brief 刷新「骨骼连线」透明度数值文案（"65%" 之类）。 */
    void refreshBoneLineOpacityValueText();

    /**
     * @brief 给「骨骼」页签应用深色样式：白色字体 + 与其他页统一的滚动条宽度。
     *
     * @details 字体全白是因为该页的树控件本身就是 #3b3b3b 深底，默认灰字对比度不足；
     *          滚动条宽度统一成 kScrollBarWidth，避免「设置」页的 QScrollArea 与
     *          「骨骼」页的树各自一套粗细。分组标题（QGroupBox::title）也一并置白。
     */
    void applySkeletonTabStyle();

  
};

