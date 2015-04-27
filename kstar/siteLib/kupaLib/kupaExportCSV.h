///////////////////////////////////////////////////////////
//  kupaExportCSV.h
//  Implementation of the Class kupaExportCSV
//  Created on:      23-4-2013 오후 1:41:50
//  Original author: ysw
///////////////////////////////////////////////////////////

/**
 * \file    kupaExportCSV.h
 * \ingroup kupa
 * \brief   수집된 데이터를 CSV 형식의 파일로 내보내는 클래스인 kupaExportCSV 정의
 * 
 * 본 파일은, 수집된 데이터를 CSV 형식의 파일로 내보내는 클래스인 kupaExportCSV 를 정의한다. \n 
 */

#if !defined(EA_064C272C_D73C_4823_ACF5_03D7687B52EA__INCLUDED_)
#define EA_064C272C_D73C_4823_ACF5_03D7687B52EA__INCLUDED_

#include "kupaExportInterface.h"

#define KUPA_CSV_EXT_STR		".csv"		///< CSV 형식의 파일 확장자

/**
 * \class   kupaExportCSV
 * \brief   수집된 데이터를 CSV 형식의 파일로 내보내는 클래스
 * 
 * 본 클래스는 수집된 데이터를 CSV 형식의 파일로 내보내는 클래스로서,
 * 수집된 데이터를 다양한 형식으로 내보내기 위한 공통 인터페이스를 제공하는 kupaExportInterface를 상속하여 그 내용을 구현하였다. \n 
 */

class kupaExportCSV : public kupaExportInterface
{
	/**
	 * \enum   kupaExportCsvOrder
	 * \brief  CSV 파일의 데이터 정렬 방법
	 */
	enum kupaExportCsvOrder {
		KUPA_CSV_ORDER_V		= 0,	///< 값을 수직 방향으로 표현
		KUPA_CSV_ORDER_H		= 1		///< 값을 수평 방향으로 표현
	};

	/**
	 * \enum   kupaExportCsvFormat
	 * \brief  CSV 파일의 유형
	 */
	enum kupaExportCsvFormat {
		KUPA_CSV_FORMAT_UNIX	= 0,	///< Unix 파일 형식
		KUPA_CSV_FORMAT_DOS		= 1		///< DOS 파일 형식. 미 지원
	};

	/**
	 * \enum   kupaExportCsvWriteMode
	 * \brief  CSV 파일의 생성 모드
	 */
	enum kupaExportCsvWriteMode {
		KUPA_CSV_WRITE_NEW		= 0,	///< 신규 파일로 생성함
		KUPA_CSV_WRITE_APPEND	= 1		///< 기존 파일에 추가함
	};

public:

	/**
     * \fn     kupaExportCSV(kupaCollectInterface * pCollect, char *fileName, char *path,
     *			char* valueOrder, char* format, char* writeMode, char* arg6, char* arg7, char* arg8, char* arg9)
     * \brief  생성자
     * \param  pCollect     데이터 수집 객체
     * \param  fileName     CSV 파일명
     * \param  path     	CSV 파일의 저장 경로
     * \param  valueOrder	CSV 파일의 데이터 정렬 방법. kupaExportCsvOrder. setValueOrder
     * \param  format     	CSV 파일의 유형. kupaExportCsvFormat. setFormat
     * \param  writeMode    CSV 파일의 생성 모드. kupaExportCsvWriteMode. setWriteMode
     * \param  arg6     	Reserved.
     * \param  arg7     	Reserved.
     * \param  arg8     	Reserved.
     * \param  arg9     	Reserved.
     * \return 없음
     *
     * 입력으로부터 내부 변수를 설정한다.\n
     */
	kupaExportCSV(kupaCollectInterface * pCollect, char *fileName, char *path,
			char* valueOrder, char* format, char* writeMode,
			char* arg6, char* arg7, char* arg8, char* arg9);

	/**
     * \fn     virtual ~kupaExportCSV()
     * \brief  소멸자
     */
	virtual ~kupaExportCSV();

	/**
     * \fn     int store()
     * \brief  데이터 내보내기
     * \return 성공(kuOK), 실패(kuNOK)
     * \see    append, storeToNewFile
     *
     * 수집된 데이터를 CSV 형식의 파일로 내보낸다. \n
     * CSV 파일의 생성 모드에 따라, 새로운 파일로 작성하거나 기존 파일에 추가한다. \n
     */
	virtual int store();

	/**
     * \fn     kupaCollectInterface * getCollectInterface()
     * \brief  데이터 수집 객체 얻기
     * \return 데이터 수집 객체
     *
     * 버퍼 내에 수집된 데이터를 갖고 있는 데이터 수집 객체를 얻는다.
     */
	kupaCollectInterface * getCollectInterface();

	string getFileName() { return (m_fileName); }				///< 파일명 얻기
	string getPathName() { return (m_pathName); }				///< 경로명 얻기
	int getValueOrder() { return (m_valueOrder); }				///< CSV 파일의 데이터 정렬 방법 얻기. kupaExportCsvOrder
	int getFormat() { return (m_format); }						///< CSV 파일의 유형 얻기. kupaExportCsvFormat
	int getWriteMode() { return (m_writeMode); }				///< CSV 파일의 생성 모드 얻기. kupaExportCsvWriteMode

	void setFileName(char *newVal) { m_fileName = newVal; }		///< 파일명 설정
	void setPathName(char *newVal) { m_pathName = newVal; }		///< 경로명 설정
	void setValueOrder(int newVal) { m_valueOrder = newVal; }	///< CSV 파일의 데이터 정렬 방법 설정. kupaExportCsvOrder
	void setValueOrder(char *newVal);							///< CSV 파일의 데이터 정렬 방법 설정
	void setFormat(int newVal) { m_format = newVal; }			///< CSV 파일의 유형 설정. kupaExportCsvFormat
	void setFormat(char *newVal);								///< CSV 파일의 유형 설정
	void setWriteMode(int newVal) { m_writeMode = newVal; }		///< CSV 파일의 생성 모드 설정. kupaExportCsvWriteMode
	void setWriteMode(char *newVal);							///< CSV 파일의 생성 모드 설정

private:
	kupaCollectInterface * m_kupaCollectInterface;				///< 생성자의 입력. 데이터 수집 객체

	string m_fileName;		///< 생성자의 입력. 파일명
	string m_pathName;		///< 생성자의 입력. 경로명
	int m_valueOrder;		///< 생성자의 입력. 데이터 정렬 방법
	int m_format;			///< 생성자의 입력. CSV 파일의 유형
	int m_writeMode;		///< 생성자의 입력. CSV 파일의 생성 모드

	// -------------------------------------
	// new
	// -------------------------------------

	/**
     * \fn     int storeToNewFile ()
     * \brief  수집된 데이터를 새로운 CSV 파일로 내보내기
     * \return 성공(kuOK), 실패(kuNOK)
     *
     * 수집된 데이터를 새로운 CSV 파일로 내보낸다. \n
     * 설정한 경로 내에 입력 파일명과 Shot 번호로 구성되는 파일을 생성한다. \n
     * 내부적으로 데이터 정렬 방법에 따라 storeHorizontal 또는, storeVertical 함수를 호출한다. \n
     */
	int storeToNewFile ();
	
	/**
     * \fn     int storeHorizontal(FILE *fp)
     * \brief  데이터를 수평 방향으로 표시한다.
     * \param  fp		storeToNewFile()에서 생성된 새로운 파일의 포인터
     * \return 성공(kuOK), 실패(kuNOK)
     */
	int storeHorizontal(FILE *fp);

	/**
     * \fn     int storeVertical(FILE *fp)
     * \brief  데이터를 수직 방향으로 표시한다.
     * \param  fp		storeToNewFile()에서 생성된 새로운 파일의 포인터
     * \return 성공(kuOK), 실패(kuNOK)
     */
	int storeVertical(FILE *fp);

	// -------------------------------------
	// append mode
	// -------------------------------------

	/**
     * \fn     int append ()
     * \brief  수집된 데이터를 기존 CSV 파일에 추가로 내보내기
     * \return 성공(kuOK), 실패(kuNOK)
     *
     * 수집된 데이터를 기존 CSV 파일에 추가로 내보낸다. \n
     * 만일 기존 CSV 파일이 없으면 새로운 파일이 생성된다. \n
     * 본 추가 모드에서는, 데이터가 시간 순으로 수직 방향으로 표시된다.
     */
	int append();
};
#endif // !defined(EA_064C272C_D73C_4823_ACF5_03D7687B52EA__INCLUDED_)
