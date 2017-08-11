void termination(int errorlevel);
void error2(char *source, int lineno, char *function, char *message);

#define error() \
	error2(__FILE__, __LINE__, __FUNCTION__, "ïsñæÇ»ÉGÉâÅ[")

#define error_m(errorcode) \
	error2(__FILE__, __LINE__, __FUNCTION__, (errorcode))

#define errorCase(status) \
	do { \
	if((status)) error(); \
	} while(0)

#define errorCase_m(status, errorcode) \
	do { \
	if((status)) error_m((errorcode)); \
	} while(0)

#define LOGPOS() \
	cout("%s (%d) %s %.3f\n", __FILE__, __LINE__, __FUNCTION__, GetTickCount() / 1000.0)

void cout(char *format, ...);
char *xcout(char *format, ...);

int hasArgs(int count);
int argIs(char *spell);
char *getArg(int index);
char *nextArg(void);
int getArgIndex(void);
void setArgIndex(int index);
