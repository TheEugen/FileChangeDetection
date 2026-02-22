#ifndef UTILS
#define UTILS

#pragma warning (disable : 4996)

#include <stdio.h>
#include <windows.h>
#include "types.h"
#include "traverse.h"

#define DEBUG_LOG(msg) printf("DEBUG_LOG: %s\n", msg);
#define iDEBUG_LOG(msg, n) printf("DEBUG_LOG: %s%d\n", msg, n);
#define i64DEBUG_LOG(msg, n) printf("DEBUG_LOG: %s%I64d\n", msg, n);
#define ptrDEBUG_LOG(msg, ptr) printf("DEBUG_LOG: %s ptr: %p\n", msg, ptr);

#define BUFSIZE 520

/*********** utils_comparing ***********/

void compare(Traverser * traverser_new, Traverser * traverser_old, const unsigned char showDifferent, const char * const writeDifferentPath);

void startComparing(const char * const readPath, Traverser * traverser_new, Traverser * traverser_old, const unsigned char showDifferent,
	const unsigned char compareOnly, const char * const readPathNew, const char * const writeDifferentPath);

/***************************************/
/************ utils_file_io ************/

void readFromDisk(const char * const readPath, Traverser * traverser);

DWORD WINAPI writeToDiskT(LPVOID lpParameter);

void writeToDisk(const char * const writePath, Traverser * traverser, const unsigned char freePath);

void calculateFileAndDirCountFromDisk(const char * const readPath, unsigned int * fileCount, unsigned int * dirCount);

/***************************************/
/********** utils_stat_output **********/

void printHTable(HTable * hTable);

void printDIArray(DirectoryItem * const dirItems[], const unsigned int dirCount);

void printFIArray(FileItem * const fileItems[], const unsigned int fileCount);

void outputStats(Traverser * const traverser);

/***************************************/
/*********** utils_fileitem ************/

unsigned long long constructByteSize(const unsigned int fileSizeLow, const unsigned int fileSizeHigh);

unsigned long constructWriteTime(const FILETIME fileTime);

/***************************************/
/*********** utils_traversing **********/

unsigned char startTraverse(const char * const root, const char * const savePath, Traverser * traverser, const unsigned char write,
	const unsigned char thread, HANDLE * hThread);

unsigned char validateDirectory(const char * const directory);

/***************************************/
/************* utils_res ***************/

void initTraverser(Traverser * const traverser);

void initTraverserWithRoot(Traverser * const traverser, const char * const root);

Traverser * allocTraverser();

void releaseRes(Traverser * const traverser, const unsigned char releaseRoot);

/***************************************/


#endif