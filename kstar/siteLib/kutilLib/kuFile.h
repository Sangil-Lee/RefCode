/**
 * \file    kuFile.h
 * \ingroup kutil
 * \brief   파일 관리를 수행하는 클래스인 kuFile 정의
 * 
 * 본 파일은 파일 관리를 수행하는 클래스인 kuFile 을 정의한다.
 */

#ifndef	__KUTIL_FILE_H
#define	__KUTIL_FILE_H

// ---------------------------------------------------------------------------
// System Include Files
// ---------------------------------------------------------------------------

// for getrlimit, getrusage, setrlimit
#if !defined(WIN32)
#include <sys/time.h>
#include <sys/resource.h>
#include <unistd.h>
#endif
#include <fcntl.h>

// ---------------------------------------------------------------------------
// User Defined Include Files
// ---------------------------------------------------------------------------

#include "kuStd.h"

// ---------------------------------------------------------------------------
// Constant & Macro Definitions
// ---------------------------------------------------------------------------


// ---------------------------------------------------------------------------
// User Type Definitions
// ---------------------------------------------------------------------------
 
#ifdef __cplusplus

/**
 * \class   kuFile
 * \brief   파일 관리를 수행하는 클래스
 * 
 * 본 클래스는 파일에 대한 접근 방법을 제공 하는 클래스로서, \n
 * 임의의 텍스트 파일 (예, 환경 설정 파일) 로부터 한 줄을 읽거나, \n
 * 임의의 문자열에서 지정된 구분자를 이용하여 토큰을 분리하는 기능을 제공한다. \n
 * 또한, 파일의 정보 얻기, 복사, 이동, 록 등 파일 관련 기본 기능도 제공한다.
 * \n
 * 본 클래스에서는, 문자열의 최대 길이를 MAX_LINE_SIZE (즉, 1024) 로 가정한다.
 * 
 */
 
class kuFile
{

public :

	/**
     * \enum   FileType_e
     * \brief  파일 유형 정의
     */
	enum FileType_e {
		REGULAR,		///< 정규파일 (일반)
		DIRECTORY		///< 디렉터리
	};

	kuFile ();		///< 생성자. 애트리뷰트(멤버변수)의 초기화

	~kuFile ();		///< 소멸자. 열려져 있는 파일 닫기

	/**
     * \fn     char *getToken (const char *buffer, const char *delimiters, char *token)
     * \brief  문자열로부터 첫 번째 토큰 얻기
     * \param  buffer     입력 문자열을 갖는 버퍼 
     * \param  delimiters 토큰 구분자 문자열
     * \param  token      문자열로부터 얻은 첫 번째 토큰의 저장 버퍼
     * \return 반환한 토큰 이후의 위치 주소. 문자열의 끝에서는 NULL을 반환한다.
     *
     * 입력 받은 문자열 \a buffer 로부터 입력된 토큰 구분자들 \a delimiters 을 이용하여 토큰을 분리한다. \n
     * 첫 번째 위치하는 토큰 문자열을 \a token 에 저장한다. \n
     * 토큰 구분자들을 나타내는 \c delimiters 에는 여러 개의 토큰 구분자들이 사용될 수 있다.\n
     */
	static char *getToken (const char *buffer, const char *delimiters, char *token);

	/**
     * \fn     void delSpace (char *str)
     * \brief  입력 문자열 내의 앞.뒤 공백 문자 제거
     * \param  str     공백을 제거할 문자열
     * \return 없음
     *
     * 입력 받은 문자열 \a str 로부터 앞.뒤 공백을 제거한 뒤 반환한다. \n
     * 공백 문자 여부는 isspace() 함수로 판단 한다.
     */
	static void delSpace (char *str);
	
	/**
     * \fn     void delSpaceRear (const char *src, char *dest)
     * \brief  입력 문자열 끝의 공백 문자 제거 하기
     * \param  src     공백을 제거할 입력 문자열
     * \param  dest    끝의 공백이 제거된 문자열
     * \return 없음
     *
     * 입력 받은 문자열 \a src 끝의 공백을 제거한 뒤 \a dest 로 반환한다. \n
     */
	static void delSpaceRear (const char *src, char *dest);
	
	/**
     * \fn     void delSpaceAll (char *str)
     * \brief  입력 문자열 내의 모든 공백 문자 제거
     * \param  str     공백을 제거할 문자열
     * \return 없음
     *
     * 입력 받은 문자열 \a str 내의 모든 공백을 제거한 뒤 반환한다. \n
     */
	static void delSpaceAll (char *str);
	
	/**
     * \fn     const kuBoolean isExist (const char *path)
     * \brief  입력 경로명의 존재 여부 알기
     * \param  path     절대 또는, 상대 경로명
     * \return kuTRUE(존재함), kuFALSE(존재하지 않음)
     *
     * 입력 경로명 \a path 의 존재 여부를 반환한다. \n
     * 절대 또는, 상대 경로명이 올 수 있다. \n
     * 파일 또는, 디렉토리 여부를 구분하지 않으며, 존재할 경우 kuTRUE를 반환한다.
     */
	static const kuBoolean isExist (const char *path);

	/**
     * \fn     const kuInt64 size (const char *path)
     * \brief  파일의 크기 얻기
     * \param  path     절대 또는, 상대 경로명
     * \return 성공(파일의 크기), 실패(kuNOK)
     * \see    const kuInt32 getStat(const char *path, kuInt32 &type, kuInt64 &size)
     *
     * 입력 경로명 \a path 의 파일에 대해 그 크기를 반환한다. \n
     * 내부적으로 getStat()를 호출하여 얻은 파일의 크기를 반환한다.
     */
	static const kuInt64 size (const char *path);

	/**
     * \fn     const kuInt32 type (const char *path)
     * \brief  입력 경로의 유형 얻기
     * \param  path     절대 또는, 상대 경로명
     * \return 성공(REGULAR 또는, DIRECTORY), 실패(kuNOK)
     * \see    const kuInt32 getStat(const char *path, kuInt32 &type, kuInt64 &size)
     *
     * 입력 경로명 \a path 에 대해 그 유형을 반환한다. \n
     * 내부적으로 getStat()를 호출하여 얻은 유형을 반환한다.
     */
	static const kuInt32 type (const char *path);

	/**
     * \fn     const kuInt32 copy (const char *src, const char *dest)
     * \brief  파일 복사 하기
     * \param  src     원본 파일의 경로명
     * \param  dest    대상 파일의 경로명
     * \return 성공(kuOK), 실패(kuNOK)
     *
     * 원본 파일 \a src 의 내용으로 대상 파일 \a dest 를 생성한다. \n
     * 이는, 명령행에서의 cp 명령과 기능상 동일하다.
     */
	static const kuInt32 copy (const char *src, const char *dest);

	/**
     * \fn     const kuInt32 append (const char *src, const char *dest)
     * \brief  파일 내용 추가 하기
     * \param  src     원본 파일의 경로명
     * \param  dest    대상 파일의 경로명
     * \return 성공(kuOK), 실패(kuNOK)
     *
     * 원본 파일 \a src 의 내용을 대상 파일 \a dest 의 끝에 추가한다. \n
     */
	static const kuInt32 append (const char *src, const char *dest);

	/**
     * \fn     const kuInt32 remove (const char *path)
     * \brief  입력 경로 삭제 하기
     * \param  path     절대 또는, 상대 경로명
     * \return 성공(kuOK), 실패(kuNOK)
     *
     * 입력 경로 \a path 를 삭제 한다. 파일과 디렉토리 모두에 사용 가능하다.\n
     * 이는, 명령행에서의 rm 및 rmdir 명령과 기능상 동일하다.
     */
	static const kuInt32 remove (const char *path);

	/**
     * \fn     const kuInt32 move (const char *src, const char *dest)
     * \brief  경로명 변경 하기
     * \param  src     원래의 경로명
     * \param  dest    변경 될 경로명
     * \return 성공(kuOK), 실패(kuNOK)
     * \see    const kuInt32 rename(const char *src, const char *dest)
     *
     * 원본 경로명 \a src 를 대상 경로명 \a dest 로 변경한다. \n
     * 이는, 명령행에서의 mv 명령과 기능상 동일하다.
     * 내부적으로 rename()을 호출한다.
     */
	static const kuInt32 move (const char *src, const char *dest);

	/**
     * \fn     const kuInt32 rename (const char *src, const char *dest)
     * \brief  경로명 변경 하기
     * \param  src     원래의 경로명
     * \param  dest    변경 될 경로명
     * \return 성공(kuOK), 실패(kuNOK)
     * \see    const kuInt32 move(const char *src, const char *dest)
     *
     * 원본 경로명 \a src 를 대상 경로명 \a dest 로 변경한다. \n
     * 이는, 명령행에서의 mv 명령과 기능상 동일하다.
     */
	static const kuInt32 rename (const char *src, const char *dest);

	/**
     * \fn     const kuInt32 getStat (const char *path, kuInt32 &type, kuInt64 &size)
     * \brief  입력 경로에 대한 정보 얻기
     * \param  path     절대 또는, 상대 경로명
     * \param  type     유형 반환 (REGULAR 또는, DIRECTORY)
     * \param  size     크기 반환 (바이트 수)
     * \return 성공(kuOK), 실패(kuNOK)
     *
     * 입력 경로 \a path 에 대한 유형 및 크기 정보를 얻는다. \n
     * 내부적으로, stat() 함수를 호출한다.
     */
	static const kuInt32 getStat (const char *path, kuInt32 &type, kuInt64 &size);

	/**
     * \fn     char *convertPath (char *path)
     * \brief  경로명의 구분자를 내부 구분자인 '/' 로 변환
     * \param  path  내부 구분자로 변환 하고자 경로명 버퍼의 주소
     * \return 변환된 경로명 버퍼의 주소 반환. 오류 시는, NULL 반환
     * \see    static const char *convertPath(const char *src, char *dest)
     * 
     * 입력 받은 경로 \a path 의 경로 구분자를 내부 경로 구분자인 '/' 로 변환하여 반환한다. \n
     * 내부적으로, static const char *convertPath (const char *src, char *dest) 를 호출한다.
	 */
	static const char *convertPath (char *path);

	/**
     * \fn     char *convertPath (const char *src, char *dest)
     * \brief  경로명의 구분자를 내부 구분자인 '/' 로 변환
     * \param  src   내부 구분자로 변환 하고자 하는 원본 경로명 버퍼의 주소
     * \param  dest  내부 구분자로 변환된 경로를 저장할 대상 버퍼의 주소
     * \return 변환된 대상 버퍼의 주소 반환. 오류 시는, NULL 반환
     * 
     * 입력 받은 원본 경로 \a src 의 경로 구분자를 내부 경로 구분자인 '/' 로 변환한 뒤, 대상 경로 \a dest 에 복사하여 반환한다. \n
     * 기본적으로 Linux에서는 '/' 문자를, Windows에서는 '\' 문자를 경로 구분자로 사용하고 있다. \n
     * 상이한 운영체제 상에서 단일한 경로 구분자를 사용하기 위해 본 함수를 사용할 수 있다. \n
     * Windows 에서는 부분적으로 '/'을 지원하는 관계로, 내부 구분자를 Linux 형식인 '/' 로 가정한다.
     */
	static const char *convertPath (const char *src, char *dest);

	/**
     * \fn     char *convertPathToHost (char *path)
     * \brief  경로명의 구분자를 호스트 기본 구분자로 변환
     * \param  path  호스트 기본 구분자로 변환 하고자 경로명 버퍼의 주소
     * \return 변환된 경로명 버퍼의 주소 반환. 오류 시는, NULL 반환
     * \see    static const char *convertPathToHost(const char *src, char *dest)
     * 
     * 입력 받은 원본 경로 \a src 의 경로 구분자를 호스트 기본 경로 구분자로 변환한 뒤, 대상 경로 \a dest 에 복사하여 반환한다. \n
     * 내부적으로, static const char *convertPathToHost (const char *src, char *dest) 를 호출한다.
     */
	static const char *convertPathToHost (char *path);

	/**
     * \fn     char *convertPathToHost (const char *src, char *dest)
     * \brief  경로명의 구분자를 호스트 기본 구분자로 변환
     * \param  src   호스트 기본 구분자로 변환 하고자 하는 원본 경로명 버퍼의 주소
     * \param  dest  호스트 기본 구분자로 변환된 경로를 저장할 대상 버퍼의 주소
     * \return 변환된 대상 버퍼의 주소 반환. 오류 시는, NULL 반환
     * 
     * 입력 받은 원본 경로 \a src 의 경로 구분자를 호스트 기본 경로 구분자로 변환한 뒤, 대상 경로 \a dest 에 복사하여 반환한다. \n
     * 기본적으로 Linux에서는 '/' 문자를, Windows에서는 '\' 문자를 경로 구분자로 사용하고 있다.
     */
	static const char *convertPathToHost (const char *src, char *dest);

	/**
     * \fn     const kuInt32 getDirFileName (const char *path, char *pDirName, char *pFileName)
     * \brief  입력 경로명으로부터 디렉토리명과 파일명의 분리
     * \param  path      절대 또는, 상대 경로명
     * \param  pDirName  디렉토리명
     * \param  pFileName 파일명
     * \return 성공(kuOK), 실패(kuNOK)
     *
     * 입력 경로명 \a path 로부터 디렉토리명과 파일명을 분리한 뒤, \n
     * 디렉토리명은 \a pDirName 에 그리고, 파일명은 \a pFileName 에 저장하여 반환한다.\n
     */
	static const kuInt32 getDirFileName (const char *path, char *pDirName, char *pFileName);

	/**
     * \fn     const kuInt32 makeDir (const char *dirName, const kuBoolean fForced)
     * \brief  입력 경로의 디렉토리 생성
     * \param  dirName  절대 또는, 상대 경로명
     * \param  fForced  동일 이름의 파일에 대한 강제 삭제 여부
     * \return 성공(kuOK), 실패(kuNOK)
     *
     * 입력 경로 \a path 에 해당하는 디렉토리를 생성한다. \n
     * 만일 동일 이름의 파일이 존재하는 경우, 강제 삭제 여부 \a fForced 에 따라 삭제 후 생성한다. \n
	 * 이 때, fForced 가 kuTRUE 이면 동일 이름의 파일을 삭제한 뒤 디렉토리를 생성하고,
	 * fForced 가 kuFALSE 이면 오류 값인 kuNOK 를 반환한다.
     */
	static const kuInt32 makeDir (const char *dirName, const kuBoolean fForced = kuFALSE);

	/**
     * \fn     const kuInt32 makeRecurDir (const char *path)
     * \brief  입력 경로에 해당하는 모든 디렉토리 생성
     * \param  path     절대 또는, 상대 경로명
     * \return 성공(kuOK), 실패(kuNOK)
     * \see    makeDir
     *
     * 입력 경로명 \a path 가 없을 경우, 해당 디렉토리를 생성한다. \n
     * 만일 경로명이 여러 디렉토리로 이루어진 경우는, 해당하는 모든 디렉토리들을 일괄적으로 생성한다. \n
     * 내부적으로, makeDir() 을 호출한다. \n
     */
	static const kuInt32 makeRecurDir (const char *path);

	/**
     * \fn     const kuInt32 getLimits (const kuInt32 resource, struct rlimit &rp)
     * \brief  시스템의 자원에 대한 정보 얻기 (Linux Only)
     * \param  resource  얻고자 하는 시스템 자원의 상수값
     * \param  rp        시스템의 자원 정보를 반환하는 구조체
     * \return 성공(kuOK), 실패(kuNOK)
     *
     * 시스템 자원을 나타내는 상수값 \a resource 에 대하여 시스템 설정값을 얻어와 구조체 \a rp 에 반환한다. \n
     * Linux의 경우, 내부적으로 시스템의 getrlimit() 를 호출한다. \n
     * Windows 버전에서는, 지원 되지 않음. 추후 구현이 요구됨. \n
     */
	static const kuInt32 getLimits (const kuInt32 resource, struct rlimit &rp);

	/**
     * \fn     const kuInt64 getMaxFileSize ()
     * \brief  파일의 최대 크기 얻기
     * \return 성공(파일의 최대 크기), 실패(kuNOK)
     * \see    getLimits
     *
     * 시스템에서 지원 가능한 파일의 최대 크기를 반환한다. \n
     * Linux의 경우, 내부적으로 getLimits() 를 호출한다. \n
     * Windows의 경우, 현재 INT_MAX 값을 반환하며 추후 구현이 요구됨. \n
     */
	static const kuInt64 getMaxFileSize ();

	/**
     * \fn     const kuInt32 getMaxNumOpenFiles ()
     * \brief  하나의 프로그램에서 열 수 있는 파일의 최대 개수 얻기
     * \return 성공(파일의 최대 개수), 실패(kuNOK)
     * \see    getLimits
     *
     * 하나의 프로그램에서 파일을 열 수 있는 시스템 차원의 최대 개수를 반환한다. \n
     * Linux의 경우, 내부적으로 getLimits() 를 호출한다. \n
     * Windows의 경우, 현재 1024 값을 반환하며 추후 구현이 요구됨. \n
     */
	static const kuInt32 getMaxNumOpenFiles ();

	/**
     * \fn     const kuInt32 getCurrMaxNumOpenFiles ()
     * \brief  하나의 프로그램에서 실제로 열 수 있는 파일의 최대 개수 얻기
     * \return 성공(가능한 파일의 개수), 실패(kuNOK)
     * \see    getLimits
     *
     * 하나의 프로그램에서 실제로 열 수 있는 파일의 최대 개수를 반환한다. \n
     * 즉, 시스템 차원의 최대 개수와 프로그램에서 실제 사용가능한 최대 개수는 상이할 수 있다. \n
     * Linux의 경우, 내부적으로 getLimits() 를 호출한다. \n
     * Windows의 경우, 현재 0 값을 반환하며 추후 구현이 요구됨. \n
     */
	static const kuInt32 getCurrMaxNumOpenFiles ();

	/**
     * \fn     const kuInt32 setMaxNumOpenFiles (const kuInt32 num)
     * \brief  하나의 프로그램에서 실제로 열 수 있는 파일의 최대 개수 설정 하기
     * \param  num     설정값
     * \return 성공(kuOK), 실패(kuNOK)
     * \see    getMaxNumOpenFiles, getCurrMaxNumOpenFiles
     *
     * 하나의 프로그램에서 실제로 열 수 있는 파일의 최대 개수를 \a num 값으로 재설정한다. \n
     * 설정값은 getMaxNumOpenFiles()을 통해 얻은 값 이하를 지정하여야 한다. \n
     * Linux의 경우, 내부적으로 시스템의 setrlimit() 를 호출한다. \n
     * Windows 버전에서는, 지원 되지 않으며 항상 kuOK 를 반환한다. \n
     */
	static const kuInt32 setMaxNumOpenFiles (const kuInt32 num);

	/**
     * \fn     const kuInt32 getUsedUserTime (kuInt32 &sec, kuInt32 &usec)
     * \brief  사용자 모드로 실행된 CPU 사용 시간 얻기
     * \param  sec     CPU 사용시간 (초)
     * \param  usec    CPU 사용시간 (마이크로 초)
     * \return 성공(kuOK), 실패(kuNOK)
     *
     * 사용자 모드로 실행된 CPU 사용 시간을 초 \a sec 와 마이크로 초 \a usec 로 반환한다. \n
     * Linux의 경우, 내부적으로 getrusage() 를 호출한다. \n
     * Windows의 경우, 항상 0 을 반환하며 추후 구현이 요구됨. \n
     */
	static const kuInt32 getUsedUserTime (kuInt32 &sec, kuInt32 &usec);

	/**
     * \fn     const kuInt32 getUsedSysTime (kuInt32 &sec, kuInt32 &usec)
     * \brief  시스템(커널) 모드로 실행된 CPU 사용 시간 얻기
     * \param  sec     CPU 사용시간 (초)
     * \param  usec    CPU 사용시간 (마이크로 초)
     * \return 성공(kuOK), 실패(kuNOK)
     *
     * 시스템 (커널) 모드로 실행된 CPU 사용 시간을 초 \a sec 와 마이크로 초 \a usec 로 반환한다. \n
     * Linux의 경우, 내부적으로 getrusage() 를 호출한다. \n
     * Windows의 경우, 항상 0 을 반환하며 추후 구현이 요구됨. \n
     */
	static const kuInt32 getUsedSysTime (kuInt32 &sec, kuInt32 &usec);

	/**
     * \fn     const kuInt32 getUsedRate (const char *pTargetDir)
     * \brief  지정 파일 시스템의 디스크 사용률 얻기
     * \param  pTargetDir   파일 시스템의 절대 경로
     * \return 성공(디스크 사용률, 백분율 %), 실패(kuNOK)
     *
     * 입력 경로 \a pTargetDir 가 소속된 파일 시스템의 디스크 사용률(%)을 제공한다. \n
     * 경로명은 절대 경로를 사용하여야 하며, 해당 파일 시스템 내의 임의의 디렉토리를 사용할 수 있다. \n
     * Linux의 경우, 내부적으로 df 명령을 사용한다. \n
     */
	static const kuInt32 getUsedRate (const char *pTargetDir);

	/**
     * \fn     const kuInt32 getFileSystemInfo (const char *, kuInt64 &, kuInt64 &, kuInt64 &, epicsInt32 &)
     * \brief  지정 파일 시스템의 디스크 용량 및 사용률 얻기
     * \param  pTargetDir     파일 시스템의 절대 경로
     * \param  nTotSize       총 용량 (KB)
     * \param  nUsedSize      사용 용량 (KB)
     * \param  nAvailableSize 남은 용량 (KB)
     * \param  nUsedRate      사용률 (%)
     * \return 성공(kuOK), 실패(kuNOK)
     *
     * 입력 경로 \a pTargetDir 가 소속된 파일 시스템의 디스크 정보를 반환한다. \n
     * 디스크 총 용량 \a nTotSize, 사용량 \a nUsedSize, 남은 용량 \a nAvailableSize, 사용률(%) \a nUsedRate 을 제공한다. \n
     * 경로명은 절대 경로를 사용하여야 하며, 해당 파일 시스템 내의 임의의 디렉토리를 사용할 수 있다. \n
     * Linux의 경우, 내부적으로 df -k -P 명령을 사용한다. \n
     * Windows의 경우, 내부적으로 _getdiskfree 함수를 사용한다. \n
     */
	static const kuInt32 getFileSystemInfo (const char *pTargetDir,
						kuInt64 &nTotSize, kuInt64 &nUsedSize, kuInt64 &nAvailableSize, kuUInt32 &nUsedRate);

	/**
     * \fn     const kuInt32 isLock (const kuInt32 fd, const kuInt64 offset = 0, const kuInt64 size = 0)
     * \brief  파일에 록 설정 여부 확인
     * \param  fd       파일 디스크립터
     * \param  offset   파일 내 위치
     * \param  size     데이터 크기
     * \return kuTRUE(록 설정), kuFALSE(록 비설정)
     * \see    getLockStat
     *
     * 입력 파일 디스크립터 \a fd 로 열린 파일의 위치 \a offset 으로부터 크기 \a size 만큼의 데이터에 대해 \n
     * 읽기 또는, 쓰기 파일 록이 설정 되어 있는지의 여부를 반환한다. \n
     * \a offset 과 \a size 값이 0 이면, 파일 전체를 대상으로 한다.
     * 내부적으로, getLockStat() 을 호출한다. \n
     */
	static const kuInt32 isLock (const kuInt32 fd, const kuInt64 offset = 0, const kuInt64 size = 0);

	/**
     * \fn     const kuInt32 lock (const kuInt32 fd, const kuBoolean fBlocking = kuTRUE, const kuInt64 offset = 0, const kuInt64 size = 0)
     * \brief  파일에 록 설정 하기
     * \param  fd        록을 설정 하고자 하는 파일의 디스크립터
     * \param  fBlocking 이미 록이 설정 되어 있을 경우, 블록되어 기다릴지의 여부
     * \param  offset    록을 설정할 파일 내 시작 위치
     * \param  size      록을 설정할 파일 내 데이터의 크기
     * \return 성공(kuOK), 실패(kuNOK)
     * \see    unlock
     *
     * 입력 파일 디스크립터 \a fd 에 대해 시작 위치 \a offset 으로부터 크기 \a size 만큼의 데이터에 대해 파일 쓰기 록을 설정한다. \n
     * 만일, 다른 록이 사전 설정 되어 있는 상태에서 \a fBlocking 을 kuTRUE로 하면 해당 요청이 만족될 때까지 대기한다. \n
     * \a offset 과 \a size 값이 0 이면, 파일 전체를 대상으로 한다.
     */
	static const kuInt32 lock (const kuInt32 fd, const kuBoolean fBlocking = kuTRUE, const kuInt64 offset = 0, const kuInt64 size = 0);

	/**
     * \fn     const kuInt32 unlock (const kuInt32 fd, const kuInt64 offset = 0, const kuInt64 size = 0)
     * \brief  파일로부터 록 해제 하기
     * \param  fd        록을 해제 하고자 하는 파일의 디스크립터
     * \param  offset    록을 해제할 파일 내 시작 위치
     * \param  size      록을 해제할 파일 내 데이터의 크기
     * \return 성공(kuOK), 실패(kuNOK)
     * \see    lock
     *
     * 입력 파일 디스크립터 \a fd 에 대해 시작 위치 \a offset 으로부터 크기 \a size 만큼의 데이터에 대해 파일 쓰기 록을 해제한다. \n
     * \a offset 과 \a size 값이 0 이면, 파일 전체를 대상으로 한다.
     */
	static const kuInt32 unlock (const kuInt32 fd, const kuInt64 offset = 0, const kuInt64 size = 0);

	/**
     * \fn     const kuInt32 getLockStat (const kuInt32 fd, struct flock &lock, const kuInt64 offset = 0, const kuInt64 size = 0)
     * \brief  파일에 대한 록 상태 얻기
     * \param  fd        록 상태를 얻고자 하는 파일의 디스크립터
     * \param  lock      파일의 록 상태 정보 반환
     * \param  offset    록 상태를 얻고자 하는 파일 내 시작 위치
     * \param  size      록 상태를 얻고자 하는 파일 내 데이터의 크기
     * \return 성공(kuOK), 실패(kuNOK)
     *
     * 입력 파일 디스크립터 \a fd 에 대해 시작 위치 \a offset 으로부터 크기 \a size 만큼의 데이터에 대한 파일 록 정보를 얻는다. \n
     * \a offset 과 \a size 값이 0 이면, 파일 전체를 대상으로 한다.
     */
	static const kuInt32 getLockStat (const kuInt32 fd, struct flock &lock, const kuInt64 offset = 0, const kuInt64 size = 0);

private :

	static const kuInt32 write (const char *src, const char *dest, const char *mode);
	static const char *convertDelimiter (const char *src, char *dest, const kuUInt8 origin, const kuUInt8 newer);
};

#endif	// __cplusplus

// ---------------------------------------------------------------------------
// Function Declarations
// ---------------------------------------------------------------------------

#endif	// __KUTIL_FILE_H
