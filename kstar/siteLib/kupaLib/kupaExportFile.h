///////////////////////////////////////////////////////////
//  kupaExportFile.h
//  Implementation of the Class kupaExportFile
//  Created on:      23-4-2013 오후 1:41:53
//  Original author: ysw
///////////////////////////////////////////////////////////

/**
 * \file    kupaExportFile.h
 * \ingroup kupa
 * \brief   수집된 데이터를 바이너리 파일로 저장하는 클래스인 kupaExportFile 정의. 미 구현
 */

#if !defined(EA_049C8714_563F_4d95_8533_5211E3404D15__INCLUDED_)
#define EA_049C8714_563F_4d95_8533_5211E3404D15__INCLUDED_

#include "kupaExportInterface.h"

/**
 * \class   kupaExportFile
 * \brief   수집된 데이터를 바이너리 파일로 내보내는 클래스. 미 구현
 * 
 * 본 클래스는 수집된 데이터를 바이너리 형식의 파일로 내보내는 클래스로서,
 * 수집된 데이터를 다양한 형식으로 내보내기 위한 공통 인터페이스를 제공하는 kupaExportInterface를 상속하여 그 내용을 구현코자 하였다. \n 
 * 현재, 구현되어 있지 않다.
 */

class kupaExportFile : public kupaExportInterface
{

public:
	kupaExportFile(kupaCollectInterface * pCollect);
	virtual ~kupaExportFile();

	virtual int store();

private:
	kupaCollectInterface *	m_kupaCollectInterface;

};
#endif // !defined(EA_049C8714_563F_4d95_8533_5211E3404D15__INCLUDED_)
