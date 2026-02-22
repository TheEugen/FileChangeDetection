#include "utils.h"


/*********** utils_comparing ***********/

void compare(Traverser * traverser_new, Traverser * traverser_old, const unsigned char showDifferent, const char * const writeDifferentPath)
{
	// check if comparable
	if( strcmp(traverser_new->root, traverser_old->root) != 0 )
	{
		printf("Comparing: unequal roots %s and %s, aborting..\n\n", traverser_new->root, traverser_old->root);
		return;
	}

	long long totalSize_diff;
	int fileCount_diff, dirCount_diff;
	unsigned char *foundArray_new_fi, *foundArray_old_fi, *foundArray_new_di, *foundArray_old_di;
	unsigned long long newFileBytes = 0, deletedFileBytes = 0;
	unsigned int newFileCount = 0, deletedFileCount = 0, newDirCount = 0, deletedDirCount = 0;
	FILE *writeDifferentFile = NULL;

	if( writeDifferentPath )
	{
		writeDifferentFile = fopen(writeDifferentPath, "ab");
		if( !writeDifferentFile )
		{
			fprintf(stderr, "compare writeDifferentFile ptr NULL\n");
			return;
		}
	}

	printf("Comparing %s..\n", traverser_new->root);
	
	totalSize_diff = traverser_new->totalSize - traverser_old->totalSize;
	fileCount_diff = traverser_new->fileCount - traverser_old->fileCount;
	dirCount_diff = traverser_new->dirCount - traverser_old->dirCount;

	//	FILEITEMS

	foundArray_new_fi = malloc(traverser_new->fileCount);
	if( !foundArray_new_fi )
	{
		fprintf(stderr, "compare foundArray_new ptr NULL\n");
		return;
	}
	memset(foundArray_new_fi, 0, traverser_new->fileCount);

	foundArray_old_fi = malloc(traverser_old->fileCount);
	if( !foundArray_old_fi )
	{
		fprintf(stderr, "compare foundArray_old ptr NULL\n");
		return;
	}
	memset(foundArray_old_fi, 0, traverser_old->fileCount);

	// find traverser_new fileitems
	FileItem *fi_tmp = NULL;
	for(int i = 0; i < traverser_new->fileCount; ++i)
	{
		HASH_FIND_STR(traverser_old->hTable.fi, traverser_new->fileItems[i]->filePath, fi_tmp);
		if( fi_tmp )
			foundArray_new_fi[i] = 1;
	}
	fi_tmp = NULL;

	// find traverser_old fileitems
	for(int i = 0; i < traverser_old->fileCount; ++i)
	{
		HASH_FIND_STR(traverser_new->hTable.fi, traverser_old->fileItems[i]->filePath, fi_tmp);
		if( fi_tmp )
			foundArray_old_fi[i] = 1;
	}
	fi_tmp = NULL;

	// DIRECTORYITEMS

	foundArray_new_di = malloc(traverser_new->dirCount);
	if( !foundArray_new_di )
	{
		fprintf(stderr, "compare foundArray_new ptr NULL\n");
		return;
	}
	memset(foundArray_new_di, 0, traverser_new->dirCount);

	foundArray_old_di = malloc(traverser_old->dirCount);
	if( !foundArray_old_di )
	{
		fprintf(stderr, "compare foundArray_old ptr NULL\n");
		return;
	}
	memset(foundArray_old_di, 0, traverser_old->dirCount);

	// find traverser_new diritems
	DirectoryItem *di_tmp = NULL;
	for(int i = 0; i < traverser_new->dirCount; ++i)
	{
		HASH_FIND_STR(traverser_old->hTable.di, traverser_new->dirItems[i]->dirPath, di_tmp);
		if( di_tmp )
			foundArray_new_di[i] = 1;
	}
	di_tmp = NULL;

	// find traverser_old diritems
	for(int i = 0; i < traverser_old->dirCount; ++i)
	{
		HASH_FIND_STR(traverser_new->hTable.di, traverser_old->dirItems[i]->dirPath, di_tmp);
		if( di_tmp )
			foundArray_old_di[i] = 1;
	}
	di_tmp = NULL;

	// file output
	if( writeDifferentFile )
		fputs("New files:\n", writeDifferentFile);

	for(int i = 0; i < traverser_new->fileCount; ++i)
	{
		if( !foundArray_new_fi[i] )
		{
			if( showDifferent )
				printf("New file #%d: %s\n", ++newFileCount, traverser_new->fileItems[i]->filePath);
			else
				++newFileCount;		

			if( writeDifferentFile )
			{
				fputs(traverser_new->fileItems[i]->filePath, writeDifferentFile);
				fputs("\n", writeDifferentFile);
			}

			newFileBytes += traverser_new->fileItems[i]->byteSize;
		}

	}

	if( showDifferent )
		printf("\n");

	if( writeDifferentFile )
		fputs("Deleted files:\n", writeDifferentFile);

	for(int i = 0; i < traverser_old->fileCount; ++i)
	{
		if( !foundArray_old_fi[i] )
		{
			if( showDifferent )
				printf("Deleted file #%d: %s\n", ++deletedFileCount, traverser_old->fileItems[i]->filePath);
			else
				++deletedFileCount;

			if( writeDifferentFile )
			{
				fputs(traverser_old->fileItems[i]->filePath, writeDifferentFile);
				fputs("\n", writeDifferentFile);
			}

			deletedFileBytes += traverser_old->fileItems[i]->byteSize;
		}
	}

	// directory output
	for(int i = 0; i < traverser_new->dirCount; ++i)
	{
		if( !foundArray_new_di[i] )
		{
			if( showDifferent )
				printf("New directory #%d: %s\n", ++newDirCount, traverser_new->dirItems[i]->dirPath);
			else
				++newDirCount;

			if( writeDifferentFile )
			{
				fputs(traverser_new->dirItems[i]->dirPath, writeDifferentFile);
				fputs("\n", writeDifferentFile);
			}
		}

	}
	if( showDifferent )
		printf("\n");
	for(int i = 0; i < traverser_old->dirCount; ++i)
	{
		if( !foundArray_old_di[i] )
		{
			if( showDifferent )
				printf("Deleted directory #%d: %s\n", ++deletedDirCount, traverser_old->dirItems[i]->dirPath);
			else
				++deletedDirCount;

			if( writeDifferentFile )
			{
				fputs(traverser_old->dirItems[i]->dirPath, writeDifferentFile);
				fputs("\n", writeDifferentFile);
			}
		}
	}

	printf("\nTotalsize difference: ");
	if( totalSize_diff > 0 )
		printf("+");
	printf("%I64d bytes\n", totalSize_diff);
	printf("File difference: ");
	if( fileCount_diff > 0 )
		printf("+");
	printf("%d\n", fileCount_diff);
	printf("Directory difference: ");
	if( dirCount_diff > 0 )
		printf("+");
	printf("%d\n", dirCount_diff);

	printf("New files: %d (+%I64d Bytes)\n", newFileCount, newFileBytes);
	printf("Deleted files: %d (-%I64d Bytes)\n\n", deletedFileCount, deletedFileBytes);

	// cleanup
	if( writeDifferentFile )
		fclose(writeDifferentFile);

	free(foundArray_new_fi);
	free(foundArray_old_fi);
	free(foundArray_new_di);
	free(foundArray_old_di);
}

void startComparing(const char * const readPath, Traverser * traverser_new, Traverser * traverser_old, const unsigned char showDifferent,
	const unsigned char compareOnly, const char * const readPathNew, const char * const writeDifferentPath)
{
	// init traverser_in struct
	initTraverser(traverser_old);

	unsigned int fileCount = 0, dirCount = 0;
	calculateFileAndDirCountFromDisk(readPath, &fileCount, &dirCount);
	traverser_old->fileItems = malloc(sizeof(FileItem*) * fileCount);
	traverser_old->dirItems = malloc(sizeof(DirectoryItem*) * dirCount);

	if( compareOnly )
	{
		fileCount = 0;
		dirCount = 0;

		initTraverser(traverser_new);

		calculateFileAndDirCountFromDisk(readPathNew, &fileCount, &dirCount);
		traverser_new->fileItems = malloc(sizeof(FileItem*) * fileCount);
		traverser_new->dirItems = malloc(sizeof(DirectoryItem*) * dirCount);

		readFromDisk(readPathNew, traverser_new);
	}

	// read from readPath and save fileitem array
	readFromDisk(readPath, traverser_old);

	// output stats
	if ( !compareOnly )
		outputStats(traverser_old);

	// compare
	compare(traverser_new, traverser_old, showDifferent, writeDifferentPath);
}

/*****************************************/

/************** utils_file_io*************/

void readFromDisk(const char * const readPath, Traverser * traverser)
{
	char bufName[BUFSIZE], bufPath[BUFSIZE], bufRoot[BUFSIZE], c = 1;
	size_t r = 0;
	unsigned long long byteSize = 0;
	unsigned int lastWrite = 0, bufIndex = 0, fileCount = 0;

	printf("Trying to read from %s..\n", readPath);

	FILE *inFile;
	inFile = fopen(readPath, "rb");

	if( !inFile )
	{
		fprintf(stderr, "readFromDisk inFile ptr NULL\n");
		return;
	}

	// pattern: root | totalsize | filecount | fileitems | dircount | diritems

	// read root string
	while( c )
	{
		r = fread(&c, sizeof(char), 1, inFile);
		bufRoot[bufIndex++] = c;
	}
	c = 1;
	bufIndex = 0;

	char *root = malloc(strlen(bufRoot) + 1);
	if( !root )
	{
		fprintf(stderr, "readfromdisk root ptr NULL\n");
		return;
	}
	strcpy(root, bufRoot);
	traverser->root = root;

	// read totalsize
	r = fread(&traverser->totalSize, sizeof(unsigned long long), 1, inFile);

	// read filecount
	r = fread(&traverser->fileCount, sizeof(unsigned int), 1, inFile);

	// read fileitems
	for(int i = 0; i < traverser->fileCount; ++i)
	{
		// read filename string
		while( c )
		{
			r = fread(&c, sizeof(char), 1, inFile);
			bufName[bufIndex++] = c;
		}
		c = 1;
		bufIndex = 0;

		// read filepath string
		while( c )
		{
			r = fread(&c, sizeof(char), 1, inFile);
			bufPath[bufIndex++] = c;
		}
		c = 1;
		bufIndex = 0;

		// read bytesize
		r = fread(&byteSize, sizeof(unsigned long long), 1, inFile);

		// read lastwrite
		r = fread(&lastWrite, sizeof(unsigned int), 1, inFile);

		// request memory for strings
		char *name = malloc(strlen(bufName) + 1);
		if( !name )
		{
			fprintf(stderr, "readFromDisk name ptr NULL\n");
			return;
		}
		char *path = malloc(strlen(bufPath) + 1);
		if( !path )
		{
			fprintf(stderr, "readFromDisk path ptr NULL\n");
			return;
		}

		// copy names in final location
		strcpy(name, bufName);
		strcpy(path, bufPath);

		// create FileItem
		FileItem *fileObj = malloc(sizeof(FileItem));
		if( !fileObj )
		{
			fprintf(stderr, "readFromDisk fileObj ptr NULL\n");
			return;
		}
		*fileObj = (FileItem) { name, path, byteSize, lastWrite };	
		traverser->fileItems[i] = fileObj;

		HASH_ADD_KEYPTR(hh, traverser->hTable.fi, fileObj->filePath, strlen(fileObj->filePath), fileObj );
	}


	// read dircount
	r = fread(&traverser->dirCount, sizeof(unsigned int), 1, inFile);

	// read diritems
	for(int i = 0; i < traverser->dirCount; ++i)
	{
		// read dirname string
		while( c )
		{
			r = fread(&c, sizeof(char), 1, inFile);
			bufName[bufIndex++] = c;
		}
		c = 1;
		bufIndex = 0;

		// read dirpath string
		while( c )
		{
			r = fread(&c, sizeof(char), 1, inFile);
			bufPath[bufIndex++] = c;
		}
		c = 1;
		bufIndex = 0;

		// request memory for strings
		char *dirName = malloc(strlen(bufName) + 1);
		if( !dirName )
		{
			fprintf(stderr, "readFromDisk name ptr NULL\n");
			return;
		}
		char *dirPath = malloc(strlen(bufPath) + 1);
		if( !dirPath )
		{
			fprintf(stderr, "readFromDisk path ptr NULL\n");
			return;
		}

		// copy names in final location
		strcpy(dirName, bufName);
		strcpy(dirPath, bufPath);

		// create DirectoryItem
		DirectoryItem *dirObj = malloc(sizeof(DirectoryItem));
		if( !dirObj )
		{
			fprintf(stderr, "readFromDisk dirObj ptr NULL\n");
			return;
		}

		*dirObj = (DirectoryItem) { dirName, dirPath };
		traverser->dirItems[i] = dirObj;

		HASH_ADD_KEYPTR(hh, traverser->hTable.di, dirObj->dirPath, strlen(dirObj->dirPath), dirObj);
	}
}

DWORD WINAPI writeToDiskT(LPVOID lpParameter)
{
	ThreadParams *tParams = (ThreadParams*) lpParameter;

	writeToDisk(tParams->savePath, tParams->traverser, 1);

	return 1;
}

void writeToDisk(const char * const writePath, Traverser * traverser, const unsigned char freePath)
{
	printf("Writing %d fileitems and %d directoryitems to %s\n\n", traverser->fileCount, traverser->dirCount, writePath);

	FILE *outFile;
	outFile = fopen(writePath, "wb");

	if( !outFile )
	{
		fprintf(stderr, "writeToDisk outFile ptr NULL\n");
		return;
	}

	// pattern: root | totalsize | filecount | fileitems | dircount | diritems

	size_t r = 0;

	// write root string
	r = fwrite(traverser->root, strlen(traverser->root) + 1, 1, outFile);

	// write totalsize
	r = fwrite(&traverser->totalSize, sizeof(unsigned long long), 1, outFile);

	// write filecount
	r = fwrite(&traverser->fileCount, sizeof(unsigned int), 1, outFile);

	// write fileitems
	for(int i = 0; i < traverser->fileCount; ++i)
	{
		r = fwrite(traverser->fileItems[i]->fileName, strlen(traverser->fileItems[i]->fileName) + 1, 1, outFile);
		r = fwrite(traverser->fileItems[i]->filePath, strlen(traverser->fileItems[i]->filePath) + 1, 1, outFile);
		r = fwrite(&traverser->fileItems[i]->byteSize, sizeof(unsigned long long), 1, outFile);
		r = fwrite(&traverser->fileItems[i]->lastWrite, sizeof(unsigned int), 1, outFile);
	}

	// write dircount
	r = fwrite(&traverser->dirCount, sizeof(unsigned int), 1, outFile);

	// write diritems
	for(int i = 0; i < traverser->dirCount; ++i)
	{
		r = fwrite(traverser->dirItems[i]->dirName, strlen(traverser->dirItems[i]->dirName) + 1, 1, outFile);
		r = fwrite(traverser->dirItems[i]->dirPath, strlen(traverser->dirItems[i]->dirPath) + 1, 1, outFile);
	}

	if( r == 0)
		fprintf(stderr, "Failed to write fileitems array");

	if( freePath )
		//free(writePath);

	fclose(outFile);
}

void calculateFileAndDirCountFromDisk(const char * const readPath, unsigned int * fileCount, unsigned int * dirCount)
{
	char c = 1;
	size_t r = 0;
	unsigned long long byteSize = 0, intBuf = 0;
	unsigned int lastWrite = 0;

	FILE *inFile;
	inFile = fopen(readPath, "rb");

	if( !inFile )
	{
		fprintf(stderr, "calculateFileAndDirCountFromDisk inFile ptr NULL\n");
		return;
	}

	// pattern: root | totalsize | filecount | fileitems | dircount | diritems

	// read root
	while( c )
		r = fread(&c, sizeof(char), 1, inFile);
	c = 1;

	// read totalSize
	r = fread(&intBuf, sizeof(unsigned long long), 1, inFile);

	// read fileCount
	r = fread(&intBuf, sizeof(unsigned int), 1, inFile);
	*fileCount = intBuf;

	// read fileItems
	for(int i = 0; i < *fileCount; ++i)
	{
		// read filename string
		while( c )
			r = fread(&c, sizeof(char), 1, inFile);		
		c = 1;

		// read filepath string
		while( c )
			r = fread(&c, sizeof(char), 1, inFile);
		c = 1;

		// bytesize and lastwrite
		r = fread(&byteSize, sizeof(unsigned long long), 1, inFile);
		r = fread(&lastWrite, sizeof(unsigned int), 1, inFile);
	}

	// read dirCount
	r = fread(&intBuf, sizeof(unsigned int), 1, inFile);
	*dirCount = intBuf;

	// read dirItems
	for(int i = 0; i < *dirCount; ++i)
	{
		while( c )
			r = fread(&c, sizeof(char), 1, inFile);		
		c = 1;

		// read filepath string
		while( c )
			r = fread(&c, sizeof(char), 1, inFile);
		c = 1;
	}

}

/*****************************************/

/*********** utils_stat_output ***********/

void printHTable(HTable * hTable)
{
	DirectoryItem *dirObj = malloc(sizeof(DirectoryItem)), *tmp_d = NULL;
	FileItem *fileObj = malloc(sizeof(FileItem)), *tmp_f = NULL;
	HASH_ITER(hh, hTable->di, dirObj, tmp_d)
	{
		printf(dirObj->dirPath);
		printf("\n");
	}
	HASH_ITER(hh, hTable->fi, fileObj, tmp_f)
	{
		printf(fileObj->filePath);
		printf("\n");
	}

	if( dirObj )
		free(dirObj);
	if( fileObj )
		free(fileObj);
	if( tmp_d )
		free(tmp_d);
	if( tmp_f )
		free(tmp_f);
}

void printDIArray(DirectoryItem * const dirItems[], const unsigned int dirCount)
{
	printf("\n");
	for(int i = 0; i < dirCount; ++i)
		printf("Directory #%d %s\n", i + 1, dirItems[i]->dirName);
}

void printFIArray(FileItem * const fileItems[], const unsigned int fileCount)
{
	printf("\n");
	for(int i = 0; i < fileCount; ++i)
		printf("File #%d %s\n", i + 1, fileItems[i]->fileName);
}

void outputStats(Traverser * const traverser)
{
	//printFIArray(traverser->fileItems, traverser->fileCount);
	//printDIArray(traverser->dirItems, traverser->dirCount);
	printf("\nStats for %s\n\n", traverser->root);
	printf("File Count: %d\n", traverser->fileCount);
	printf("Directory Count: %d\n", traverser->dirCount);
	printf("Total Size: %I64d Bytes\n", traverser->totalSize);
	printf("Total Size: %.2f GBytes\n\n", (float) traverser->totalSize / (1024 * 1024 * 1024));
}

/***************************************/

/*********** utils_fileitem ************/

unsigned long long constructByteSize(const unsigned int fileSizeLow, const unsigned int fileSizeHigh)
{
	return (unsigned long long)(fileSizeLow + ((unsigned long long)fileSizeHigh << 32));
}

unsigned long constructWriteTime(const FILETIME fileTime)
{
	SYSTEMTIME sTime;
	unsigned long result;

	if( !FileTimeToSystemTime(&fileTime, &sTime) )
		printf("Failed while constructing last writetime of file");

	// constructing an positive number from SYSTEMTIME
	result = 1;

	return result;
}

/***************************************/

/*********** utils_traversing ***********/

unsigned char startTraverse(const char * const root, const char * const savePath, Traverser * traverser, const unsigned char write,
	const unsigned char thread, HANDLE * hThread)
{
	WIN32_FIND_DATA fData, fData_2;
	HANDLE fHandle = NULL;

	unsigned int fileIndex = 0, dirIndex = 0;

	initTraverserWithRoot(traverser, root);

	calculateFileAndDirCount(root, &fHandle, &fData, &traverser->fileCount, &traverser->dirCount);

	//printFileInts();
	//resetFileInts();

	traverser->fileItems = malloc(sizeof(FileItem*) * traverser->fileCount);
	if( !traverser->fileItems )
	{
		fprintf(stderr, "startTraverse fileItems ptr NULL\n");
		return 0;
	}

	traverser->dirItems = malloc(sizeof(DirectoryItem*) * traverser->dirCount);
	if( !traverser->dirItems )
	{
		fprintf(stderr, "startTraverse dirItems ptr NULL\n");
		return 0;
	}

	fHandle = NULL;

	// start directory traverse
	printf("Traversing %s..\n", root);
	traverse(root, &fHandle, &fData_2, traverser->fileItems, traverser->dirItems, &traverser->totalSize, &fileIndex, &dirIndex, &traverser->hTable);

	//printFileInts();
	//resetFileInts();

	//iDEBUG_LOG("fileindex ", fileIndex);
	//iDEBUG_LOG("filecount ", traverser->fileCount);
	//iDEBUG_LOG("dirindex ", dirIndex);
	//iDEBUG_LOG("dircount ", traverser->dirCount);

	//printHTable(&traverser->hTable);

	// output stats
	outputStats(traverser);

	// write fileitem array to savePath
	if( write )
	{
		if( thread )
		{
			DWORD threadId;

			ThreadParams *tParams = (ThreadParams*) HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(ThreadParams));
			
			//ThreadParams *tParams = malloc(sizeof(ThreadParams));
			if( !tParams )
			{
				fprintf(stderr, "startTraverse tParams ptr NULL\n");
				return 0;
			}

			tParams->savePath = malloc(sizeof(strlen(savePath) + 1));
			if( !tParams->savePath )
			{
				fprintf(stderr, "startTraverse tParams->savePath ptr NULL\n");
				return 0;
			}

			strcpy(tParams->savePath, savePath);
			tParams->traverser = traverser;

			hThread = CreateThread(NULL, 0, writeToDiskT, tParams, 0, &threadId);
		}
		else
			writeToDisk(savePath, traverser, 0);
	}


	// cleanup
	if( fHandle )
		FindClose(fHandle);

	return 1;
}

unsigned char validateDirectory(const char * const directory)
{
	WIN32_FIND_DATA fData;
	HANDLE fHandle = NULL;

	char *filemask = malloc(strlen(directory) + 5);
	if( !filemask )
	{
		fprintf(stderr, "validateDirectory filemask ptr NULL\n");
		return 0;
	}

	sprintf(filemask, "%s\\*.*", directory);

#ifdef __GNUC__
	fHandle = FindFirstFile(filemask , &fData);
#elif defined _MSC_VER
	fHandle = FindFirstFileA(filemask , &fData);
#endif
	if( fHandle == INVALID_HANDLE_VALUE || !(fData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) )
	{
		printf("%s is not a valid directory, aborting..\n", directory);
		return 0;
	} 

	FindClose(fHandle);
	free(filemask);

	return 1;
}

/******************************/

/********** utils_res *********/

void initTraverser(Traverser * const traverser)
{
	traverser->totalSize = 0;
	traverser->fileCount = 0;
	traverser->dirCount = 0;

	traverser->hTable.di = NULL;
	traverser->hTable.fi = NULL;
}

void initTraverserWithRoot(Traverser * const traverser, const char * const root)
{
	initTraverser(traverser);

	traverser->root = malloc(strlen(root) + 1);
	if( !traverser->root )
	{
		fprintf(stderr, "initTraverserWithRoot traverser->root ptr NULL\n");
		return;
	}
	strcpy(traverser->root, root);
}

Traverser * allocTraverser()
{
	Traverser *traverser = malloc(sizeof(Traverser));
	if( !traverser )
	{
		fprintf(stderr, "allocTraverser traverser ptr NULL\n");
		return NULL;
	}
	return traverser;
}

void releaseRes(Traverser * const traverser, const unsigned char releaseRoot)
{
	if( !traverser )
	{
		fprintf(stderr, "releaseRes traverser ptr NULL\n");
		return;
	}

	for(int i = 0; i < traverser->fileCount; ++i)
	{
		free(traverser->fileItems[i]->fileName);
		free(traverser->fileItems[i]->filePath);
		free(traverser->fileItems[i]);
	}
	free(traverser->fileItems);

	for(int i = 0; i < traverser->dirCount; ++i)
	{
		free(traverser->dirItems[i]->dirName);
		free(traverser->dirItems[i]->dirPath);
		free(traverser->dirItems[i]);
	}
	free(traverser->dirItems);

	if( releaseRoot )
		free(traverser->root);

	free(traverser);
	
	printf("Released resources\n");
}

/******************************/