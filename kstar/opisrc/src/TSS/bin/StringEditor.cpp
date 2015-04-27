#include "StringEditor.h"

#include <string.h>


CStringEditor::CStringEditor(void)
{
}

CStringEditor::~CStringEditor(void)
{
}

char* CStringEditor::GetPathName(char* fullname)
{

	
	char* dirname = new char;
	char* pfilename = NULL;

	int len = (int)strlen(fullname);
	int len_file = 0;	
	
	pfilename = strrchr(fullname, '\\');
	len_file = (int)strlen(pfilename);

	int len_dir = len-len_file;
	
//	dirname = new char[len_dir +1];
//	dirname = NULL;
//	size_t test = sizeof(dirname);
//	int tmp = strlen(dirname);


//	dirname = (char*)malloc(sizeof(char) + 1);

	strcpy( dirname, "");
	strncat( dirname, fullname, len_dir+1 );  // 마지막에 "\" 를 붙이기위해 +1 한다. 
//	strncpy( dirname, fullname, len_dir+1 ); // 쓰레기가 붙어 크기가 이상해진다.

//	int tmp2 = (int)strlen(dirname);

	delete pfilename;

	return dirname;
}

char* CStringEditor::GetFileName(char* fullname)
{
	char* temp = NULL;
	char pfilename[64];
//	char filename[64];
	
	temp = strrchr(fullname, '\\');
	int len = (int)strlen(temp);

	for( int i=0; i<len; i++)
	{
		char key;
		key = temp[i + 1];
		
		pfilename[i] = key;
//		strcat(pfilename, (char*)key);
	}
	pfilename[len] = '\0';

//	strncat( pfilename, temp, len_dir+1 );  // 마지막에 "\" 를 붙이기위해 +1 한다. 
//	strcpy( filename, pfilename);

	return (char*)pfilename;
}


void CStringEditor::getPathFromFullName(char *FullFileName , char *PathName)
{
	char Str[512] , Temp[256];
	
	*Temp = NULL;

	strcpy(Str , FullFileName);

	int pathLength;
	for(pathLength = strlen(Str); --pathLength;)
	{
		if(Str[pathLength] == '/' || Str[pathLength] == '\\')
		{
			pathLength++;
			break;
		}
	}
	strncpy(Temp , Str , pathLength);
	
	if(pathLength > 0)
		Temp[pathLength++] = '\0';

	if( *Temp == NULL)
		strcpy(PathName, "none");
	else 
        strcpy(PathName , Temp);
}

void CStringEditor::getFileFromFullName(char *FullFileName , char *FileName)
{
	char* temp1 = NULL;
	char* temp2 = NULL;
	char* temp = NULL;
	int len, len1, len2;

	char pfilename[64];
	char filename[64];
	
	temp1 = strrchr(FullFileName, '\\');
	if( temp1 == NULL )
	{
		strcpy(FileName, "none");
		return;
	}		
	len1 = (int)strlen(temp1);

	temp2 = strrchr(FullFileName, '/');
	if( temp2 == NULL )
	{
		temp = temp1;
		len = len1;
	} else {
		int len2 = (int)strlen(temp2);

		if( len1 < len2 )
		{
			temp = temp1;
			len = len1;
		} else {
			temp = temp2;
			len = len2;
		}
	}


	for( int i=0; i<len; i++)
	{
		char key;
		key = temp[i + 1];
		
		pfilename[i] = key;
//		strcat(pfilename, (char*)key);
	}
	pfilename[len] = '\0';

//	strncat( pfilename, temp, len_dir+1 );  // 마지막에 "\" 를 붙이기위해 +1 한다. 
	strcpy( FileName, pfilename);
}

void CStringEditor::getStrBetweenQuot(char* inputbuf, char* str)
{
	int len = strlen(inputbuf);
	char temp[256];
	char source[512];

	strcpy(source, inputbuf);

	for(int i=0; i< len; i++){
		if( source[i] == '\"' ) {
			i++;
			for(int j=0; ;j++, i++){
				if( source[i] != '\"')	temp[j] = source[i];
				else{
					temp[j] = NULL;
					break;
				}
			}
			i++;
		}
	}

	strcpy(str, temp);

}

void CStringEditor::getStrAfterQuot(char* inputbuf, char* str)
{
	int i;
	int len = strlen(inputbuf);
	char temp[256];
	char source[512];
	int cnt=0;

	strcpy(source, inputbuf);

	for( i=0; i< len; i++){
		if( source[i] == '\"' )
			cnt++;

		if( cnt == 2) {
			i++; break;
		}
	}

	for(int j=0; j< len; i++, j++ ) {
		temp[j] = source[i];
	}
	

	strcpy(str, temp);
}


