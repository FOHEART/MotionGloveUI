/**
 * @file  h2JoyStickPadWidget.h
 * @brief 摇杆姿态显示控件的声明。
 * @ingroup h2ui
 * @version 1.0.0
 *
 * @details
 * 含 Q_OBJECT，需 moc 生成元对象代码。
 * 函数级参数与返回值说明见各函数自身的注释块。
 */

#pragma once

#include <QtWidgets/QWidget>
#include <QtWidgets/QDialog>
#include <QtWidgets/QInputDialog>
#include <QtWidgets/QMessageBox>
#include <QtCore/QDebug>
#include <QtGui/QPainter>
#include <QtGui/QPaintEvent>
#include <QtGui/QMouseEvent>

#define SMALL_CIRCLE_RADIUS 10 ///< 小圆（摇杆位置指示）半径，单位像素。
#define BIG_CIRCLE_RADIUS (100/2) ///< 大圆（活动范围）半径，单位像素。

/**
 * @brief 摇杆二维姿态盘：用「大圆 + 小圆」直观显示摇杆当前位置。
 *
 * @details 大圆为活动范围，小圆为当前摇杆位置；支持鼠标拖动并把位置换算成
 *          归一化坐标。所有图形都是运行时用 QPainter 绘制的，没有外部素材依赖
 *          （仅图标从模块目录加载）。
 */
class h2JoyStickPadWidget : public QWidget
{
	Q_OBJECT

public:
	/**
	 * @brief 构造控件、加载圆环贴图并初始化圆心位置。
	 * @param[in] parent 父控件，**可为 nullptr**。
	 */
	h2JoyStickPadWidget(QWidget *parent = nullptr);

	/** @brief 析构：释放两份 QPixmap。 */
	~h2JoyStickPadWidget();

	/**
	 * @brief 设置摇杆按键是否按下（影响小圆使用的贴图）。
	 * @param[in] s true 表示按下。
	 */
	void setJoyStickDown(bool s);

	/**
	 * @brief 设置摇杆的归一化坐标。
	 * @param[in] x 横轴位置，取值 [-1.0f, 1.0f]，0 为居中；超出范围会夹取到圆边界。
	 * @param[in] y 纵轴位置，取值 [-1.0f, 1.0f]，0 为居中；超出范围会夹取到圆边界。
	 * @post 小圆圆心随之移动并触发重绘。
	 */
	void setXY(float x, float y);

private:
	/** @brief 大圆圆心坐标（控件坐标系，像素）。 */
	QPoint BigCir_xy; //大圆圆心坐标
	/** @brief 小圆圆心坐标（控件坐标系，像素）。 */
	QPoint SmallCir_xy; //小圆圆心坐标

	/** @brief 鼠标是否处于按下拖动状态。 */
	bool MousePressFlag; //鼠标按下的标志位

	/** @brief 上一次鼠标位置，用于计算拖动增量。 */
	QPoint MapRemov_Old;

	/** @brief 大圆的常态贴图（灰色圈）。 */
	QPixmap bigCircle_Pixmap;//灰色圈
	/** @brief 大圆的边界贴图（蓝色圈）。 */
	QPixmap bigCircleEdge_Pixmap;//蓝色圈
	/** @brief 摇杆是否处于最大位移处。 */
	bool isValue1_JS = false;//摇杆达到最大值

	/** @brief 小圆的常态贴图。 */
	QPixmap smallCircle_Pixmap;//没按下
	/** @brief 小圆按下时的贴图。 */
	QPixmap smallCircleDown_Pixmap;//按下
	/** @brief 小圆位于零点时的贴图。 */
	QPixmap smallCircleZero_Pixmap;//0值
	/** @brief 摇杆是否处于零点。 */
	bool isValue0_JS = false;//摇杆达到0值

	/** @brief 摇杆按键是否按下。 */
	bool isKeyDown = false;
	/**
	 * @brief 初始化圆心坐标与贴图。
	 * @note 在构造函数中调用一次。
	 */
	void Init();

	/**
	 * @brief 绘制事件：画大圆、边界圈与当前小圆。
	 * @param[in] event 绘制事件对象，**不可为 nullptr**。
	 */
	void paintEvent(QPaintEvent *event); //绘图事件：绘制地图、摇杆中的大圆和小圆
	/**
	 * @brief 取本模块所在目录，用于定位 Resources 下的圆环贴图。
	 * @return 目录绝对路径，结尾不带分隔符；非 Windows 平台返回空串。
	 */
	std::string GetLocalDllDirectory();
};

