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

class LIBH2UI_API h2Widget : public QWidget
{
    Q_OBJECT

public:
    h2Widget(QWidget* parent = Q_NULLPTR);
    ~h2Widget();
    std::string getActorName() const { return m_actorName; }
    void setActorName(std::string val) { m_actorName = val; }
    unsigned int getSn() const { return m_sn; }
    void setSn(unsigned int val) { m_sn = val; }

   


public slots:

    /*手指的相关参数*/
    void onGloveCaptureModeChanged(int index);


    void onH2MonitorTimer();


 void setSNAndAN(const unsigned int& suitNumber3E,
        const std::string& actorName);
    //end
private slots:
    void on_PB_gestureConfig_clicked();
    void on_PB_indexSimuDebug_LH_clicked();
    void on_PB_indexSimuDebug_RH_clicked();

    /*
    Unity高清渲染窗口
    */
    void on_PB_openUnityPort_clicked();
    void on_PB_closeUnityPort_clicked();

    /*
    Unity高清渲染窗口窗口配置
    */
    void on_PB_unityWindowConfig_clicked();

    //uart
    void on_PB_uartOpen_clicked();
    void on_PB_uartDetect_clicked();

    void launchUnityApp(std::string s);


    void embedUnityWindow();
private:
    Ui::h2Widget* ui = nullptr;
    QWidget* WIG_fingerAdjustFreeMode_LH = 0;
    QWidget* WIG_fingerAdjustFreeMode_RH = 0;
    QWidget* WIG_fingerAdjustLockYaw_LH = 0;
    QWidget* WIG_fingerAdjustLockYaw_RH = 0;
    QWidget* WIG_fingerAdjustEnhanced_LH = 0;
    QWidget* WIG_fingerAdjustEnhanced_RH = 0;

    QWidget* WIG_Joystick_LH = 0;
    QWidget* WIG_Joystick_RH = 0;


    QWidget* WIG_jsCali_LH = 0;
    QWidget* WIG_jsCali_RH = 0;

    QWidget* WIG_h2GestureDisplay_LH = 0;
    QWidget* WIG_h2GestureDisplay_RH = 0;

    QWidget* WIG_steamVRMonitor = 0;

    QObject* m_h2UnityWindowMgr = 0;

	QWidget* WIG_h2SkeletonTree = 0;


private:
    std::string m_actorName;
    /*结尾是3E*/
    unsigned int m_sn=0;
    /*连接多套设备 把人按照距离分开*/
    //GloveSepMgr* m_pActorSepMgr = nullptr;
    /*全局配置文件*/
    h2GloveMode_TypeDef m_MocapGloveMode;

    QTimer* h2MonitorTimer = nullptr;
    unsigned int h2MonitorTimerHz = 1;


    std::string m_unityExeName = "foheart_hmax_u3d_port";

    //是否显示Index模拟调试按钮
    bool m_isIndexSimuDebug = false;

    // track current visibility of joystick calibration widgets to avoid redundant layout operations
    bool m_leftJsCaliVisible = true;
    bool m_rightJsCaliVisible = true;
    // track current visibility of joystick widgets
    bool m_leftJoystickVisible = true;
    bool m_rightJoystickVisible = true;
    //手势调试 弹出窗口
    QWidget* m_gestureContainer = 0;

    //串口
    unsigned int uartOpenStatus = 0;

    //嵌入
    QProcess* unityProcess = nullptr;
    QWindow* m_unityPortWindow = nullptr;
    QWidget* m_unityPortWidget = nullptr;

    bool m_isSteamVRMonitorEnabled = false;
private:
    void initUI();
    /*
    将配置文件中的设置读出来显示到界面上
    */
    void readGloveConfigFileToUI(void);

    void updateUartPorts(void);

    /*
    根据手套的在线离线状态 处理界面上的控件是否使能
    */
    void  updateUIFromGloveOnlineStatus(bool isLeftHandOnline, bool isRightHandOnline);
    std::string GetLocalDllDirectory();


    void setLeftJoystickCaliWidgetVisible(bool isVisible);
    void setRightJoystickCaliWidgetVisible(bool isVisible);

    // control visibility of joystick widgets (add/remove placeholders) to avoid repeated layout changes
    void setLeftJoystickWidgetVisible(bool isVisible);
    void setRightJoystickWidgetVisible(bool isVisible);

  
};
#ifdef _WIN32
class ProcessWorker : public QObject
{
    Q_OBJECT
public:
    ProcessWorker() {}
    ~ProcessWorker() {}
    std::string GetLocalDllDirectory()
    {
        std::string ret;
        // 创建一个足够大的字符数组来保存路径
        char dllPath[260] = { 0 };
        HMODULE hModule = GetModuleHandle(NULL);
        if (hModule != NULL)
        {
            if (GetModuleFileNameA(hModule, dllPath, sizeof(dllPath)))
            {
                std::string ws(dllPath);
                size_t pos = ws.find_last_of("\\");
                if (pos != std::string::npos)
                {
                    // Extract the directory part of the path
                    ws = ws.substr(0, pos);
                }
                ret = ws;
                //std::cout << "DLL Path: " << dllPath << std::endl;
            }
            else
            {
                //std::cerr << "Failed to get DLL path." << std::endl;
            }
        }
        else
        {
            //std::cerr << "Failed to get module handle." << std::endl;
        }
        return ret;
    }
public slots:
    void startProcess();
   

private slots:
    void handleProcessFinished()
    {
        // 处理完成后的逻辑...
    }
};
#else

#endif
