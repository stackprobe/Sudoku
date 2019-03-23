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

// sync > @ cout

void cout(char *format, ...)
{
	va_list marker;

	va_start(marker, format);

	if(!strcmp(format, "%s"))
	{
		coutLongText(va_arg(marker, char *));
	}
	else if(!strcmp(format, "%s\n"))
	{
		coutLongText(va_arg(marker, char *));
		coutLongText("\n");
	}
	else
		coutLongText_x(vxcout(format, marker));

	va_end(marker);
}
char *xcout(char *format, ...)
{
	char *ret;
	va_list marker;

	va_start(marker, format);
	ret = vxcout(format, marker);
	va_end(marker);

	return ret;
}
char *vxcout(char *format, va_list marker)
{
	char *buffer;

	for(int size = strlen(format) + 100; ; size *= 2)
	{
		int ret;

#define MARGIN 10

		buffer = (char *)memAlloc(size + MARGIN * 2);
		ret = _vsnprintf(buffer, size + MARGIN, format, marker);

#undef MARGIN

		if(0 <= ret && ret <= size)
			break;

		memFree(buffer);
		errorCase(128 * 1024 * 1024 < size); // ANTI OVER-FLOW
	}
	return strr(buffer);
}
void coutLongText(char *text)
{
	char format[6]; // max: "%.99s"
	char *p;
	char *q;
	uint d;

	for(p = text; *p; p = q)
	{
#define PRINT_LMT 98

		for(q = p; *q && (uint)q - (uint)p < PRINT_LMT; q += _ismbblead(*q) ? 2 : 1)
		{}

#undef PRINT_LMT

		d = (uint)q - (uint)p;

		errorCase(sprintf(format, "%%.%us", d) < 0);
		errorCase(printf(format, p) != d);
	}
}
void coutLongText_x(char *text)
{
	coutLongText(text);
	memFree(text);
}

// < sync

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
	errorCase(index < 0 || __argc < index); // index == __argc ‚Í‘S•”“Ç‚ÝI‚í‚Á‚½ó‘Ô
	ArgIndex = index;
}
