#include "all.h"

int existPath(char *path)
{
	return *path && !_access(path, 0);
}
int existFile(char *file)
{
	int result = existPath(file) && !(GetFileAttributes(file) & FILE_ATTRIBUTE_DIRECTORY);

	if(!result)
		cout("%s does not exist.\n", file);

	return result;
}
int existDir(char *dir)
{
	return existPath(dir) && GetFileAttributes(dir) & FILE_ATTRIBUTE_DIRECTORY;
}

void trimPath(char *path)
{
	escapeYen(path);
	trimSequ(path, '/');
	unescapeYen(path);
}
char *combine(char *dir, char *file)
{
	if(dir[0] && dir[1] == ':' && dir[2] == '\0')
	{
		static char buff[] = "?:.";

		buff[0] = dir[0];
		dir = buff;
	}
	char *dirFile = xcout("%s\\%s", dir, file);

	trimPath(dirFile);
	return dirFile;
}
