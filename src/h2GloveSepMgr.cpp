/**
 * @file  h2GloveSepMgr.cpp
 * @brief 手套左右分离管理器的实现。
 * @ingroup h2ui
 * @version 1.0.0
 *
 * @details
 * 纯声明或转发头，无运行期逻辑。
 * 函数级说明见对应头文件，此处不再重复 @param / @return。
 */

#include "../inc/h2GloveSepMgr.h"
h2GloveSepMgr::h2GloveSepMgr(QObject* parent)
	: QObject(parent)
{
}

h2GloveSepMgr::h2GloveSepMgr()
{

}

h2GloveSepMgr::~h2GloveSepMgr()
{
}

void h2GloveSepMgr::getCoordByIndex(int ActorIndex, float* loc)
{
	int hang = ActorIndex / m_liveActorPerRow;
	int lie = ActorIndex % m_liveActorPerRow;

	loc[0] = hang * m_seprateDistance;
	loc[1] = lie * m_seprateDistance;
	loc[2] = 0;

}
