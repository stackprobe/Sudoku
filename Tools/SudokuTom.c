#include "C:\Factory\Common\all.h"
#include "SudokuTomCommon.h"

#define SUDOKU_EXE "C:\\Dev\\Main\\Sudoku\\Sudoku\\Release\\Sudoku.exe"

#define EV_BRUTE_FORCE_STARTED "{b6747fc1-a187-4b3b-9bc9-0635d9165b9f}" // shared_uuid
#define EV_STARTED "aaa"
#define EV_STOP "bbb"
#define EV_STOPPED "ccc"

#define TRUSTED_TIMEOUT_MILLIS 30000 // タイムアウト前にシグナルが必ず来る「はず」の待ち用
#define BRUTE_FORCE_TIMEOUT_MILLIS 30000

int main(int argc, char **argv)
{
	errorCase(!existFile(SUDOKU_EXE));

	if(argIs("/T")) // Start
	{
		uint evStarted = eventOpen(EV_STARTED);

		execute_x(xcout("START \"Sudoku-TOM\" /MIN \"%s\" TOM-START", getSelfFile()));

		handleWaitForMillis(evStarted, TRUSTED_TIMEOUT_MILLIS);
		handleClose(evStarted);
		return;
	}
	if(argIs("/E")) // End
	{
		uint evStopped = eventOpen(EV_STOPPED);

		eventWakeup(EV_STOP);

		handleWaitForMillis(evStopped, TRUSTED_TIMEOUT_MILLIS);
		handleClose(evStopped);
		return;
	}
	if(argIs("/KZ")) // Kill Zombies
	{
		eventWakeup(EV_STOP);
		sleep(100);
		eventWakeup(EV_STOP);
		sleep(100);
		eventWakeup(EV_STOP);
		sleep(100);
		return;
	}

	// 以下、内部呼び出し用オプション

	if(argIs("TOM-START"))
	{
		uint evBfStarted = eventOpen(EV_BRUTE_FORCE_STARTED);
		uint evStop = eventOpen(EV_STOP);

		LOGPOS();
		removeFileIfExist(SUDOKU_TIMED_OUT_FLAG);
		LOGPOS();
		eventWakeup(EV_STARTED);

		while(!handleWaitForMillis(evStop, 2000))
		{
			LOGPOS();

			if(handleWaitForMillis(evBfStarted, 0))
			{
				LOGPOS();

				if(!handleWaitForMillis(evStop, BRUTE_FORCE_TIMEOUT_MILLIS))
				{
					LOGPOS();
					createFile(SUDOKU_TIMED_OUT_FLAG);
					LOGPOS();
					execute("\"" SUDOKU_EXE "\" /S");
					LOGPOS();
					handleWaitForMillis(evStop, TRUSTED_TIMEOUT_MILLIS);
				}
				LOGPOS();
				break;
			}
			LOGPOS();
		}
		LOGPOS();
		eventWakeup(EV_STOPPED);

		handleClose(evBfStarted);
		handleClose(evStop);
		return;
	}
}
