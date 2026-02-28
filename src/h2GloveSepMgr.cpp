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
