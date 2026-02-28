
#pragma once

#include <QtCore/QObject>

/*
将手套向展开显示
*/
class  h2GloveSepMgr : public QObject
{
	Q_OBJECT

public:
	h2GloveSepMgr(QObject* parent);
	h2GloveSepMgr();
	~h2GloveSepMgr();

	/*连接多套设备 把人按照距离分开*/
	float getSeprateDistance() const { return m_seprateDistance; }
	void setSeprateDistance(float val) { m_seprateDistance = val; }

	int m_liveActorPerRow = 5;//一行有几个人

	void getCoordByIndex(int ActorIndex, float* loc);

private:
	float m_seprateDistance = -1.0f;
};