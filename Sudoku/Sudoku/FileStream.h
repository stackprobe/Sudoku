FILE *rfopen(char *file, char *mode);
FILE *fileOpen(char *file, char *mode);
void fileClose(FILE *fp);

int readChar(FILE *fp);
void writeChar(FILE *fp, int chr);
void writeToken(FILE *fp, char *token);
