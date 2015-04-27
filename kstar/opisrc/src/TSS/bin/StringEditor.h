#pragma once

#include <stdio.h>

class CStringEditor
{
public:
	CStringEditor(void);
	~CStringEditor(void);

	char* GetPathName(char* fullname);
	char* GetFileName(char* fullname);

	void getPathFromFullName(char* FullFileName , char* PathName);
	void getFileFromFullName(char* FullFileName , char* FileName);

	void getStrBetweenQuot(char* inputbuf, char* str);
	void getStrAfterQuot(char* inputbuf, char* str);

};

