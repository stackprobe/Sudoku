void AddErrorPos(i2D_t pos);
void UnaddErrorPos(void);

void WriteErrorCsvFile(char *source, int lineno, char *function, char *errorcode);
void WriteLoadedDataCsvFile(void);
