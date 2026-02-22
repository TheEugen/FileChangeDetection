#ifndef TRAVERSE
#define TRAVERSE

#pragma warning (disable : 4996)

#include <stdio.h>
#include <windows.h>

#include "types.h"
#include "utils.h"

#define BUFSIZE 520

//#ifdef _MSC_VER
void convertWCharToString(char * buf, WCHAR * wchar);

void resetFileInts();

void printFileInts();

void incFileInts(unsigned int filemask);
//#endif

void calculateFileAndDirCount(const char * const inputPath, HANDLE * fHandle, WIN32_FIND_DATA * fData, unsigned int * const fileCount,
	unsigned int * const dirCount);

void traverse(const char * const inputPath, HANDLE * fHandle, WIN32_FIND_DATA * fData, FileItem * fileItems[], DirectoryItem * dirItems[],
	unsigned long long * const totalSize, unsigned int * const fileIndex, unsigned int * const dirIndex, HTable * hTable);

#endif