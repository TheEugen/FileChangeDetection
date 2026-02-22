#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include "types.h"
#include "traverse.h"
#include "utils.h"
#include "extern\\argtable3.h"

#pragma warning (disable : 4996)


int main(int argc, char ** argv)
{
	printf("\n#############################\n");
	printf("##  File Change Detection  ##\n");
	printf("#############################\n\n");

	Traverser *traverser_new = NULL, *traverser_old = NULL;

	HANDLE hThread = NULL;

	unsigned int major = 1, minor = 0, patch = 0;

	struct arg_lit *diffItem, *c, *help, *version;
	struct arg_file *o, *file, *directory, *w;
	struct arg_end *end;

	void *argtable[] = 
	{
		directory	= arg_filen(NULL, NULL, "<dir>", 0, 1, "directory to parse"),
		help		= arg_litn(NULL, "help", 0, 1, "display this help and exit"),
		version		= arg_litn(NULL, "version", 0, 1, "display version info and exit"),
		o			= arg_filen("o", NULL, "saveFile", 0, 1, "save directory structure"),
		c			= arg_litn("c", NULL, 0, 1, "compare only"),
		diffItem    = arg_litn("d", NULL, 0, 1, "show added and deleted files and directorys"),
		file		= arg_filen(NULL, NULL, "<file>", 0, 1, "file to compare"),
		w           = arg_filen("w", NULL, "<file>", 0, 1, "write added and deleted files and directorys to file"),	
		end			= arg_end(20),
	};

	int exitcode = 0;
	char progname[] = "FileChangeDetection";

	int nerrors;
	nerrors = arg_parse(argc, argv, argtable);

	if( help->count > 0 )
	{
		printf("Usage: %s", progname);
		arg_print_syntax(stdout, argtable, "\n\n");
		//printf("FileChangeDetection usage:\n\n");
		arg_print_glossary(stdout, argtable, "  %-25s %s\n");
		exitcode = 0;
	}
	else if( version->count > 0 )
	{
		printf("FileChangeDetection Version: %d.%d.%d\n", major, minor, patch);
	}
	else if( directory->count == 0 )
	{
		printf("No directory specified, aborting..\n");
		exitcode = -1;
	}
	else if( c->count > 0 ) // compare only
	{
		if( file->count == 0 )
		{
			printf("No file specified, aborting..\n");
			exitcode = -1;
		}
		else
		{
			traverser_new = allocTraverser();		
			traverser_old = allocTraverser();

			if( diffItem->count > 0 && w->count > 0 )
				startComparing(file->filename[0], traverser_new, traverser_old, 1, 1, directory->filename[0], w->filename[0]);
			else if( diffItem->count > 0)
				startComparing(file->filename[0], traverser_new, traverser_old, 1, 1, directory->filename[0], NULL);
			else if( w->count > 0 )
				startComparing(file->filename[0], traverser_new, traverser_old, 0, 1, directory->filename[0], w->filename[0]);
			else
				startComparing(file->filename[0], traverser_new, traverser_old, 0, 1, directory->filename[0], NULL);
		}
	}
	else if( validateDirectory(directory->filename[0]) )
	{
		traverser_new = allocTraverser();

		if( o->count > 0 && file->count > 0 ) // parse, write and compare
		{
			startTraverse(directory->filename[0], o->filename[0], traverser_new, 1, 1, &hThread);
			traverser_old = allocTraverser();
			/*if( diffItem->count > 0 )
				startComparing(file->filename[0], traverser_new, traverser_old, 1, 0, NULL);
			else
				startComparing(file->filename[0], traverser_new, traverser_old, 0, 0, NULL);*/

			if( diffItem->count > 0 && w->count > 0 )
				startComparing(file->filename[0], traverser_new, traverser_old, 1, 0, NULL, w->filename[0]);
			else if( diffItem->count > 0)
				startComparing(file->filename[0], traverser_new, traverser_old, 1, 0, NULL, NULL);
			else if( w->count > 0 )
				startComparing(file->filename[0], traverser_new, traverser_old, 0, 0, NULL, w->filename[0]);
			else
				startComparing(file->filename[0], traverser_new, traverser_old, 0, 0, NULL, NULL);
		}
		else if( o->count > 0 ) // parse and write
		{	
			startTraverse(directory->filename[0], o->filename[0], traverser_new, 1, 0, NULL);
		}
		else if( file->count > 0 ) // parse and compare
		{	
			startTraverse(directory->filename[0], NULL, traverser_new, 0, 0, NULL);
			traverser_old = allocTraverser();

			if( diffItem->count > 0 && w->count > 0 )
				startComparing(file->filename[0], traverser_new, traverser_old, 1, 0, NULL, w->filename[0]);
			else if( diffItem->count > 0)
				startComparing(file->filename[0], traverser_new, traverser_old, 1, 0, NULL, NULL);
			else if( w->count > 0 )
				startComparing(file->filename[0], traverser_new, traverser_old, 0, 0, NULL, w->filename[0]);
			else
				startComparing(file->filename[0], traverser_new, traverser_old, 0, 0, NULL, NULL);		
		}
		else
		{				
			startTraverse(directory->filename[0], NULL, traverser_new, 0, 0, NULL);
		}
	}

	if( hThread )
	{
		WaitForSingleObject(hThread, INFINITE);
		CloseHandle(hThread);
	}

	// cleanup
	arg_freetable(argtable, sizeof(argtable) / sizeof(argtable[0]));

	if( traverser_new )
		releaseRes(traverser_new, 0);
	if( traverser_old )
		releaseRes(traverser_old, 1);

	return exitcode;
}
