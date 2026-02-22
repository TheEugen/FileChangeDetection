#include "traverse.h"


#ifdef __GNUC__

void calculateFileAndDirCount(const char * const inputPath, HANDLE * fHandle, WIN32_FIND_DATA * fData, unsigned int * const fileCount,
	unsigned int * const dirCount)
{
	if( strlen(inputPath) > 253 )
	{
		fprintf(stderr, "given path is too long, aborting..");
		exit(0);
	}

	char sPath[BUFSIZE];

	// specify file mask
	sprintf(sPath, "%s\\*.*", inputPath);

	fHandle = FindFirstFile(sPath, fData);
	if( fHandle == INVALID_HANDLE_VALUE )
	{
		//fprintf(stderr, "%s is not a valid directory, aborting..\n", inputPath);
		return;
	} 

	do
	{	
		// FindFirstFile always returns . and .. first
		if( strcmp(fData->cFileName, ".") != 0 && strcmp(fData->cFileName, "..") != 0 )
		{
			// check if inputPath is directory or file
			if( fData->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
			{
				++*dirCount;

				// concatenate filepath
				sprintf(sPath, "%s\\%s", inputPath, fData->cFileName);

				//recursion
				calculateFileAndDirCount(sPath, fHandle, fData, fileCount, dirCount);	
			}					         
			else
				++*fileCount;			
		}
	} while( FindNextFile(fHandle, fData) );	
}

void traverse(const char * const inputPath, HANDLE * fHandle, WIN32_FIND_DATA * fData, FileItem * fileItems[], DirectoryItem * dirItems[],
	unsigned long long * const totalSize, unsigned int * const fileIndex, unsigned int * const dirIndex, HTable * hTable)
{
	if( strlen(inputPath) > 253 )
	{
		fprintf(stderr, "given path is too long, aborting..");
		exit(0);
	}

	char sPath[BUFSIZE];

	// specify file mask
	sprintf(sPath, "%s\\*.*", inputPath);

	// check if sPath is a directory
	fHandle = FindFirstFile(sPath, fData);
	if( fHandle == INVALID_HANDLE_VALUE )
	{
		//fprintf(stderr, "%s is not a valid directory, aborting..\n", inputPath);
		return;
	}   

	do
	{
		// FindFirstFile always returns . and .. first
		if( strcmp(fData->cFileName, ".") != 0 && strcmp(fData->cFileName, "..") != 0 )
		{
			// concatenate filepath
			sprintf(sPath, "%s\\%s", inputPath, fData->cFileName); 

			// check if inputPath is directory or file
			if( fData->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
			{       
				// output directory
				//printf("Directory: %s\n", sPath);

				DirectoryItem *dirObj = malloc(sizeof(DirectoryItem));
				if( !dirObj )
				{
					fprintf(stderr, "traverse dirObj ptr NULL\n");
					return;
				}

				char *dirName = malloc(strlen(fData->cFileName) + 1); 
				if( !dirName )
				{
					fprintf(stderr, "traverse dirName ptr NULL\n");
					return;
				}

				char *dirPath = malloc(strlen(sPath) + 1);
				if( !dirPath )
				{
					fprintf(stderr, "traverse dirPath ptr NULL\n");
					return;
				}

				// copy strings to final location
				strcpy(dirName, fData->cFileName);
				strcpy(dirPath, sPath);

				// create directoryitem
				*dirObj = (DirectoryItem) { dirName, dirPath };
				dirItems[*dirIndex] = dirObj;

				HASH_ADD_KEYPTR( hh, hTable->di, dirObj->dirPath, strlen(dirObj->dirPath), dirObj );

				//printf("Dir #%d  %s\n", *dirIndex + 1, dirName);

				++*dirIndex;

				// recursion
				traverse(sPath, fHandle, fData, fileItems, dirItems, totalSize, fileIndex, dirIndex, hTable);
			}
			else
			{
				// output file
				//printf("File: %s\n", sPath);

				// request memory for new fileitem
				FileItem *fileObj = malloc(sizeof(FileItem));
				if( !fileObj )
				{
					fprintf(stderr, "fileObj NULL");
					return;
				}

				char *fileName = malloc(strlen(fData->cFileName) + 1);
				if( !fileName )
				{
					fprintf(stderr, "fileName ptr NULL\n");
					exit(0);
				}

				char *filePath = malloc(strlen(inputPath) + 1 + strlen(fData->cFileName) + 1);
				if( !filePath )
				{
					fprintf(stderr, "filePath ptr NULL\n");
					exit(0);
				}

				// copy strings to final location
				strcpy(fileName, fData->cFileName);
				strcpy(filePath, sPath);

				// check if file > 2^32 bytes then create new fileitem
				if( fData->nFileSizeHigh != 0 )
					*fileObj = (FileItem) { fileName, filePath, constructByteSize(fData->nFileSizeLow, fData->nFileSizeHigh), constructWriteTime(fData->ftLastWriteTime) };          
				else
					*fileObj = (FileItem) { fileName, filePath, fData->nFileSizeLow, constructWriteTime(fData->ftLastWriteTime) };

				fileItems[*fileIndex] = fileObj;

				HASH_ADD_KEYPTR( hh, hTable->fi, fileObj->filePath, strlen(fileObj->filePath), fileObj );

				// add bytesize to sum
				*totalSize += fileItems[*fileIndex]->byteSize;	

				++*fileIndex;
			}
		}
	} while( FindNextFile(fHandle, fData) );
}


#elif defined _MSC_VER

void convertWCharToString(char * buf, WCHAR * wchar)
{
	//while( *buf++ = (char)*wchar++ );
	while( *buf++ = (char)*wchar++ )
	{
		// handling .|52428|52428|52428|..
		if( *wchar > 255 ) //== 52428 )
		{
			*buf = 0;
			break;
		}
	}
}

unsigned int fa_readonly = 0, fa_hidden = 0, fa_system = 0, fa_directory = 0, fa_archive = 0, fa_device = 0, fa_normal = 0, fa_temporary = 0, fa_sparsefile = 0,
fa_reparsepoint = 0, fa_compressed = 0, fa_offline = 0, fa_not_indexed = 0, fa_encrypted = 0, fa_integrity = 0, fa_virtual = 0, fa_noscrub = 0,
fa_recallopen = 0, fa_recallaccess = 0;

void calculateFileAndDirCount(const char * const inputPath, HANDLE * fHandle, WIN32_FIND_DATA * fData, unsigned int * const fileCount,
	unsigned int * const dirCount)
{
	if( strlen(inputPath) > 253 )
	{
		fprintf(stderr, "given path is too long, aborting..");
		exit(0);
	}

	char sPath[BUFSIZE], buf[BUFSIZE];

	// specify file mask
	sprintf(sPath, "%s\\*.*", inputPath);

	fHandle = FindFirstFileA(sPath, fData);
	if( fHandle == INVALID_HANDLE_VALUE )
	{
		//fprintf(stderr, "%s is not a valid directory, aborting..\n", inputPath);
		return;
	} 

	// recursion loop
	do
	{	
		convertWCharToString(buf, fData->cFileName);

		// FindFirstFile always returns . and .. first
		if( strcmp(buf, ".") != 0 && strcmp(buf, "..") != 0 )
		{
			incFileInts(fData->dwFileAttributes);

			// set sPath 0
			memset(sPath, 0, BUFSIZE);

			// concatenate filepath
			sprintf(sPath, "%s\\%s", inputPath, buf);

			// check if inputPath is directory or file
			if( fData->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
			{
				++*dirCount;

				// set cFileName 0
				memset(fData->cFileName, 0, BUFSIZE);

				//recursion
				calculateFileAndDirCount(sPath, fHandle, fData, fileCount, dirCount);	
			}					         
			else
			{
				++*fileCount;

				// set cFileName 0
				memset(fData->cFileName, 0, BUFSIZE);
			}			
		}
	} while( FindNextFile(fHandle, fData) );	
}

void traverse(const char * const inputPath, HANDLE * fHandle, WIN32_FIND_DATA * fData, FileItem * fileItems[], DirectoryItem * dirItems[],
	unsigned long long * const totalSize, unsigned int * const fileIndex, unsigned int * const dirIndex, HTable * hTable)
{
	if( strlen(inputPath) > 252 )
	{
		fprintf(stderr, "given path is too long, aborting..");
		exit(0);
	}

	char sPath[BUFSIZE], buf[BUFSIZE];

	// specify file mask
	sprintf(sPath, "%s\\*.*", inputPath);

	fHandle = FindFirstFileA(sPath, fData);
	if( fHandle == INVALID_HANDLE_VALUE )
	{
		//fprintf(stderr, "%s is not a valid directory, aborting..\n", inputPath);
		return;
	}   

	do
	{
		convertWCharToString(buf, fData->cFileName);

		// FindFirstFile always returns . and .. first
		if( strcmp(buf, ".") != 0 && strcmp(buf, "..") != 0 )
		{
			incFileInts(fData->dwFileAttributes);

			// set cFileName 0
			memset(sPath, 0, BUFSIZE);

			// concatenate filepath
			sprintf(sPath, "%s\\%s", inputPath, buf);

			// check if inputPath is directory or file
			if( fData->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
			{       
				// output directory
				//printf("Directory: %s\n", sPath);		

				// request memory
				DirectoryItem *dirObj = malloc(sizeof(DirectoryItem));
				if( !dirObj )
				{
					fprintf(stderr, "traverse dirObj ptr NULL\n");
					return;
				}

				char *dirName = malloc(strlen(buf) + 1);
				if( !dirName )
				{
					fprintf(stderr, "traverse dirName ptr NULL\n");
					return;
				}

				char *dirPath = malloc(strlen(sPath) + 1);
				if( !dirPath )
				{
					fprintf(stderr, "traverse dirPath ptr NULL\n");
					return;
				}

				// copy strings to final location
				strcpy(dirName, buf);
				strcpy(dirPath, sPath);

				// create DirectoryItem
				*dirObj = (DirectoryItem) { dirName, dirPath };
				dirItems[*dirIndex] = dirObj;

				// add item to hashtable
				HASH_ADD_KEYPTR(hh, hTable->di, dirObj->dirPath, strlen(dirObj->dirPath), dirObj);

				// inc index
				++*dirIndex;

				// set cFileName 0
				memset(fData->cFileName, 0, BUFSIZE);

				// recursion
				traverse(sPath, fHandle, fData, fileItems, dirItems, totalSize, fileIndex, dirIndex, hTable);
			}
			else
			{
				// output file
				//printf("File: %s\n", sPath);

				// request memory for new fileitem
				FileItem *fileObj = malloc(sizeof(FileItem));
				if( !fileObj )
				{
					fprintf(stderr, "fileObj NULL");
					return;
				}

				char *fileName = malloc(strlen(buf) + 1);	
				if( !fileName )
				{
					fprintf(stderr, "fileName ptr NULL\n");
					exit(0);
				}

				char *filePath = malloc(strlen(inputPath) + 1 + strlen(buf) + 1);
				if( !filePath )
				{
					fprintf(stderr, "filePath ptr NULL\n");
					exit(0);
				}

				// copy strings to final location
				strcpy(fileName, buf);
				strcpy(filePath, sPath);

				// check if file > 2^32 bytes then create new fileitem
				if( fData->nFileSizeHigh != 0 )
					*fileObj = (FileItem) { fileName, filePath, constructByteSize(fData->nFileSizeLow, fData->nFileSizeHigh), constructWriteTime(fData->ftLastWriteTime) };          
				else
					*fileObj = (FileItem) { fileName, filePath, fData->nFileSizeLow, constructWriteTime(fData->ftLastWriteTime) };

				fileItems[*fileIndex] = fileObj;

				// add item to hashtable
				HASH_ADD_KEYPTR(hh, hTable->fi, fileObj->filePath, strlen(fileObj->filePath), fileObj);

				// add bytesize to sum
				*totalSize += fileItems[*fileIndex]->byteSize;	

				// inc index
				++*fileIndex;

				// set cFileName 0
				memset(fData->cFileName, 0, BUFSIZE);
			}
		}
	} while( FindNextFile(fHandle, fData) );
}

void incFileInts(unsigned int filemask)
{
	if( filemask & 1 )
		++fa_readonly;
	if( filemask & 2 )
		++fa_hidden;
	if( filemask & 4 )
		++fa_system;
	if( filemask & 16 )
		++fa_directory;
	if( filemask & 32 )
		++fa_archive;
	if( filemask & 64 )
		++fa_device;
	if( filemask & 128 )
		++fa_normal;
	if( filemask & 256 )
		++fa_temporary;
	if( filemask & 512 )
		++fa_sparsefile;
	if( filemask & 1024 )
		++fa_reparsepoint;
	if( filemask & 2048 )
		++fa_compressed;
	if( filemask & 4096 )
		++fa_offline;
	if( filemask & 8192 )
		++fa_not_indexed;
	if( filemask & 16384 )
		++fa_encrypted;
	if( filemask & 32768 )
		++fa_integrity;
	if( filemask & 65536 )
		++fa_virtual;
	if( filemask & 131072 )
		++fa_noscrub;
	if( filemask & 262144 )
		++fa_recallopen;
	if( filemask & 4194304 )
		++fa_recallaccess;
}

void resetFileInts()
{
	fa_readonly = 0; fa_hidden = 0; fa_system = 0; fa_directory = 0; fa_archive = 0; fa_device = 0; fa_normal = 0; fa_temporary = 0; fa_sparsefile = 0;
	fa_reparsepoint = 0; fa_compressed = 0; fa_offline = 0; fa_not_indexed = 0; fa_encrypted = 0; fa_integrity = 0; fa_virtual = 0; fa_noscrub = 0;
	fa_recallopen = 0, fa_recallaccess = 0;
}

void printFileInts()
{
	iDEBUG_LOG("readonly ", fa_readonly);
	iDEBUG_LOG("hidden ", fa_hidden);
	iDEBUG_LOG("system ", fa_system);
	iDEBUG_LOG("directory ", fa_directory);
	iDEBUG_LOG("archive ", fa_archive);
	iDEBUG_LOG("device ", fa_device);
	iDEBUG_LOG("normal ", fa_normal);
	iDEBUG_LOG("temporary ", fa_temporary);
	iDEBUG_LOG("sparsefile ", fa_sparsefile);
	iDEBUG_LOG("reparsepoint ", fa_reparsepoint);
	iDEBUG_LOG("compressed ", fa_compressed);
	iDEBUG_LOG("offline ", fa_offline);
	iDEBUG_LOG("not_indexed ", fa_not_indexed);
	iDEBUG_LOG("encrypted ", fa_encrypted);
	iDEBUG_LOG("integrity ", fa_integrity);
	iDEBUG_LOG("virtual ", fa_virtual);
	iDEBUG_LOG("noscrub ", fa_noscrub);
	iDEBUG_LOG("recallopen ", fa_recallopen);
	iDEBUG_LOG("recallaccess ", fa_recallaccess);

}

#endif