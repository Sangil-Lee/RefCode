///////////////////////////////////////////////////////////
//  kupaCollectStream.h
//  Implementation of the Class kupaCollectStream
//  Created on:      23-4-2013 오후 1:41:59
//  Original author: ysw
///////////////////////////////////////////////////////////

/**
 * \file    kupaCollectStream.h
 * \ingroup kupa
 * \brief   Stream 형태의 소스로부터 데이터를 수집하는 클래스인 kupaCollectStream 정의. 미 구현
 */

#if !defined(EA_712571D7_474D_47e7_949A_EEC4FA8DCA3F__INCLUDED_)
#define EA_712571D7_474D_47e7_949A_EEC4FA8DCA3F__INCLUDED_

#include "kupaCollectInterface.h"

/**
 * \class   kupaCollectStream
 * \brief   Stream 형태의 소스로부터 데이터를 수집하는 클래스. 미 구현
 * 
 * 본 클래스는 Stream 형태의 소스로부터 데이터를 수집하는 클래스로서,
 * 데이터 수집을 위한 공통 인터페이스를 제공하는 클래스인 kupaCollectInterface를 상속하여 그 내용을 구현코자 하였다. \n 
 * 현재, 구현되어 있지 않다.
 */

class kupaCollectStream : public kupaCollectInterface
{

public:
	kupaCollectStream();
	virtual ~kupaCollectStream();

	virtual int collect();
	virtual void reset();
	virtual int addPV(string pvName, string nodeName, int caType, string description, string unit);
	virtual int updatePV(string pvName, string value);

};
#endif // !defined(EA_712571D7_474D_47e7_949A_EEC4FA8DCA3F__INCLUDED_)
