/****************************************************************************
 * kuTime.h
 * --------------------------------------------------------------------------
 * date and time interface
 * --------------------------------------------------------------------------
 * Copyright(c) 2012 NFRI. All Rights Reserved.
 * --------------------------------------------------------------------------
 * 2012.07.03  yunsw        Initial revision
 ****************************************************************************/

/**
 * \file    kuTime.h
 * \ingroup kutil
 * \brief   날짜와 시각을 제공하는 클래스인 kuDateTime 정의
 * 
 * 본 파일은 날짜와 시각 등의 기능을 지원하는 클래스인 kuDateTime 을 정의한다.
 */

#ifndef __KUTIL_TIME_H
#define __KUTIL_TIME_H

#include "kuCommon.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef POSIX_TIME_AT_EPICS_EPOCH
#define POSIX_TIME_AT_EPICS_EPOCH	631152000u	///< EPICS와 POSIX epoch 시각의 차이 (초)
#endif

/**
 * \fn     kuInt32 kuTimeEpoch ()
 * \brief  Epoch 시각 (초) 얻기
 * \return 00:00:00 Jan 1, 1970 UTC 시점 기준의 Epoch 시각(초)
 * \see    kuInt32 kuDateTime::getTime()
 */
kuShareFunc kuUInt32 kuShareAPI kuTimeEpoch ();

/**
 * \fn     kuTimeStamp kuTimeCurrrent (kuUInt32 *sec, kuUInt32 *nsec)
 * \brief  Epoch 시각 (초) 과 나노 초 얻기
 * \param  sec     Epoch 시각(초)
 * \param  nsec    나노 초
 * \return Epoch 시각(초), 나노 초를 갖는 구조체 반환. epicsTimeStamp와 동일함
 * \see    kuInt32 kuDateTime::getTime(int &sec, int &nsec)
 */
kuShareFunc kuTimeStamp kuShareAPI kuTimeCurrrent (kuUInt32 *sec, kuUInt32 *nsec);

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus

// ---------------------------------------------------------------------------
// Constant & Macro Definitions
// ---------------------------------------------------------------------------

const kuInt32	ONE_SEC_MILLI	= 1000;		///< 1 초     = 1000 밀리초
const kuInt32	ONE_MILLI_MICRO	= 1000;		///< 1 밀리초 = 1000 마이크로초

// ---------------------------------------------------------------------------
// User Type Definitions
// ---------------------------------------------------------------------------

/**
 * \class   kuDateTime
 * \brief   날짜와 시각을 지원하는 클래스
 * 
 * 본 클래스는 날짜와 시각 등의 정보를 위한 API를 제공한다.  \n
 */
class kuDateTime
{

public :

	static const char DATE_DEFAULT_DELIMITER	= '/';		///< 날짜 문자열의 기본 구분자. 예) 2009/09/17
	static const char TIME_DEFAULT_DELIMITER	= ':';		///< 시각 문자열의 기본 구분자. 예) 15:05:37
	static const char NONE_DELIMITER 			= 0x00;		///< 구분자를 사용하지 않음

	/**
	 * \enum   DateFormat_e
	 * \brief  날짜(연월일) 문자열의 표시 형식 유형. D : Day, M : Month, Y : Year
	 */
	enum DateFormat_e {
		DATE_NONE,		///< 날짜를 표시하지 않음
		DATE_DD,		///< 일자만 표시. 예) 17
		DATE_MMDD,		///< 월일 표시. 예) 09/17 또는, 0917
		DATE_YYMMDD,	///< 연월일 표시. 예) 09/09/17 또는, 090917
		DATE_YYYYMM,	///< 연월 표시. 예) 2009/09 또는, 200909
		DATE_YYYYMMDD,	///< 연월일 표시. 기본 값. 예) 2009/09/17 또는, 20090917
		DATE_MMDDYY,	///< 월일년 표시. 예) 09/17/09 또는, 091709
		DATE_MMDDYYYY	///< 월일년 표시. 예) 09/17/2009 또는, 09172009
	};

	/**
	 * \enum   TimeFormat_e
	 * \brief  시각(시분초) 문자열의 표시 형식 유형. S : Second, M : Minute, H : Hour, mS : Millisecond, nS : Nanosecond
	 */
	enum TimeFormat_e {
		TIME_NONE,		///< 시각을 표시하지 않음

		TIME_HHMM,		///< 시분 표시. 예) 15:05

		// sec
		TIME_SS,		///< 초 표시. 예) 37
		TIME_MMSS,		///< 분초 표시. 예) 05:37 또는, 0537
		TIME_HHMMSS,	///< 시분초 표시. 기본 값. 예) 15:05:37 또는, 150537

		// msec
		TIME_mSS,		///< 초.밀리 표시. 예) 37.123
		TIME_MMmSS,		///< 분초.밀리 표시. 예) 05:37.123
		TIME_HHMMmSS,	///< 시분초.밀리 표시. 예) 15:05:37.123

		// nsec
		TIME_nSS,		///< 초.나노 표시. 예) 37.123346
		TIME_MMnSS,		///< 분초.나노 표시. 예) 05:37.123456
		TIME_HHMMnSS	///< 시분초.나노 표시. 예) 15:05:37.123456
	};

	/**
	 * \enum   DateTimeUpdate_e
	 * \brief  객체 내의 시각 변경 여부
	 *
	 * \see getDateTimeStr 함수를 이용하여 해당 객체 내의 시각을 얻을 때,
	 * 시스템의 현재 시각으로 갱신을 한 뒤 반환할 지의 여부를 나타낸다.
	 */
	enum DateTimeUpdate_e {
		DATETIME_CURRENT,	///< 객체 내의 시각 유지
		DATETIME_UPDATE		///< 객체 내의 시각을 시스템의 현재 시각으로 갱신
	};

	/**
	 * \enum   DateTimeNextDirection_e
	 * \brief  이전 또는, 이후 날짜 지정
	 *
	 * \see getTimeOfNextOneDay 함수를 이용하여 하루 이전.이후의 날짜를 얻을 때,
	 * 이전.이후의 여부를 나타낸다.
	 */
	enum DateTimeNextDirection_e {
		DATETIME_PREV_DAY,	///< 이전 날짜
		DATETIME_NEXT_DAY	///< 이후 날짜
	};

	/**
	 * \enum   DateTimeType_e
	 * \brief  이전 또는, 이후 날짜 지정
	 *
	 * \see getNextEpochTime 함수를 이용하여 이전.이후의 시각을 얻을 때,
	 * 현재를 기준으로 한 옵셋의 단위를 나타낸다. (일, 월, 연)
	 */
	enum DateTimeType_e {
		DATETIME_DAY,		///< 일 기준 옵셋 사용
		DATETIME_MON,		///< 월 기준 옵셋 사용
		DATETIME_YEAR		///< 연 기준 옵셋 사용
	};

	/**
     * \fn     kuDateTime ()
     * \brief  기본 생성자.
     * \return 없음
     *
     * 기본 생성자로서 시스템의 현재 시각을 얻어와 객체 멤버 변수에 보관한다. \n
     * 날짜와 시각의 구분자를, 기본 구분자인 DATE_DEFAULT_DELIMITER와 TIME_DEFAULT_DELIMITER 로 설정한다.
	 */
	kuDateTime ();

	/**
     * \fn     kuDateTime (const kuInt32 sec)
     * \brief  Epoch 시각을 사용하는 생성자.
     * \param  sec		생성될 객체에 설정할 Epoch 시각 (초)
     * \return 없음
     *
     * 입력으로 받은 Epoch 시각 \a sec 를 객체 멤버 변수에 보관한다. \n
     * 날짜와 시각의 구분자를, 기본 구분자인 DATE_DEFAULT_DELIMITER와 TIME_DEFAULT_DELIMITER 로 설정한다.
	 */
	kuDateTime (const kuInt32 sec);

	/**
     * \fn     kuDateTime (const kuInt32 sec, const kuInt32 nsec)
     * \brief  Epoch 시각과 나노 초를 사용하는 생성자.
     * \param  sec		생성될 객체에 설정할 Epoch 시각 (초)
     * \param  nsec		생성될 객체에 설정할 시각 (나노 초)
     * \return 없음
     *
     * 입력으로 받은 Epoch 시각 \a sec 와 나노 초 \a nsec 를 객체 멤버 변수에 보관한다. \n
     * 날짜와 시각의 구분자를, 기본 구분자인 DATE_DEFAULT_DELIMITER와 TIME_DEFAULT_DELIMITER 로 설정한다.
	 */
	kuDateTime (const kuInt32 sec, const kuInt32 nsec);

	/**
     * \fn     kuDateTime (const kuDateTime &rh)
     * \brief  다른 객체를 사용하는 복사 생성자.
     * \param  rh		현재 객체에 복사할 원본 kuDateTime 객체
     * \return 없음
     *
     * 입력으로 받은 kuDateTime 객체 \a rh 를 생성되는 객체에 복사한다. \n
     * 시각, 구분자, 표시 형식 등이 복사된다.
	 */
	kuDateTime (const kuDateTime &rh);

	~kuDateTime ();	///< 소멸자

	/**
     * \fn     kuInt32 getYear ()
     * \brief  객체 내의 연도 얻기
     * \return 연도 (1970 ~ 2038). 예) 2012 (2012년)
     *
     * 객체에 저장 되어 있는 시각으로부터 해당 연도를 반환한다. \n
	 * 객체 내에 저장되는 시각은 객체를 생성, setTime 함수 호출 또는, updateDateTime 함수 호출 시에만 변경된다. \n
     */
	inline kuInt32 getYear () { return (m_dateTimeFields.tm_year); };

	/**
     * \fn     kuInt32 getMonth ()
     * \brief  객체 내의 월 얻기
     * \return 월 (1 ~ 12). 예) 9 (09월)
     *
     * 객체에 저장 되어 있는 시각으로부터 해당 월을 반환한다. \n
	 * 객체 내에 저장되는 시각은 객체를 생성, setTime 함수 호출 또는, updateDateTime 함수 호출 시에만 변경된다. \n
     */
	inline kuInt32 getMonth () { return (m_dateTimeFields.tm_mon); };

	/**
     * \fn     kuInt32 getDay ()
     * \brief  객체 내의 일자 얻기
     * \return 일 (1 ~ 31). 예) 17 (17일)
     *
     * 객체에 저장 되어 있는 시각으로부터 해당 일자를 반환한다. \n
	 * 객체 내에 저장되는 시각은 객체를 생성, setTime 함수 호출 또는, updateDateTime 함수 호출 시에만 변경된다. \n
     */
	inline kuInt32 getDay () { return (m_dateTimeFields.tm_mday); };

	/**
     * \fn     kuInt32 getHour ()
     * \brief  객체 내의 시 얻기
     * \return 시 (0 ~ 23). 예) 15 (15시)
     *
     * 객체에 저장 되어 있는 시각으로부터 해당 시를 반환한다. \n
	 * 객체 내에 저장되는 시각은 객체를 생성, setTime 함수 호출 또는, updateDateTime 함수 호출 시에만 변경된다. \n
     */
	inline kuInt32 getHour () { return (m_dateTimeFields.tm_hour); };

	/**
     * \fn     kuInt32 getMinute ()
     * \brief  객체 내의 분 얻기
     * \return 분 (0 ~ 59). 예) 05 (05분)
     *
     * 객체에 저장 되어 있는 시각으로부터 해당 분을 반환한다. \n
	 * 객체 내에 저장되는 시각은 객체를 생성, setTime 함수 호출 또는, updateDateTime 함수 호출 시에만 변경된다. \n
     */
	inline kuInt32 getMinute () { return (m_dateTimeFields.tm_min); };

	/**
     * \fn     kuInt32 getSecond ()
     * \brief  객체 내의 초 얻기
     * \return 초 (0 ~ 59). 예) 37 (37초)
     *
     * 객체에 저장 되어 있는 시각으로부터 해당 초를 반환한다. \n
	 * 객체 내에 저장되는 시각은 객체를 생성, setTime 함수 호출 또는, updateDateTime 함수 호출 시에만 변경된다. \n
     */
	inline kuInt32 getSecond () { return (m_dateTimeFields.tm_sec); };

	/**
     * \fn     kuInt32 getMillis ()
     * \brief  객체 내의 밀리 초 얻기
     * \return 밀리 초 (0 ~ 999)
     *
     * 객체에 저장 되어 있는 시각으로부터 해당 밀리 초를 반환한다. \n
	 * 객체 내에 저장되는 시각은 객체를 생성, setTime 함수 호출 또는, updateDateTime 함수 호출 시에만 변경된다. \n
     */
	inline kuInt32 getMillis () { return (m_msec); };

	/**
     * \fn     kuInt32 getNanos ()
     * \brief  객체 내의 나노 초 얻기
     * \return 나노 초 (0 ~ 999999999)
     *
     * 객체에 저장 되어 있는 시각으로부터 해당 나노 초를 반환한다. \n
	 * 객체 내에 저장되는 시각은 객체를 생성, setTime 함수 호출 또는, updateDateTime 함수 호출 시에만 변경된다. \n
     */
	inline kuInt32 getNanos () { return (m_nsec); };

	/**
     * \fn     kuInt32 getTime ()
     * \brief  객체 내의 Epoch 시각 (초) 얻기
     * \return Epoch 시각 (초)
     *
     * 객체에 저장 되어 있는 시각으로부터 해당 Epoch 시각을 반환한다. \n
	 * Epoch 시각은 1970년 1월 1일 00시 00분 00초 UTC(Coordinated Universal Time) 으로부터 seconds의 수를 표현한다 \n
	 * 객체 내에 저장되는 시각은 객체를 생성, setTime 함수 호출 또는, updateDateTime 함수 호출 시에만 변경된다. \n
     */	
	inline kuInt32 getTime () { return (m_sec); };

	/**
     * \fn     void getTime (kuInt32 &sec, kuInt32 &nsec)
     * \brief  객체 내의 Epoch 시각 (초) 과 나노 초 얻기
     * \param  sec		Epoch 시각 (초) 반환
     * \param  nsec		나노 초 반환
     * \return 없음
     *
     * 객체에 저장 되어 있는 시각으로부터 해당 Epoch 시각과 나노 초를 반환한다. \n
	 * Epoch 시각은 \a sec 에 그리고, 나노 초는 \a nsec 에 저장 되어 반환된다.
     */	
	inline void getTime (kuInt32 &sec, kuInt32 &nsec) { sec = m_sec; nsec = m_nsec; };

	/**
     * \fn     kuInt32 getTimeMillis ()
     * \brief  객체 내의 밀리 초 얻기
     * \return 밀리 초 (0 ~ 999)
     * \see    kuInt32 getMillis()
     *
     * 객체에 저장 되어 있는 시각으로부터 해당 밀리 초를 반환한다. \n
	 * 객체 내에 저장되는 시각은 객체를 생성, setTime 함수 호출 또는, updateDateTime 함수 호출 시에만 변경된다. \n
	 * getMillis 함수와 동일하다.
     */
	inline kuInt32 getTimeMillis () { return (m_msec); };

	/**
     * \fn     kuInt32 getTimeNanos ()
     * \brief  객체 내의 나노 초 얻기
     * \return 나노 초 (0 ~ 999999999)
     * \see    kuInt32 getNanos()
     *
     * 객체에 저장 되어 있는 시각으로부터 해당 나노 초를 반환한다. \n
	 * 객체 내에 저장되는 시각은 객체를 생성, setTime 함수 호출 또는, updateDateTime 함수 호출 시에만 변경된다. \n
  	 * getNanos 함수와 동일하다.
     */
	inline kuInt32 getTimeNanos () { return (m_nsec); };

	/**
     * \fn     void setDateDelimiter (const char delimiter)
     * \brief  날짜 문자열의 구분자 설정
     * \param  delimiter	시각 구분자 (예, '/' 또는, '-')
     * \return 없음
     * \see    DATE_DEFAULT_DELIMITER
     *
     * 날짜 문자열에 사용될 구분자를 \a delimiter 로 설정한다. \n
  	 * 기본 구분자는 DATE_DEFAULT_DELIMITER 이다.
     */
	inline void setDateDelimiter (const char delimiter) { m_dateDelimiter = delimiter; };

	/**
     * \fn     void setTimeDelimiter (const char delimiter)
     * \brief  시각 문자열의 구분자 설정
     * \param  delimiter	시각 구분자 (예, ':' 또는, '.')
     * \return 없음
     * \see    TIME_DEFAULT_DELIMITER
     *
     * 시각 문자열에 사용될 구분자를 \a delimiter 로 설정한다. \n
  	 * 기본 구분자는 TIME_DEFAULT_DELIMITER 이다.
     */
	inline void setTimeDelimiter (const char delimiter) { m_timeDelimiter = delimiter; };

	/**
     * \fn     void setDateFormat (DateFormat_e dateFormat)
     * \brief  날짜 문자열의 표시 형식 설정
     * \param  dateFormat	날짜 표시 형식 (예, DATE_YYYYMMDD, DATE_MMDD)
     * \return 없음
     *
     * 날짜 문자열의 연.월.일 표시 형식을 설정한다. \n
     */
	inline void setDateFormat (DateFormat_e dateFormat) { m_dateFormat = dateFormat; };

	/**
     * \fn     void setTimeFormat (TimeFormat_e timeFormat)
     * \brief  시각 문자열의 표시 형식 설정
     * \param  timeFormat	시각 표시 형식 (예, TIME_HHMMSS, TIME_HHMMmSS)
     * \return 없음
     *
     * 시각 문자열의 시.분.초 표시 형식을 설정한다. \n
     */
	inline void setTimeFormat (TimeFormat_e timeFormat) {m_timeFormat = timeFormat; };

	/**
     * \fn     void setTime (const kuInt32 sec)
     * \brief  Epoch 시각을 이용한 객체 시각 설정
     * \param  sec		설정할 Epoch 시각(초)
     * \return 없음
     *
     * Epcoh 시각 \a sec 로 객체 내의 시각을 설정한다. \n
     */
	void setTime (const kuInt32 sec);

	/**
     * \fn     void setTime (const kuInt32 sec, const kuInt32 nsec)
     * \brief  Epoch 시각과 나노 초를 이용한 객체 시각 설정
     * \param  sec		설정할 Epoch 시각(초)
     * \param  nsec		설정할 나노 초
     * \return 없음
     *
     * Epcoh 시각 \a sec 와 나노 초 \a nsec 로 객체 내의 시각을 설정한다. \n
     */
	void setTime (const kuInt32 sec, const kuInt32 nsec);

	/**
     * \fn     void setTime (const kuDateTime &dateTime)
     * \brief  다른 객체를 이용한 객체 시각 설정
     * \param  dateTime 	설정할 kuDateTime 객체
     * \return 없음
     *
     * 다른 객체 \a dateTime 로 객체 내의 시각을 설정한다. \n
     */
	void setTime (const kuDateTime &dateTime);

	/**
     * \fn     char *getLongDateTimeStr (DateTimeUpdate_e dateTimeUpdate = DATETIME_CURRENT)
     * \brief  날짜 및 시각에 대한 긴 형식의 문자열 얻기
     * \param  dateTimeUpdate	객체 내의 시각 갱신 여부
     * \return 날짜 및 시각을 갖는 문자열
     *
     * "Wed Jun 30 21:49:08 1993" 형식의 문자열을 반환한다. \n
	 * 내부적으로 ctime 함수의 출력 결과를 사용한다. \n
	 * \a dateTimeUpdate 의 값이 DATETIME_UPDATE 인 경우, 객체 내의 시각을 갱신한 뒤 그 결과를 반환한다.
     */
	const char *getLongDateTimeStr (DateTimeUpdate_e dateTimeUpdate = DATETIME_CURRENT);

	/**
     * \fn     char *getDateTimeStr (DateTimeUpdate_e dateTimeUpdate = DATETIME_CURRENT)
     * \brief  날짜 및 시각 문자열 얻기
     * \param  dateTimeUpdate	객체 내의 시각 갱신 여부
     * \return 날짜 및 시각을 갖는 문자열
     *
     * "2012/07/17 15:05:37" 형식의 문자열을 반환한다. \n
	 * 날짜의 출력 형식은 사전 설정된 날짜 문자열 구분자와 표시 형식 유형 \a DateFormat_e 에 따른다. \n
	 * 시각의 출력 형식은 사전 설정된 시각 문자열 구분자와 표시 형식 유형 \a TimeFormat_e 에 따른다. \n
	 * 문자열 구분자와 표시 형식 유형을 별도로 지정하지 않은 경우, 기본값을 사용한다. \n
	 * \a dateTimeUpdate 의 값이 DATETIME_UPDATE 인 경우, 객체 내의 시각을 갱신한 뒤 그 결과를 반환한다.
     */
	const char *getDateTimeStr (DateTimeUpdate_e dateTimeUpdate = DATETIME_CURRENT);

	/**
     * \fn     char *getDateStr (DateTimeUpdate_e dateTimeUpdate = DATETIME_CURRENT)
     * \brief  날짜 문자열 얻기
     * \param  dateTimeUpdate	객체 내의 시각 갱신 여부
     * \return 날짜를 갖는 문자열
     *
     * "2012/07/17" 형식의 문자열을 반환한다. \n
	 * 날짜의 출력 형식은 사전 설정된 날짜 문자열 구분자와 표시 형식 유형 \a DateFormat_e 에 따른다. \n
	 * 문자열 구분자와 표시 형식 유형을 별도로 지정하지 않은 경우, 아래의 기본값을 사용한다. \n
	 * - 날짜 문자열의 기본 구분자    : DATE_DEFAULT_DELIMITER
	 * - 날짜 문자열의 기본 표시 형식 : DATE_YYYYMMDD \n
	 * \a dateTimeUpdate 의 값이 DATETIME_UPDATE 인 경우, 객체 내의 시각을 갱신한 뒤 그 결과를 반환한다.
     */
	const char *getDateStr (DateTimeUpdate_e dateTimeUpdate = DATETIME_CURRENT);

	/**
     * \fn     char *getTimeStr (DateTimeUpdate_e dateTimeUpdate = DATETIME_CURRENT)
     * \brief  시각 문자열 얻기
     * \param  dateTimeUpdate	객체 내의 시각 갱신 여부
     * \return 시각을 갖는 문자열
     *
     * "15:05:37" 형식의 문자열을 반환한다. \n
	 * 시각의 출력 형식은 사전 설정된 시각 문자열 구분자와 표시 형식 유형 \a TimeFormat_e 에 따른다. \n
	 * 문자열 구분자와 표시 형식 유형을 별도로 지정하지 않은 경우, 아래의 기본값을 사용한다. \n
	 * - 시각 문자열의 기본 구분자    : TIME_DEFAULT_DELIMITER
	 * - 시각 문자열의 기본 표시 형식 : TIME_HHMMSS \n
	 * \a dateTimeUpdate 의 값이 DATETIME_UPDATE 인 경우, 객체 내의 시각을 갱신한 뒤 그 결과를 반환한다.
     */
	const char *getTimeStr (DateTimeUpdate_e dateTimeUpdate = DATETIME_CURRENT);

	/**
     * \fn     void updateDateTime (DateTimeUpdate_e dateTimeUpdate = DATETIME_UPDATE)
     * \brief  객체 내의 시각 갱신
     * \param  dateTimeUpdate	객체 내의 시각 갱신 여부
     * \return 없음
     *
	 * \a dateTimeUpdate 의 값이 DATETIME_UPDATE 인 경우, 객체 내의 시각을 갱신한다.
     */
	void updateDateTime (DateTimeUpdate_e dateTimeUpdate = DATETIME_UPDATE);

	/**
     * \fn     kuInt32 getTimeTerm (const kuDateTime &rh)
     * \brief  두 객체간의 시각 차이 얻기
     * \param  rh	비교할 상대 kuDateTime 객체
     * \return 두 객체의 시각 차이(밀리 초)
     *
     * 해당 객체와 입력 객체 \a rh 간의 시각 차이를 밀리 초 환산하여 반환한다\n
     */
	const kuInt32 getTimeTerm (const kuDateTime &rh);

	// -------------------------------------------------------
	// static methods
	// -------------------------------------------------------

	/**
     * \fn     char *getCurrLongDateTimeStr ()
     * \brief  현재 날짜 및 시각에 대한 긴 형식의 문자열 얻기
     * \return 현재 날짜 및 시각을 갖는 문자열
     * \see    char *getLongDateTimeStr(DateTimeUpdate_e dateTimeUpdate = DATETIME_CURRENT)
     *
     * 현재 시각에 대한 "Wed Jun 30 21:49:08 1993" 형식의 문자열을 반환한다. \n
     */
	static const char *getCurrLongDateTimeStr ();

	/**
     * \fn     char *getCurrDateTimeStr (const DateFormat_e dateFormat = DATE_YYYYMMDD, const char dateDelimiter = DATE_DEFAULT_DELIMITER, 
	 *					const TimeFormat_e timeFormat = TIME_HHMMSS, const char timeDelimiter = TIME_DEFAULT_DELIMITER)
     * \brief  현재 날짜 및 시각 문자열 얻기
     * \param  dateFormat	 날짜 표시 형식 (예, DATE_YYYYMMDD, DATE_MMDD)
     * \param  dateDelimiter 날짜 구분자 (예, ':' 또는, '.')
     * \param  timeFormat	 시각 표시 형식 (예, TIME_HHMMSS, TIME_HHMMmSS)
     * \param  timeDelimiter 시각 구분자 (예, ':' 또는, '.')
     * \return 현재 날짜 및 시각을 갖는 문자열
     * \see    char *getDateTimeStr(DateTimeUpdate_e dateTimeUpdate = DATETIME_CURRENT)
     *
     * 현재 시각에 대한 "2012/07/17 15:05:37" 형식의 문자열을 반환한다. \n
     */
	static const char *getCurrDateTimeStr (
			const DateFormat_e dateFormat = DATE_YYYYMMDD,
			const char dateDelimiter = DATE_DEFAULT_DELIMITER,
			const TimeFormat_e timeFormat = TIME_HHMMSS,
			const char timeDelimiter = TIME_DEFAULT_DELIMITER);

	/**
     * \fn     const char *getCurrDateStr (const DateFormat_e dateFormat = DATE_YYYYMMDD, const char dateDelimiter = DATE_DEFAULT_DELIMITER)
     * \brief  현재 날짜 문자열 얻기
     * \param  dateFormat	 날짜 표시 형식 (예, DATE_YYYYMMDD, DATE_MMDD)
     * \param  dateDelimiter 날짜 구분자 (예, ':' 또는, '.')
     * \return 현재 날짜를 갖는 문자열
     * \see    char *getDateStr(DateTimeUpdate_e dateTimeUpdate = DATETIME_CURRENT)
     *
     * 현재 시각에 대한 "2012/07/17" 형식의 문자열을 반환한다. \n
     */
	static const char *getCurrDateStr (const DateFormat_e dateFormat = DATE_YYYYMMDD, 
			const char dateDelimiter = DATE_DEFAULT_DELIMITER);

	/**
     * \fn     char *getCurrTimeStr (const TimeFormat_e timeFormat = TIME_HHMMSS, const char timeDelimiter = TIME_DEFAULT_DELIMITER)
     * \brief  현재 시각 문자열 얻기
     * \param  timeFormat	 시각 표시 형식 (예, TIME_HHMMSS, TIME_HHMMmSS)
     * \param  timeDelimiter 시각 구분자 (예, ':' 또는, '.')
     * \return 현재 시각을 갖는 문자열
     * \see    char *getTimeStr(DateTimeUpdate_e dateTimeUpdate = DATETIME_CURRENT)
     *
     * 현재 시각에 대한 "15:05:37" 형식의 문자열을 반환한다. \n
     */
	static const char *getCurrTimeStr (const TimeFormat_e timeFormat = TIME_HHMMSS, 
			const char timeDelimiter = TIME_DEFAULT_DELIMITER);

	/**
     * \fn     kuInt32 getCurrDateTime ()
     * \brief  현재 Epoch 시각 (초) 얻기
     * \return Epoch 시각 (초)
     * \see    kuInt32 getTime()
     *
     * 시스템의 현재 시각에 대한 Epoch 시각 (초)을 반환한다. \n
	 * 내부적으로, void getCurrDateTime (kuInt32 &sec, kuInt32 &nsec) 함수를 호출한다.
     */
	static kuInt32 getCurrDateTime ();

	/**
     * \fn     void getCurrDateTime (kuInt32 &sec, kuInt32 &nsec)
     * \brief  현재 Epoch 시각 (초) 과 나노 초 얻기
     * \param  sec		Epoch 시각 (초) 반환
     * \param  nsec		나노 초 반환
     * \see    void getTime(kuInt32 &sec, kuInt32 &nsec)
     *
     * 시스템의 현재 시각에 대한 Epoch 시각과 나노 초를 반환한다. \n
	 * Epoch 시각은 \a sec 에 그리고, 나노 초는 \a nsec 에 저장 되어 반환된다.
     */
	static void getCurrDateTime (kuInt32 &sec, kuInt32 &nsec);

	/**
     * \fn     void getCurrDateTimeMillis (kuInt32 &sec, kuInt32 &msec)
     * \brief  현재 Epoch 시각 (초) 과 밀리 초 얻기
     * \param  sec		Epoch 시각 (초) 반환
     * \param  msec		밀리 초 반환
     *
     * 시스템의 현재 시각에 대한 Epoch 시각과 밀리 초를 반환한다. \n
	 * Epoch 시각은 \a sec 에 그리고, 밀리 초는 \a msec 에 저장 되어 반환된다.
     */
	static void getCurrDateTimeMillis (kuInt32 &sec, kuInt32 &msec);

	/**
     * \fn     char *getDateTimeStr (kuInt32 sec, 
						const DateFormat_e dateFormat = DATE_YYYYMMDD, const char dateDelimiter = DATE_DEFAULT_DELIMITER, 
	 *					const TimeFormat_e timeFormat = TIME_HHMMSS, const char timeDelimiter = TIME_DEFAULT_DELIMITER)
     * \brief  지정 시각에 대한 날짜 및 시각 문자열 얻기
     * \param  sec			 문자열을 얻고자 하는 입력 시각
     * \param  dateFormat	 날짜 표시 형식 (예, DATE_YYYYMMDD, DATE_MMDD)
     * \param  dateDelimiter 시각 구분자 (예, ':' 또는, '.')
     * \param  timeFormat	 시각 표시 형식 (예, TIME_HHMMSS, TIME_HHMMmSS)
     * \param  timeDelimiter 시각 구분자 (예, ':' 또는, '.')
     * \return 날짜 및 시각을 갖는 문자열
     * \see    char *getCurrDateTimeStr(const DateFormat_e dateFormat = DATE_YYYYMMDD, const char dateDelimiter = DATE_DEFAULT_DELIMITER,
	 *					const TimeFormat_e timeFormat = TIME_HHMMSS, const char timeDelimiter = TIME_DEFAULT_DELIMITER)
     *
     * 지정한 Epoch 시각 \a sec 에 대한 "2012/07/17 15:05:37" 형식의 문자열을 반환한다. \n
     */
	static const char *getDateTimeStr (
			kuInt32 sec,
			const DateFormat_e dateFormat = DATE_YYYYMMDD,
			const char dateDelimiter = DATE_DEFAULT_DELIMITER,
			const TimeFormat_e timeFormat = TIME_HHMMSS,
			const char timeDelimiter = TIME_DEFAULT_DELIMITER);

	/**
     * \fn     char *getDateStr (kuInt32 sec, const DateFormat_e dateFormat = DATE_YYYYMMDD, const char dateDelimiter = DATE_DEFAULT_DELIMITER)
     * \brief  지정 시각에 대한 날짜 문자열 얻기
     * \param  sec			 문자열을 얻고자 하는 입력 시각
     * \param  dateFormat	 날짜 표시 형식 (예, DATE_YYYYMMDD, DATE_MMDD)
     * \param  dateDelimiter 날짜 구분자 (예, ':' 또는, '.')
     * \return 날짜를 갖는 문자열
     * \see    char *getCurrDateStr (const DateFormat_e dateFormat = DATE_YYYYMMDD, const char dateDelimiter = DATE_DEFAULT_DELIMITER)
     *
     * 지정한 Epoch 시각 \a sec 에 대한 "2012/07/17" 형식의 문자열을 반환한다. \n
     */
	static const char *getDateStr (
			kuInt32 sec,
			const DateFormat_e dateFormat = DATE_YYYYMMDD,
			const char dateDelimiter = DATE_DEFAULT_DELIMITER);

	/**
     * \fn     char *getTimeStr (kuInt32 sec, const TimeFormat_e timeFormat = TIME_HHMMSS, const char timeDelimiter = TIME_DEFAULT_DELIMITER)
     * \brief  지정 시각에 대한 시각 문자열 얻기
     * \param  sec			 문자열을 얻고자 하는 입력 시각
     * \param  timeFormat	 시각 표시 형식 (예, TIME_HHMMSS, TIME_HHMMmSS)
     * \param  timeDelimiter 시각 구분자 (예, ':' 또는, '.')
     * \return 시각을 갖는 문자열
     * \see    char *getCurrTimeStr (const TimeFormat_e timeFormat = TIME_HHMMSS, const char timeDelimiter = TIME_DEFAULT_DELIMITER)
     *
     * 지정한 Epoch 시각 \a sec 에 대한 "15:05:37" 형식의 문자열을 반환한다. \n
     */
	static const char *getTimeStr (
			kuInt32 sec,
			const TimeFormat_e timeFormat = TIME_HHMMSS,
			const char timeDelimiter = TIME_DEFAULT_DELIMITER);

	/**
     * \fn     char *getDateTimeStr (char *buf,kuInt32 sec, 
						const DateFormat_e dateFormat = DATE_YYYYMMDD, const char dateDelimiter = DATE_DEFAULT_DELIMITER, 
	 *					const TimeFormat_e timeFormat = TIME_HHMMSS, const char timeDelimiter = TIME_DEFAULT_DELIMITER)
     * \brief  지정 시각에 대한 날짜 및 시각 문자열 얻기
     * \param  buf			 날짜 및 시각을 갖는 문자열이 저장될 버퍼의 주소
     * \param  sec			 문자열을 얻고자 하는 입력 시각
     * \param  dateFormat	 날짜 표시 형식 (예, DATE_YYYYMMDD, DATE_MMDD)
     * \param  dateDelimiter 시각 구분자 (예, ':' 또는, '.')
     * \param  timeFormat	 시각 표시 형식 (예, TIME_HHMMSS, TIME_HHMMmSS)
     * \param  timeDelimiter 시각 구분자 (예, ':' 또는, '.')
     * \return 날짜 및 시각을 갖는 문자열
     * \see    char *getDateTimeStr(kuInt32 sec, 
	 *					const DateFormat_e dateFormat = DATE_YYYYMMDD, const char dateDelimiter = DATE_DEFAULT_DELIMITER,
	 *					const TimeFormat_e timeFormat = TIME_HHMMSS, const char timeDelimiter = TIME_DEFAULT_DELIMITER)
     *
     * 지정한 Epoch 시각 \a sec 에 대한 "2012/07/17 15:05:37" 형식의 문자열을 반환한다. \n
	 * 문자열은 반환 값과 함께 \a buf 에 보관되어 반환된다.
     */
	static const char *getDateTimeStr (
			char *buf,
			kuInt32 sec,
			const DateFormat_e dateFormat = DATE_YYYYMMDD,
			const char dateDelimiter = DATE_DEFAULT_DELIMITER,
			const TimeFormat_e timeFormat = TIME_HHMMSS,
			const char timeDelimiter = TIME_DEFAULT_DELIMITER);

	/**
     * \fn     char *getDateStr (char *buf,kuInt32 sec, 
						const DateFormat_e dateFormat = DATE_YYYYMMDD, const char dateDelimiter = DATE_DEFAULT_DELIMITER)
     * \brief  지정 시각에 대한 날짜 문자열 얻기
     * \param  buf			 날짜 문자열이 저장될 버퍼의 주소
     * \param  sec			 문자열을 얻고자 하는 입력 시각
     * \param  dateFormat	 날짜 표시 형식 (예, DATE_YYYYMMDD, DATE_MMDD)
     * \param  dateDelimiter 시각 구분자 (예, ':' 또는, '.')
     * \return 날짜를 갖는 문자열
     * \see    char *getDateStr(kuInt32 sec, const DateFormat_e dateFormat = DATE_YYYYMMDD, const char dateDelimiter = DATE_DEFAULT_DELIMITER)
     *
     * 지정한 Epoch 시각 \a sec 에 대한 "2012/07/17" 형식의 문자열을 반환한다. \n
	 * 문자열은 반환 값과 함께 \a buf 에 보관되어 반환된다.
     */
	static const char *getDateStr (
			char *buf,
			kuInt32 sec,
			const DateFormat_e dateFormat = DATE_YYYYMMDD,
			const char dateDelimiter = DATE_DEFAULT_DELIMITER);

	/**
     * \fn     char *getTimeStr (char *buf,kuInt32 sec, 
	 *					const TimeFormat_e timeFormat = TIME_HHMMSS, const char timeDelimiter = TIME_DEFAULT_DELIMITER)
     * \brief  지정 시각에 대한 시각 문자열 얻기
     * \param  buf			 시각을 갖는 문자열이 저장될 버퍼의 주소
     * \param  sec			 문자열을 얻고자 하는 입력 시각
     * \param  timeFormat	 시각 표시 형식 (예, TIME_HHMMSS, TIME_HHMMmSS)
     * \param  timeDelimiter 시각 구분자 (예, ':' 또는, '.')
     * \return 시각을 갖는 문자열
     * \see    char *getTimeStr(kuInt32 sec, const TimeFormat_e timeFormat = TIME_HHMMSS, const char timeDelimiter = TIME_DEFAULT_DELIMITER)
     *
     * 지정한 Epoch 시각 \a sec 에 대한 "15:05:37" 형식의 문자열을 반환한다. \n
	 * 문자열은 반환 값과 함께 \a buf 에 보관되어 반환된다.
     */
	static const char *getTimeStr (
			char *buf,
			kuInt32 sec,
			const TimeFormat_e timeFormat = TIME_HHMMSS,
			const char timeDelimiter = TIME_DEFAULT_DELIMITER);

	/**
     * \fn     kuInt32 getEpochTime (const char *timeStr)
     * \brief  문자열로부터 시각 얻기
     * \param  timeStr		날짜 및 시각을 갖는 문자열. (예, "20090917150537", "2009/09/17 15:05:37")
     * \return Epoch 시각 (초)
     *
     * 날짜 및 시각을 갖는 문자열 \a timeStr 로부터 Epoch 시각을 반환한다. \n
	 * 입력 문자열 내의 숫자만으로 시각을 계산한다. \n
	 * 즉, 입력 "2012/07/17 15:05:37" 은, 내부적으로 "20090917150537" 으로 처리된다.
     */
	static const kuUInt32 getEpochTime (const char *timeStr);

	/**
     * \fn     kuInt32 getNextEpochTime (const DateTimeType_e type, const time_t epochTime, const kuInt32 nNext, const kuInt32 offset = 0)
     * \brief  이전 또는, 이후 일자의 시각 얻기
     * \param  type			nNext의 기준. 일, 월 또는, 연 기준
     * \param  epochTime	기준 시각 (초)
     * \param  nNext		지정 시각으로부터의 이전, 이후 일/월/연. 음수(이전), 양수(이후)
     * \param  offset		기준 시각의 옵셋 적용 여부. 0(옵셋 비적용), 1(기준 시각의 옵셋 적용)
     * \return Epoch 시각 (초)
     *
     * 지정한 Epoch 시각 \a epochTime 으로부터 \a nNext 만큼의 이전 또는 이후의 시각을 반환한다. \n
	 * 날짜 기준은 \a type 으로 지정 되며, 일, 월 또는, 연 기준을 지정할 수 있다. \n
	 * 즉, 기준에 따른 관련 함수는 각각 다음과 같ㄷ. \n
	 *	- DATETIME_DAY	: \see kuInt32 getTimeOfNextDay(const time_t epochTime, const kuInt32 nNextDay, const kuInt32 offset = 0)
	 *	- DATETIME_MON	: \see kuInt32 getTimeOfNextMonth(const time_t epochTime, const kuInt32 nNextMonth, const kuInt32 offset = 0)
	 *	- DATETIME_YEAR	: \see kuInt32 getTimeOfNextYear(const time_t epochTime, const kuInt32 nNextYear, const kuInt32 offset = 0)
	 * \n
	 * \a offset이 1이면 기준 시각의 옵셋을 적용한 대상 시각을 반환한다.
     */
	static const kuInt32 getNextEpochTime (const DateTimeType_e type, const time_t epochTime, const kuInt32 nNext, const kuInt32 offset = 0);

	/**
     * \fn     kuInt32 getTimeOfNextDay (const time_t epochTime, const kuInt32 nNextDay, const kuInt32 offset = 0)
     * \brief  이전 또는, 이후 일자의 시각 얻기
     * \param  epochTime	기준 시각 (초)
     * \param  nNextDay		지정 일자로부터의 이전, 이후 일수. 음수(이전), 양수(이후)
     * \param  offset		기준 시각이 속한 일자의 옵셋 적용 여부. 0(옵셋 비적용), 1(기준 시각의 옵셋 적용)
     * \return Epoch 시각 (초)
     *
     * 지정한 Epoch 시각 \a epochTime 이 속한 일자로부터 \a nNextDay 일수 이전 또는 이후의 시각을 반환한다. \n
	 * \a offset 이 0 이면 대상 일자의 시작 시각을, 1이면 기준 시각의 옵셋을 적용한 대상 시각을 반환한다.
	 * 예를 들어, 기준 시각 \a epochTime이 2012년 07월 17일 15시 05분 37초이고, \a nNextDay 가 -2 일 때
	 *		- offset 이 0 인 경우, 2012년 07월 15일 00시 00분 00초에 해당하는 Epoch 시각 반환
	 *		- offset 이 1 인 경우, 2012년 07월 15일 15시 05분 37초에 해당하는 Epoch 시각 반환
     */
	static const kuInt32 getTimeOfNextDay (const time_t epochTime, const kuInt32 nNextDay, const kuInt32 offset = 0);

	/**
     * \fn     kuInt32 getTimeOfNextOneDay (const time_t epochTime, const kuInt32 direction, const kuInt32 offset = 0)
     * \brief  1일 이전 또는, 1일 이후 일자의 시각 얻기
     * \param  epochTime	기준 시각 (초)
     * \param  direction	지정 일자로부터의 이전 또는, 이후 여부. DATETIME_PREV_DAY(이전), DATETIME_NEXT_DAY(이후)
     * \param  offset		기준 시각이 속한 일자의 옵셋 적용 여부. 0(옵셋 비적용), 1(기준 시각의 옵셋 적용)
     * \return Epoch 시각 (초)
     *
     * 지정한 Epoch 시각 \a epochTime 이 속한 일자로부터 1일 이전 또는, 이후의 시각을 반환한다. \n
	 * \a direction 은 \a DateTimeNextDirection_e 에 정의된 enum 상수를 사용할 수 있다. \n
	 * \a offset 이 0 이면 대상 일자의 시작 시각을, 1이면 기준 시각의 옵셋을 적용한 대상 시각을 반환한다.
     */
	static const kuInt32 getTimeOfNextOneDay (const time_t epochTime, const kuInt32 direction, const kuInt32 offset = 0);

	/**
     * \fn     kuInt32 getLastDayOfMonth (const kuInt32 year, const kuInt32 month)
     * \brief  해당 연.월의 마지막 일자 얻기
     * \param  year		기준 연도 (1970 ~ )
     * \param  month	기준 월 (0 ~ 11) (월 - 1. 즉, 2월인 경우 1을 입력함)
     * \return 일자 (28 ~ 31)
     *
     * 지정한 연도 \a year, 월 \a month 의 마지막 일자를 반환한다. \n
     */
	static const kuInt32 getLastDayOfMonth (const kuInt32 year, const kuInt32 month);

	/**
     * \fn     kuInt32 getTimeOfNextYear (const time_t epochTime, const kuInt32 nNextYear, const kuInt32 offset = 0)
     * \brief  이전 또는, 이후 연도의 시각 얻기
     * \param  epochTime	기준 시각 (초)
     * \param  nNextYear	지정 연도로부터의 이전, 이후 연수. 음수(이전), 양수(이후)
     * \param  offset		기준 시각이 속한 연도의 옵셋 적용 여부. 0(옵셋 비적용), 1(기준 시각의 옵셋 적용)
     * \return Epoch 시각 (초)
     *
     * 지정한 Epoch 시각 \a epochTime 이 속한 연도로부터 \a nNextYear 연수 이전 또는 이후의 시각을 반환한다. \n
	 * \a offset 이 0 이면 대상 연도의 시작 시각을, 1이면 기준 시각의 옵셋을 적용한 대상 시각을 반환한다. \n
	 * 예를 들어, 기준 시각 \a epochTime이 2012년 07월 17일 15시 05분 37초이고, \a nNextYear 이 2 일 때
	 *		- offset 이 0 인 경우, 2014년 01월 01일 00시 00분 00초에 해당하는 Epoch 시각 반환
	 *		- offset 이 1 인 경우, 2014년 07월 17일 15시 05분 37초에 해당하는 Epoch 시각 반환
     */
	static const kuInt32 getTimeOfNextYear (const time_t epochTime, const kuInt32 nNextYear, const kuInt32 offset = 0);

	/**
     * \fn     kuInt32 getTimeOfNextMonth (const time_t epochTime, const kuInt32 nNextMonth, const kuInt32 offset = 0)
     * \brief  이전 또는, 이후 월의 시각 얻기
     * \param  epochTime	기준 시각 (초)
     * \param  nNextMonth	지정 월로부터의 이전, 이후 개월 수. 음수(이전), 양수(이후)
     * \param  offset		기준 시각이 속한 월의 옵셋 적용 여부. 0(옵셋 비적용), 1(기준 시각의 옵셋 적용)
     * \return Epoch 시각 (초)
     *
     * 지정한 Epoch 시각 \a epochTime 이 속한 달로부터 \a nNextMonth 개월 이전 또는 이후의 시각을 반환한다. \n
	 * \a offset 이 0 이면 대상 월의 시작 시각을, 1이면 기준 시각의 옵셋을 적용한 대상 시각을 반환한다.
	 * 예를 들어, 기준 시각 \a epochTime이 2012년 07월 17일 15시 05분 37초이고, \a nNextMonth 가 2 일 때
	 *		- offset 이 0 인 경우, 2012년 09월 01일 00시 00분 00초에 해당하는 Epoch 시각 반환
	 *		- offset 이 1 인 경우, 2012년 09월 17일 15시 05분 37초에 해당하는 Epoch 시각 반환
     */
	static const kuInt32 getTimeOfNextMonth (const time_t epochTime, const kuInt32 nNextMonth, const kuInt32 offset = 0);

	/**
     * \fn     kuInt32 getStartTimeOfDay (const time_t epochTime)
     * \brief  지정 Epoch 시각의 해당 일자 시작 시각 얻기
     * \param  epochTime	입력 시각
     * \return Epoch 시각 (초)
     *
     * 지정한 Epoch 시각 \a epochTime 이 속한 해당 일자 시작 시각의 Epoch 시각을 반환한다. \n
     */
	static const kuInt32 getStartTimeOfDay (const time_t epochTime);

	/**
     * \fn     kuInt32 getEndTimeOfDay (const time_t epochTime)
     * \brief  지정 Epoch 시각의 해당 일자 마지막 시각 얻기
     * \param  epochTime	입력 시각
     * \return Epoch 시각 (초)
     *
     * 지정한 Epoch 시각 \a epochTime 이 속한 해당 일자 마지막 시각의 Epoch 시각을 반환한다. \n
     */
	static const kuInt32 getEndTimeOfDay (const time_t epochTime);

	/**
     * \fn     kuInt32 getEndTimeOfMonth (const time_t epochTime)
     * \brief  지정 Epoch 시각의 해당 월 시작 시각 얻기
     * \param  epochTime	입력 시각
     * \return Epoch 시각 (초)
     *
     * 지정한 Epoch 시각 \a epochTime 이 속한 해당 월의 시작 날짜 시작 시각의 Epoch 시각을 반환한다. \n
     */
	static const kuInt32 getStartTimeOfMonth (const time_t epochTime);

	/**
     * \fn     kuInt32 getEndTimeOfMonth (const time_t epochTime)
     * \brief  지정 Epoch 시각의 해당 월 마지막 시각 얻기
     * \param  epochTime	입력 시각
     * \return Epoch 시각 (초)
     *
     * 지정한 Epoch 시각 \a epochTime 이 속한 해당 월의 마지막 날짜 마지막 시각의 Epoch 시각을 반환한다. \n
     */
	static const kuInt32 getEndTimeOfMonth (const time_t epochTime);

	/**
     * \fn     void getDateTimeFields (const kuInt32 sec, struct tm &dateTimeFields)
     * \brief  지정 Epoch 시각에 대한 시각 정보 구조체 얻기
     * \param  sec			  문자열을 얻고자 하는 입력 시각
     * \param  dateTimeFields 시각 정보 구조체
     * \return 없음
     *
     * 지정한 Epoch 시각 \a sec 에 대한 시각 정보를 아래의 구조에 저장하여 반환한다. \n
	 * \code
	 * 	 struct tm {
	 *         kuInt32 tm_sec;     // seconds after the minute - [0,59] 
	 *         kuInt32 tm_min;     // minutes after the hour - [0,59] 
	 *         kuInt32 tm_hour;    // hours since midnight - [0,23] 
	 *         kuInt32 tm_mday;    // day of the month - [1,31] 
	 *         kuInt32 tm_mon;     // months since January - [0,11] 
	 *         kuInt32 tm_year;    // years since 1900 
	 *         kuInt32 tm_wday;    // days since Sunday - [0,6] 
	 *         kuInt32 tm_yday;    // days since January 1 - [0,365] 
	 *         kuInt32 tm_isdst;   // daylight savings time flag 
	 *   };
	 * \endcode
     */
	static void getDateTimeFields (const kuInt32 sec, struct tm &dateTimeFields);

private :

	int			m_sec;
	int			m_msec;
	int			m_nsec;

	DateFormat_e	m_dateFormat;
	TimeFormat_e	m_timeFormat;

	char			m_dateDelimiter;
	char			m_timeDelimiter;

	struct tm		m_dateTimeFields;

	void initDateTime (const kuInt32 sec, const kuInt32 nsec);

	inline static kuInt32 getSecondFromMsec (const kuInt32 msec) { return (msec / 1000); };
	inline static kuInt32 getNanosFromMsec (const kuInt32 msec) { return ((msec % 1000) * 1000000); };
	inline static kuInt32 getMillisFromNanos (const kuInt32 nsec) { return (nsec / 1000000); };

	static const char *getLongDateTimeStr (const kuInt32 sec);
	static const char *getDateTimeStr (const struct tm &dateTimeFields, const kuInt32 nsec,
			const DateFormat_e dateFormat, const char dateDelimiter,
			const TimeFormat_e timeFormat, const char timeDelimiter);
	static const char *getDateStr (const struct tm &dateTimeFields, 
			const DateFormat_e dateFormat, const char dateDelimiter);
	static const char *getTimeStr (const struct tm &dateTimeFields, const kuInt32 nsec,
			const TimeFormat_e timeFormat, const char timeDelimiter);

	static const char *getDateTimeStr (char *buf, const struct tm &dateTimeFields, const kuInt32 nsec,
			const DateFormat_e dateFormat, const char dateDelimiter,
			const TimeFormat_e timeFormat, const char timeDelimiter);
	static const char *getDateStr (char *buf, const struct tm &dateTimeFields, 
			const DateFormat_e dateFormat, const char dateDelimiter);
	static const char *getTimeStr (char *buf, const struct tm &dateTimeFields, const kuInt32 nsec,
			const TimeFormat_e timeFormat, const char timeDelimiter);

};

#endif	// __cplusplus

#endif	// __KUTIL_TIME_H

