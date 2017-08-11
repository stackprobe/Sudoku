#include "all.h"

static void DoCloseHandle(HANDLE &hdl)
{
	if(hdl != NULL)
	{
		CloseHandle(hdl);
		hdl = NULL;
	}
}
void termination(int errorlevel)
{
	// app >

	DoCloseHandle(Dc.EvStop);
	DoCloseHandle(Dc.EvBruteForceStarted);
errorCase(Dc.EvStop != NULL); // test
errorCase(Dc.EvBruteForceStarted != NULL); // test

	// < app

	exit(errorlevel);
}
void error2(char *source, int lineno, char *function, char *errorcode)
{
	{
		static int entcnt;

		if(entcnt)
			exit(2);

		entcnt = 1;
	}

	cout("+-------+\n");
	cout("| ERROR |\n");
	cout("+-------+\n");
	cout("location: %s (%u) %s\n", source, lineno, function);
	cout("cause: %s\n", errorcode);

	// app >

	WriteErrorCsvFile(source, lineno, function, errorcode);
	WriteLoadedDataCsvFile();

	// < app

	termination(1);
}

void cout(char *format, ...)
{
	va_list marker;

	va_start(marker, format);
	errorCase(vprintf(format, marker) < 0);
	va_end(marker);
}
char *xcout(char *format, ...)
{
	char *buffer;
	int size;
	va_list marker;

	va_start(marker, format);

	for(size = strlen(format) + 128; ; size *= 2)
	{
		buffer = (char *)memAlloc(size + 20);
		int retval = _vsnprintf(buffer, size + 10, format, marker);
		buffer[size + 10] = '\0'; // 強制的に閉じる。

		if(0 <= retval && retval <= size)
			break;

		memFree(buffer);
		errorCase(128 * 1024 * 1024 < size); // anti-overflow
	}
	va_end(marker);

	return buffer;
}

static int ArgIndex = 1;

int hasArgs(int count)
{
	return count <= __argc - ArgIndex;
}
int argIs(char *spell)
{
	if(ArgIndex < __argc)
	{
		if(!_stricmp(__argv[ArgIndex], spell))
		{
			ArgIndex++;
			return 1;
		}
	}
	return 0;
}
char *getArg(int index)
{
	errorCase(index < 0 || __argc - ArgIndex <= index);
	return __argv[ArgIndex + index];
}
char *nextArg(void)
{
	char *arg = getArg(0);

	ArgIndex++;
	return arg;
}
int getArgIndex(void)
{
	return ArgIndex;
}
void setArgIndex(int index)
{
	errorCase(index < 0 || __argc < index); // index == __argc は全部読み終わった状態
	ArgIndex = index;
}
