#ifndef TYPES
#define TYPES

#include "extern\\uthash.h"

typedef struct
{
	char *dirName;
	char *dirPath;
	UT_hash_handle hh;
} DirectoryItem;

typedef struct
{
	char *fileName;
	char *filePath;
	unsigned long long byteSize;
	unsigned int lastWrite;
	UT_hash_handle hh;
} FileItem;

typedef struct
{
	FileItem *fi;
	DirectoryItem *di;
} HTable;

typedef struct
{
	unsigned long long totalSize;
	unsigned int fileCount, dirCount;
	FileItem **fileItems;
	DirectoryItem **dirItems;
	HTable hTable;
	char *root, **ignoreList;
} Traverser;

typedef struct
{
	Traverser *traverser;
	char *savePath;
} ThreadParams;


#endif