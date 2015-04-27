///////////////////////////////////////////////////////////
//  kupaExportInterface.h
//  Implementation of the Interface kupaExportInterface
//  Created on:      23-4-2013 오후 1:41:53
//  Original author: ysw
///////////////////////////////////////////////////////////

/**
 * \file    kupaExportInterface.h
 * \ingroup kupa
 * \brief   수집된 데이터를 내보내기 위한 공통 인터페이스를 제공하는 클래스인 kupaExportInterface 정의
 * 
 * 본 파일은, kupaCollectInterface의 구현으로부터 수집된 데이터를 다양한 형식으로 내보내기 위한 
 * 공통 인터페이스를 제공하는 클래스 kupaExportInterface 를 정의한다. \n 
 */

#if !defined(EA_D67CFCDC_7256_4d33_82CF_DB4351321472__INCLUDED_)
#define EA_D67CFCDC_7256_4d33_82CF_DB4351321472__INCLUDED_

#include "kupaCollectInterface.h"

/**
 * \class   kupaExportInterface
 * \brief   수집된 데이터를 내보내기 위한 공통 인터페이스를 제공하는 클래스
 * 
 * 본 클래스는 kupaCollectInterface의 구현으로부터 수집된 데이터를 다양한 형식으로 내보내기 위한 
 * 공통 인터페이스를 제공하는 클래스로서, 내부 구현이 없이 인터페이스만을 제공하기 위한 순수 가상함수들로 구성되어 있다. \n 
 * 내보내기 형식으로는 CSV, MDSplus, 파일, 통신 등이 될 수 있다. \n
 * 참고로, 현재 CSV와 MDSplus에 대해서만 구현되어 있다. \n
 */

class kupaExportInterface
{

public:

	/**
     * \fn     virtual ~kupaExportInterface()
     * \brief  소멸자
     */	
    virtual ~kupaExportInterface() {};


	/**
     * \fn     int store()
     * \brief  데이터 내보내기
     * \return 성공(kuOK), 실패(kuNOK)
     *
     * 수집된 데이터를 지정 형식으로 내보낸다. \n
     */
	virtual int store() =0;

};
#endif // !defined(EA_D67CFCDC_7256_4d33_82CF_DB4351321472__INCLUDED_)
