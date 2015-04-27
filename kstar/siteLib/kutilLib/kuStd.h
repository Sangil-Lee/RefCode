/**
 * \file    kuStd.h
 * \ingroup kutil
 * \brief   기본 함수를 제공 (C 표준 라이브러리 및 기타 기능)
 * 
 * 본 파일은, 기본 함수를 제공하는 클래스인 kuStd를 정의한다. \n
 */

#ifndef	__KUTIL_STD_H
#define	__KUTIL_STD_H

// ---------------------------------------------------------------------------
// System Include Files
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// User Defined Include Files
// ---------------------------------------------------------------------------

#include "kuCommon.h"

// ---------------------------------------------------------------------------
// Constant & Macro Definitions
// ---------------------------------------------------------------------------

#define	KU_STD_MAX_COPY_SIZE		81920		///< 복사 1회 당 최대 메모리 크기 (바이트 수)
#define	KU_STD_ALIGN_UNIT			4			///< 메모리 정렬을 위한 최소 단위 (바이트 수)
#define	KU_HEXA_DECIMAL_PREIFX		"0x"		///< 16진수 문자열을 나타내는 선행 문자열

// ---------------------------------------------------------------------------
// User Type Definitions
// ---------------------------------------------------------------------------

#ifdef __cplusplus

/**
 * \class   kuStd
 * \brief   기본 함수를 제공하는 클래스
 * 
 * 본 클래스는 기본 함수를 제공하는 클래스로서, \n
 * 성능 및 안정성 개선을 위하여 일부 C 표준 라이브러리를 대체할 함수를 제공한다. \n
 * 아울러, 추가적인 편의 함수들을 제공한다. \n
 * 본 클래스의 함수들은 모두 static 함수이므로, 별도로 객체를 생성함 없이 사용이 가능하다.
 */

class kuStd
{

public :

	/**
     * \fn     void *memcpy (void *dest, const void *src, const size_t &len)
     * \brief  메모리 복사
     * \param  dest 원본 버퍼인 \a src 의 결과를 복사할 대상 버퍼의 주소
     * \param  src  복사할 정보를 갖는 원본 버퍼의 주소
     * \param  len  복사할 정보의 길이 (바이트 수)
     * \return 대상 버퍼인 \a dest 의 주소를 반환. 오류 시는 NULL을 반환한다.
     * 
     * 원본 버퍼인 \a src 로부터 크기 \a len 만큼의 바이트를 대상 버퍼인 \a dest 로 복사한다.
     * 크기가 RT_STD_MAX_COPY_SIZE 보다 클 경우는,\n
     * 내부적으로 RT_STD_MAX_COPY_SIZE 단위 만큼 여러 번에 걸쳐 복사가 이루어진다.
     */
	static void *memcpy (void *dest, const void *src, const size_t &len);

	/**
     * \fn     void *memset (void *buf, const kuInt32 &ch, const size_t &len)
     * \brief  메모리 초기화
     * \param  buf  초기값 \a ch 로 초기화 될 버퍼의 주소
     * \param  ch   버퍼 \a buf 를 초기화 할 값
     * \param  len  초기화 할 버퍼의 길이 (바이트 수)
     * \return 대상 버퍼인 \a buf 의 주소를 반환. 오류 시는 NULL을 반환한다.
     * 
     * 버퍼 \a buf 에 대해 크기 \a len 만큼의 바이트를 \a ch 값으로 초기화 한다.
     */
	static void *memset (void *buf, const kuInt32 &ch, const size_t &len);

	/**
     * \fn     kuInt32 equal (const char *s1, const char *s2)
     * \brief  두 개의 문자열 비교
     * \param  s1   비교할 문자열의 주소
     * \param  s2   문자열 \a s1에 비교 되는 문자열의 주소
     * \return 0(두 문자열이 같은 경우), 양수(문자열 \a s1 이 클 경우), 음수(문자열 \a s1 이 작을 경우)
     * 
     * 문자열 \a s1 이 문자열 \a s2 와 같은지를 비교 한다.
     * 입력되는 두 문자열 \a s1 과 \a s2 는 NULL 로 종료되는 문자열의 형식을 취하여야 한다.
     */
	static kuInt32 equal (const char *s1, const char *s2);

	/**
     * \fn     template<typename T> kuInt32 equal (T v1, T v2, T gap)
     * \brief  두 개의 값이 오차 범위 이내에 있는지 비교
     * \param  v1     비교되는 값 1
     * \param  v2     비교되는 값 2
     * \param  gap    값의 오차 범위
     * \return 0(두 문자열이 같은 경우), 양수(문자열 \a s1 이 클 경우), 음수(문자열 \a s1 이 작을 경우)
     * \see    kuInt32 kuDoubleEqual (double v1, double v2, double gap)
     *
     * 값 \a v1 이 값 \a v2 과 오차범위 이내에 있는지를 비교 한다.
     * 입력되는 두 값은 정수 또는, 실수의  데이터 유형을 사용 하여야 한다.
     */
	template<typename T>
	static kuInt32 equal (T v1, T v2, T gap);

	/**
     * \fn     kuInt32 equal (const char *s1, const char *s2, const size_t &len)
     * \brief  두 개의 문자열에 대해 일정 길이만큼 비교
     * \param  s1   비교할 문자열의 주소
     * \param  s2   문자열 \a s1에 비교 되는 문자열의 주소
     * \param  len  비교 될 문자열의 길이 (바이트 수)
     * \return 0(두 문자열이 같은 경우), 양수(문자열 \a s1 이 클 경우), 음수(문자열 \a s1 이 작을 경우)
     * 
     * 문자열 \a s1 의 시작으로부터 크기 \a len 만큼의 메모리 값이 문자열 \a s2 와 같은지를 비교 한다.\n
     * 입력되는 두 문자열 \a s1 과 \a s2 가 NULL 로 종료 되지 않아도 되며,\n
     * 타입 캐스팅을 사용하면 임의 데이터 유형의 버퍼에 대해서도 사용이 가능하다.
     */
	static kuInt32 equal (const char *s1, const char *s2, const size_t &len);

	/**
     * \fn     kuInt32 timecmp (const kuInt32 &lsec, const kuInt32 &lmsec, const kuInt32 &rsec, const kuInt32 &rmsec)
     * \brief  두 개의 시각 비교
     * \param  lsec  비교할 시각 (초) : 처음 시각의 초
     * \param  lmsec 비교할 시각 (밀리 초) : 처음 시각의 밀리 초 (1/1000)
     * \param  rsec  시각 \a lsec 와 \a lmsec 에 비교 되는 시각 (초)
     * \param  rmsec 시각 \a lsec 와 \a lmsec 에 비교 되는 시각 (밀리 초)
     * \return 0(두 시각이 같은 경우), 양수(처음 시각이 클 경우), 음수(처음 시각이 작을 경우)
     * 
     * 밀리 초의 처음 시각 \a lsec 와 \a lmsec 가 비교 되는 시각 \a rsec 와 \a rmsec 와 같은지를 비교 한다.
     * 시각의 초가 같을 경우, 밀리 초에 대해 비교가 이루어진다.
     */
	static kuInt32 timecmp (const kuInt32 &lsec, const kuInt32 &lmsec, const kuInt32 &rsec, const kuInt32 &rmsec);

	/**
     * \fn     const kuInt32 getAlignSize (const kuInt32 &size)
     * \brief  입력값에 대한 정렬 크기 반환
     * \param  size  정렬 크기를 얻고자 하는 (메모리 용량을 나타내는) 크기
     * \return RT_STD_ALIGN_UNIT 단위로 정렬된 크기 반환
     * 
     * 공유 메모리 등의 생성 및 접근시 사용 될 메모리 크기를 나타내는 입력값 \a size 에 대하여 \n
     * RT_STD_ALIGN_UNIT 단위로 정렬된 크기를 반환 한다. \n
     * 운영체제에서는 데이터 유형에 따라 메모리 정렬을 하는 과정 중에 실제 정의 되지 않은 메모리를 추가할 수도 있다.
     * 이러한 슬랙 바이트 (또는, Padding data) 는 메모리 접근시 문제를 유발할 수도 있다. \n
     * 이를 방지하기 위하여, 사전 생성시 일정 단위로 정렬된 크기를 사용할 수 있도록 기능을 제공 한다.
	 * 다음의 식에 의해 정렬된 크기를 계산한다. \n
	 *     (size + (RT_STD_ALIGN_UNIT - 1)) / RT_STD_ALIGN_UNIT * RT_STD_ALIGN_UNIT)
     */
	static const kuInt32 getAlignSize (const kuInt32 &size);

	/**
     * \fn     const char *int2str (const kuInt32 ival)
     * \brief  정수형 숫자에 해당하는 ASCII 문자열 반환
     * \param  ival  정수형 숫자
     * \return 입력 받은 정수형 숫자에 해당하는 문자열을 갖는 버퍼의 주소 반환
     * 
     * 입력으로 받은 정수형 숫자 \a ival 을 ASCII 문자열로 변환한 뒤, ASCII 문자열을 갖는 버퍼의 주소를 반환 한다 \n
     */
	static const char *int2str (const kuInt32 ival);

	/**
     * \fn     const kuInt32 getValueFromStr (const char *valStr)
     * \brief  문자열에 해당하는 정수형 숫자 반환
     * \param  valStr  정수형 숫자를 갖는 문자열 (예, "28" 또는, "0x1C")
     * \return 정수형 숫자 반환
     * 
     * 입력으로 받은 정수형 숫자 형태의 문자열 \a valStr 을 정수형 숫자로 변환하여 반환 한다 \n
     * 16진수 형태의 숫자를 갖는 문자열은, "0x" 로 시작 하여야 한다.
     */
	static const kuInt32 getValueFromStr (const char *valStr);

	/**
     * \fn     const kuUInt32 binaryToUInt32 (const char *valStr)
     * \brief  이진수 문자열에 해당하는 부호없는 정수형 숫자 반환
     * \param  valStr  이진수형 숫자를 갖는 문자열 (예, "1101" 또는, "1001")
     * \return 부호없는 정수형 숫자 반환
     * 
     * 입력으로 받은 이진수형 숫자 형태의 문자열 \a valStr 을 부호없는 정수형 숫자로 변환하여 반환 한다 \n
     */
	static const kuUInt32 binaryToUInt32 (const char *valStr);

	/**
     * \fn     const float getFloatFromStr (const char *valStr)
     * \brief  문자열에 해당하는 실수형 숫자 반환
     * \param  valStr  숫자 (2진수형, 16진수형, 실수형) 를 갖는 문자열
     * \return 실수형 숫자 반환
     * 
     * 입력으로 받은 숫자 (2진수형, 16진수형, 실수형) 형태의 문자열 \a valStr 을 실수형 숫자로 변환하여 반환 한다 \n
     * 2진수 형태의 숫자를 갖는 문자열은, 'b' 또는, 'B' 로 시작 하여야 한다.
     * 16진수 형태의 숫자를 갖는 문자열은, "0x" 로 시작 하여야 한다.
     */
	static const float getFloatFromStr (const char *valStr);

	/**
     * \fn     void upper (const char *src, char *dest)
     * \brief  입력 받은 문자열을 대문자로 변환
     * \param  src   대문자로 변환 하고자 하는 입력 문자열의 주소
     * \param  dest  입력 문자열 \a src 에 대해 대문자를 갖는 대상 문자열의 주소
     * \return 없음
     * 
     * 입력 받은 문자열 \a src 내의 소문자를 대문자로 변환하여 대상 버퍼 \a dest 에 저장한다.
     */
	static void upper (const char *src, char *dest);

	/**
     * \fn     static void lower (const char *src, char *dest)
     * \brief  입력 받은 문자열을 소문자로 변환
     * \param  src   소문자로 변환 하고자 하는 입력 문자열의 주소
     * \param  dest  입력 문자열 src 에 대해 소문자를 갖는 대상 문자열의 주소
     * \return 없음
     * 
     * 입력 받은 문자열 \a src 내의 대문자를 소문자로 변환하여 대상 버퍼 \a dest 에 저장한다.
     */
	static void lower (const char *src, char *dest);

	/**
     * \fn     void dump (const char *buf, const kuInt32 nMaxLenPerLine = 20)
     * \brief  입력 받은 문자열의 내용을 16 진수로 출력
     * \param  buf   출력할 내용을 갖는 문자열의 주소
     * \param  nMaxLenPerLine   한 줄에 표시할 내용의 최대 길이 (기본 : 20 바이트. 0 : 전체)
     * \return 없음
     * 
     * 입력 받은 문자열 \a buf 의 내용을 16 진수의 형태로 출력한다. \n
     * 입력 받은 문자열은 NULL 로 종료 되어야 한다. \n
     * \a nMaxLenPerLine 의 값이 0 이면, 입력 받은 문자열 전체를 한 줄로 출력한다. \n
     */
	static void dump (const char *buf, const kuInt32 nMaxLenPerLine = 20);
	
	/**
     * \fn     void dump (const kuInt32 len, const char *buf, const kuInt32 nMaxLenPerLine = 20)
     * \brief  입력 받은 버퍼의 내용을 16 진수로 출력
     * \param  len   출력할 내용의 길이
     * \param  buf   출력할 내용을 갖는 버퍼의 주소
     * \param  nMaxLenPerLine   한 줄에 표시할 내용의 최대 길이 (기본 : 20 바이트. 0 : 전체)
     * \return 없음
     * 
     * 입력 받은 버퍼 \a buf 로부터 길이 \a len 만큼의 내용을 16 진수의 형태로 출력한다. \n
     * ASCII 값이 아닌 정보를 표시하기 위하여 사용될 수 있다. (예, 통신 데이터 등) \n
     * \a nMaxLenPerLine 의 값이 0 이면, 입력 받은 문자열 전체를 한 줄로 출력한다. \n
     */
	static void dump (const kuInt32 len, const char *buf, const kuInt32 nMaxLenPerLine = 20);

	/**
     * \fn     static void todigit (const char *src, char *dest)
     * \brief  입력 받은 문자열로부터 숫자 추출
     * \param  src   숫자를 추출 하고자 하는 입력 문자열의 주소
     * \param  dest  입력 문자열 \a src 로부터 추출된 숫자를 갖는 문자열의 주소
     * \return 없음
     * 
     * 입력 받은 문자열 \a src 로부터 숫자만을 추출하여 대상 버퍼 \a dest 에 저장한다. \n
     * 이는, 'YYYY/MM/DD HH:MM:SS' 또는 'YYYY-MM-DD HH/MM/SS' 등과 같은 날짜 문자열로부터 숫자만을 추출하는 경우에 사용될 수 있다.
     */
   static void todigit (const char *src, char *dest);
    
	/**
     * \fn     const char *nullprint (const char *str)
     * \brief  출력 가능한 문자열 반환
     * \param  str   출력할 문자열의 주소
     * \return 출력 가능한 문자열의 주소 반환. \a str 이 NULL일 경우, "null" 문자열 반환
     * 
     * 입력 받은 문자열 \a buf 이 NULL 인지를 판변하여, 출력 가능한 문자열을 반환한다. \n
     * 입력 문자열의 주소가 NULL인 경우, prkuInt32f 등으로 출력시 문제를 유발하지 않도록 NULL 대신 "null" 문자열을 반환한다. 
     */
	static const char *nullprint (const char *str);

	/**
     * \fn     kuInt32 readn (const kuInt32 fd, void *buf, const kuInt32 nbytes)
     * \brief  요청한 길이만큼의 데이터 읽기
     * \param  fd     데이터를 읽고자 하는 파일의 식별자 (file descriptor)
     * \param  buf    파일로부터 읽은 데이터를 저장할 대상 버퍼의 주소
     * \param  nbytes 파일로부터 읽고자 하는 데이터의 길이 (바이트 수)
     * \return 파일로부터 읽은 데이터의 길이를 반환. 오류 시는 -1을 반환한다.
     * 
     * 파일 \a fd 로부터 크기 \a nbytes 만큼의 바이트를 읽어 대상 버퍼인 \a buf 에 저장 한다. \n
	 * 만일, 요청한 길이 보다 파일로부터 읽을 데이터가 적은 경우는, 요청한 만큼의 길이 보다 작은 값을 반환할 수 있다. \n
	 * 일반적으로 시스템 함수 \a read 를 이용하여 파일로부터 데이터를 읽는 과정에 운영체제로부터 인터럽트가 발생할 수 있다.
	 * 이러한 인터럽트로 인해 시스템 함수 \a read 는 요청한 만큼의 데이터를 모두 읽지 못한 채로 반환을 하게 되어
	 * 원하지 않는 결과를 초래하게 된다. \n
	 * 따라서, 본 함수는 인터럽트가 발생 하여도 요청한 만큼의 데이터를 읽을 수 있도록 보장한다.
     */
	static const kuInt32 readn (const kuInt32 fd, void *buf, const kuUInt32 nbytes);

	/**
     * \fn     kuInt32 writen (const kuInt32 fd, const void *buf, const kuInt32 nbytes)
     * \brief  요청한 길이만큼의 데이터 쓰기
     * \param  fd     데이터를 쓰고자 하는 파일의 식별자 (file descriptor)
     * \param  buf    파일에 기록할 데이터를 갖는 버퍼의 주소
     * \param  nbytes 파일에 기록할 데이터의 길이 (바이트 수)
     * \return 파일에 기록한 데이터의 길이를 반환. 오류 시는 -1을 반환한다.
     * 
     * 버퍼 \a buf 로부터 크기 \a nbytes 만큼의 바이트를 읽어 파일 \a fd 에 기록 한다. \n
	 * 일반적으로 시스템 함수 \a write 를 이용하여 파일에 데이터를 기록하는 과정에 운영체제로부터 인터럽트가 발생할 수 있다.
	 * 이러한 인터럽트로 인해 시스템 함수 \a write 는 요청한 만큼의 데이터를 모두 기록하지 못한 채로 반환을 하게 되어
	 * 원하지 않는 결과를 초래하게 된다. \n
	 * 따라서, 본 함수는 인터럽트가 발생 하여도 요청한 만큼의 데이터를 기록할 수 있도록 보장한다.
     */
	static const kuInt32 writen (const kuInt32 fd, const void *buf, const kuUInt32 nbytes);

	/**
     * \fn     kuInt32 cpdir (const char *src, const char *dest)
     * \brief  디렉토리 복사
     * \param  src  복사할 파일들을 갖는 원본 디렉토리의 경로명
     * \param  dest 원본 디렉토리인 \a src 전체를 복사할 대상 디렉토리의 경로명
     * \return 성공(kuOK), 실패(kuNOK)
     * 
     * 원본 디렉토리인 \a src 와 그 디렉토리 내의 파일들을 대상 디렉토리인 \a dest 로 복사한다.
     * 원본 디렉토리 내의 하위 디렉토리에 대해서도 자동으로 복사된다.
     */
	static const kuInt32 cpdir (const char *src, const char *dest);

	/**
     * \fn     kuInt32 deldir (const char *path)
     * \brief  디렉토리 삭제
     * \param  path 삭제할 디렉토리의 경로명
     * \return 성공(kuOK), 실패(kuNOK)
     * 
     * 디렉토리 \a path 를 삭제한다.
     * 디렉토리 내의 하위 디렉토리에 대해서도 자동으로 삭제된다.
     */
	static const kuInt32 deldir (const char *path);

	/**
     * \fn     kuBoolean isNan(T value)
     * \brief  입력값이 숫자인지의 여부를 반환
     * \param  value 숫자 여부를 판별하고자 하는 값 (숫자 또는, 문자열)
     * \return 숫자가 아닌 경우(TRUE), 숫자인 경우(FALSE)
     * 
     * 입력값 \a value 가 NaN (Not a Number) 인지를 반환한다.
     * 즉 순수한 문자이면 TRUE를 반환하고, 숫자 형태이면 FALSE를 반환한다. \n
	 * 본 함수는, float 변수가 비정상 값을 갖는지를 판별하기 위하여 사용될 수 있다.
	 * 참고로, 그 값이 NaN 인 float 변수에 대한 연산시 해당 프로그램에 문제를 초래할 수 있다. \n
	 *
	 * 본 함수 대신에 Linux의 경우 isnan 함수를 그리고, Windows의 경우는 _isnan을 사용할 수 있다.
     */
	template<typename T>
	static inline kuBoolean isNan(T value) { return value != value; }

	/**
     * \fn     kuInt32 scandir(const char *dir, struct dirent ***namelist, const char *compar)
     * \brief  디렉토리 목록 얻기
     * \param  dir       목록을 얻고자 하는 디렉토리의 경로명
     * \param  namelist  디렉토리에 대한 반환 목록
     * \param  compar    정렬시 사용될 함수의 이름 (예, alphasort)
     * \return 검색된 목록의 개수 반환. 실패 시 kuNOK 반환
     * \see     void free2d(T ***value, kuInt32 size)
     * 
     * 디렉토리 \a dir 내의 목록 전체를 얻어, \a namelist 에 저장한 뒤 반환한다.
     * Linux의 경우, 정렬 함수 \a compar 를 이용하여 정렬된 목록을 반환한다.
	 * 현재 alphasort 만을 지원 하므로, 기본으로는 alphasort 를 사용 하도록 한다. \n
	 *
	 * 본 함수의 사용법은, Linux의 scandir과 동일하다.
	 * 본 함수는 가변 목록을 저장하기 위하여 \a namelist 에 대해 malloc을 이용하여 메모리를 할당한다.
	 * 따라서, 목록에 대한 사용을 마친 이후 또는, 동일한 디렉토리에 대해 다시 목록을 얻고자 하는 경우에는
	 * free2d 함수를 이용하여 명시적으로 할당된 메모리를 해제해 주어야 한다.
     */
	static kuInt32 scandir(const char *dir, struct dirent ***namelist, const char *compar="alphasort");


	/**
     * \fn     void free2d(T ***value, kuInt32 size)
     * \brief  포인터를 배열로 갖는 포인터 변수에 대한 메모리 해제
     * \param  value     포인터 배열의 주소
     * \param  size      포인터 배열 내의 원소의 개수
     * \return 없음
     * \see     kuInt32 scandir(const char *dir, struct dirent ***namelist, const char *compar="alphasort")
     * 
     * 포인터를 배열로 갖는 포인터 변수 \a value 에 대해 할당된 메모리를 해제한다.
	 * 이 때, 개수 \a size 만큼의 원소에 대해 메모리를 해제한다. \n
     * 본 함수는 scandir 함수를 통해 반환된 목록 내의 메모리를 해제하기 위해 사용된다.
     */
	template<typename T>
	static inline void free2d(T ***value, kuInt32 size) { for(kuInt32 i=0;i<size;i++){ free((*value)[i]); } free(*value); *value=NULL;}

};

#endif	// __cplusplus

#if defined(WIN32)

struct dirent {
    char        d_name[256]; /* We must not include limits.h! */
};

#endif

// ---------------------------------------------------------------------------
// Function Declarations
// ---------------------------------------------------------------------------

#endif	// __KUTIL_STD_H
