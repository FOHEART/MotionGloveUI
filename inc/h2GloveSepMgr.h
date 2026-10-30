/**
 * @file  h2GloveSepMgr.h
 * @brief 手套左右分离（展开显示）管理器的内部接口。
 * @ingroup h2ui
 * @version 1.0.0
 *
 * @details
 * 含 Q_OBJECT，需 moc 生成元对象代码。
 * 函数级参数与返回值说明见各函数自身的注释块。
 */


#pragma once

#include <QtCore/QObject>

/**
 * @brief 多套手套的「分离摆放」管理器：把同场多套设备按网格坐标铺开。
 *
 * @details
 * 连接多套设备时所有人若都停在原点会互相重叠，本类按「每行
 * @ref m_liveActorPerRow 人」的网格给每个演员算一个平面坐标，
 * 便于在三维场景里一眼分辨。
 *
 * 只做坐标计算与间距存取，不含定时器与事件循环逻辑，
 * 可在 UI 线程任意调用；不做线程同步。
 */
class  h2GloveSepMgr : public QObject
{
	Q_OBJECT

public:
	/**
	 * @brief 构造管理器并挂到指定父对象。
	 * @param[in] parent 父 QObject，**可为 nullptr**；为空时无父对象，
	 *                   生命周期由调用方负责。
	 */
	h2GloveSepMgr(QObject* parent);

	/**
	 * @brief 构造无父对象的管理器。
	 * @note 没有父对象意味着不会被自动回收，调用方必须自行 delete。
	 */
	h2GloveSepMgr();

	/** @brief 析构。本类不持有需手动释放的资源。 */
	~h2GloveSepMgr();

	/**
	 * @brief 读取演员之间的摆放间距。
	 * @return 间距，单位米；默认值 -1.0f 表示「尚未配置」。
	 * @warning 默认值下 getCoordByIndex() 会算出负坐标，
	 *          调用方应先通过 setSeprateDistance() 设置有意义的正值。
	 */
	float getSeprateDistance() const { return m_seprateDistance; }

	/**
	 * @brief 设置演员之间的摆放间距。
	 * @param[in] val 间距，单位米，建议取值 (0.0f, 100.0f]；
	 *                传入 0 会让所有演员重叠在原点，传入负值会让坐标反号，
	 *                两种情况都不做拦截。
	 */
	void setSeprateDistance(float val) { m_seprateDistance = val; }

	/** @brief 网格每行可容纳的演员数，取值 [1, INT_MAX]，默认 5。 */
	int m_liveActorPerRow = 5;

	/**
	 * @brief 按网格布局计算第 @p ActorIndex 个演员的三维坐标。
	 * @param[in]  ActorIndex 演员序号，从 0 开始，取值 [0, INT_MAX]；
	 *                        超出网格范围时行号继续递增，不回绕。
	 * @param[out] loc        长度为 3 的浮点数组，**不可为 nullptr**；
	 *                        写入 {行号乘间距, 列号乘间距, 0}，
	 *                        Y 分量恒为 0；本函数不会失败返回。
	 * @pre @p loc 至少可容纳 3 个 float，且已通过 setSeprateDistance()
	 *      设置有效间距。
	 * @warning 未对 @p loc 做空指针检查。
	 */
	void getCoordByIndex(int ActorIndex, float* loc);

private:
	/** @brief 演员之间的摆放间距，单位米；-1.0f 表示尚未配置。 */
	float m_seprateDistance = -1.0f;
};