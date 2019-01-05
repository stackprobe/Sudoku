#include "C:\Factory\Common\all.h"
#include "C:\Factory\Common\Options\csv.h"
#include "C:\Factory\Common\Options\CryptoRand_MS.h"
#include "libs\Mk9x9.h"
#include "libs\KnownPosNumb.h"

#define EV_BRUTE_FORCE_STARTED "{b6747fc1-a187-4b3b-9bc9-0635d9165b9f}" // shared_uuid
#define EV_TOM_STARTED "{6a3a714c-fdba-4bd6-a492-2e1fd0a63fee}"
#define EV_TOM_STOP    "{34133957-8e31-40df-8e23-faaeb5270087}"
#define EV_TOM_STOPPED "{d3e62a1b-9ec8-4103-a820-926d5c90f563}"
#define EV_TIMED_OUT   "{1561db74-b873-4e4d-9db7-0c46d03b69cc}"

#define TRUSTED_TIMEOUT_MILLIS 30000 // タイムアウト前にシグナルが必ず来る「はず」の待ち用
#define PUT_RAND_TIMEOUT_SEC 3600
#define BRUTE_FORCE_TIMEOUT_SEC 30
#define RAND_BUFF_SIZE 8000

static int Difficulty_Hard;

static char *DataDir;
static char *InputFile;
static char *OutputFile;
static char *ErrorFile;
static char *LoadedDataFile;

static uint EvCancel;
static autoList_t *AnswerTable;
static uint Ans_W;
static uint Ans_H;
static uint NumbMax;
static autoList_t *InputTable;

static uint Cnt_P;
static uint Cnt_K;
static uint Cnt_X;

enum
{
	RET_SUCCESSFUL = 1,
	RET_FAILED,
	RET_TIMED_OUT,
};

static uint GetNumb(autoList_t *table, uint x, uint y)
{
	return toValue(getLine(getList(table, y), x));
}
static void SetNumb(autoList_t *table, uint x, uint y, uint numb)
{
	char **p_cell = (char **)directGetPoint(getList(table, y), x);

	errorCase(!*p_cell); // 2bs

	if(numb)
		strzp_x(p_cell, xcout("%u", numb));
	else
		**p_cell = '\0';
}
static uint NextRand(void)
{
	static uchar buff[RAND_BUFF_SIZE];
	static uint index = RAND_BUFF_SIZE;

	if(index == RAND_BUFF_SIZE)
	{
		getCryptoBlock_MS(buff, RAND_BUFF_SIZE);
		index = 0;
	}
	return buff[index++];
}
static uint GetRand_Mod(uint modulo)
{
	uint64 value;

	((uchar *)&value)[0] = NextRand();
	((uchar *)&value)[1] = NextRand();
	((uchar *)&value)[2] = NextRand();
	((uchar *)&value)[3] = NextRand();
	((uchar *)&value)[4] = NextRand();
	((uchar *)&value)[5] = NextRand();
	((uchar *)&value)[6] = NextRand();
	((uchar *)&value)[7] = NextRand();

	return (uint)(value % (uint64)modulo);
}
static uint GetRand32(void)
{
	uint value;

	getCryptoBlock_MS((uchar *)&value, sizeof(value));

	return value;
}
static uint GetRand(uint minval, uint maxval)
{
	errorCase(maxval < minval);

	if(minval == 0 && maxval == UINTMAX)
		return GetRand32();

	return GetRand_Mod(maxval - minval + 1) + minval;
}
static void PrintProgPct(double pct)
{
	cout("PROGRESS == %f PCT OK -- P=%u K=%u X=%u\n", pct, Cnt_P, Cnt_K, Cnt_X);

	cmdTitle_x(xcout("MkSudoku - %f PCT OK", pct));
}
static char *GetSudokuExe(void)
{
	static char *file;

	if(!file)
	{
		file = combine(getSelfDir(), "Sudoku.exe");

		if(!existFile(file)) // ? dev env
		{
			memFree(file);
			file = "C:\\Dev\\Main\\Sudoku\\Sudoku\\Release\\Sudoku.exe";
			errorCase(!existFile(file));
		}
	}
	return file;
}
static int CallSudoku(void)
{
	uint evTomStarted = eventOpen(EV_TOM_STARTED);
	uint evTimedOut = eventOpen(EV_TIMED_OUT);
	int ret;

	removeFileIfExist(OutputFile);
	removeFileIfExist(ErrorFile);
	removeFileIfExist(LoadedDataFile);

	// memo: コマンドラインに "..." が2つ以上あれば START "" /B /WAIT そうでなければ何もいらないっぽい。
	// <- 本当か？

	execute_x(xcout("START \"\" /B \"%s\" /TIME-OUT-MONITOR", getSelfFile()));
	handleWaitForMillis(evTomStarted, TRUSTED_TIMEOUT_MILLIS); // TOM の開始を待つ。
	execute_x(xcout("START \"\" /B /WAIT /HIGH \"%s\" /D \"%s\" > NUL", GetSudokuExe(), DataDir));
	execute_x(xcout("\"%s\" /TIME-OUT-MONITOR-STOP", getSelfFile()));

	handleClose(evTomStarted);

	if(handleWaitForMillis(evTimedOut, 0))
	{
		ret = RET_TIMED_OUT;
	}
	else if(existFile(OutputFile) && !existFile(ErrorFile)) // ? 成功
	{
		ret = RET_SUCCESSFUL;
	}
	else
	{
		ret = RET_FAILED;
	}
	handleClose(evTimedOut);
	cout("CallSudoku_ret: %d\n", ret);

	if(handleWaitForMillis(EvCancel, 0)) // ★★★ ここで中断処理してるよ！ ★★★
	{
		cout("+---------------+\n");
		cout("| 中断しました。|\n");
		cout("+---------------+\n");
		termination(1);
	}
	return ret;
}
static int CallSudoku_I(void)
{
	writeCSVFile(InputFile, InputTable);
	return CallSudoku();
}
static uint GetNumbMax(void)
{
	uint ret = 0;
	uint x;
	uint y;

	for(x = 0; x < Ans_W; x++)
	for(y = 0; y < Ans_H; y++)
	{
		m_maxim(ret, GetNumb(AnswerTable, x, y));
	}
	return ret;
}
static void MkInputTable(void)
{
	uint x;
	uint y;

	InputTable = newList();

	for(y = 0; y < Ans_H; y++)
	{
		autoList_t *row = newList();

		for(x = 0; x < Ans_W; x++)
		{
			addElement(row, (uint)strx(""));
		}
		addElement(InputTable, (uint)row);
	}
}
static int TPR_IsPuttable(uint x, uint y)
{
	return GetNumb(AnswerTable, x, y) && !GetNumb(InputTable, x, y);
}
static int TPR_IsPutted(uint x, uint y)
{
	return GetNumb(InputTable, x, y);
}
static double TPR_GetPuttedRate(void)
{
	uint a = 0;
	uint d = 0;
	uint x;
	uint y;

	for(x = 0; x < Ans_W; x++)
	for(y = 0; y < Ans_H; y++)
	{
		if(TPR_IsPuttable(x, y))
		{
			a++;
		}
		else if(TPR_IsPutted(x, y))
		{
			d++;
		}
	}
	return d * 1.0 / (a + d);
}
static int TryPutRand(void) // ret: ? まだ追加出来ると思う。
{
	uint refindCount = 0;
	uint count;
	uint numb;
	uint x;
	uint y;

refindPosNumb:

	// Puttable の個数を数える。-> count

	count = 0;

	for(x = 0; x < Ans_W; x++)
	for(y = 0; y < Ans_H; y++)
	{
		if(TPR_IsPuttable(x, y))
			count++;
	}
	if(!count)
	{
		cout("もう追加出来ない！\n");
		return 0;
	}

	// count -> 0 ～ (count - 1)

	cout("%u -> ", count);
	count = GetRand_Mod(count);
	cout("%u\n", count);

	// (count + 1) 番目の Puttable を採用する。

	for(x = 0; x < Ans_W; x++)
	for(y = 0; y < Ans_H; y++)
	{
		if(TPR_IsPuttable(x, y))
		{
			if(!count)
				goto foundPos;

			count--;
		}
	}
	error();

foundPos:
	numb = GetRand(1, NumbMax);

	if(KPN_IsKnown(x, y, numb))
	{
		if(refindCount < 10) // fixme: 上限適当
		{
			refindCount++;
			goto refindPosNumb;
		}
		KPN_Reset();
	}
	KPN_Add(x, y, numb);

	cout("TryPutRand: x=%u, y=%u, numb=%u\n", x, y, numb);

	SetNumb(InputTable, x, y, numb);

	switch(CallSudoku_I())
	{
	case RET_SUCCESSFUL:
		Cnt_P++;
		break;

	case RET_FAILED:
	case RET_TIMED_OUT:
		SetNumb(InputTable, x, y, 0);
		break;

	default:
		error();
	}
	return 1;
}
static void PutRand(void)
{
	uint startTime = now();

	do
	{
		uint e = now() - startTime;

		cout("PR_Elapsed: %u (%u)\n", e, PUT_RAND_TIMEOUT_SEC);

		if(PUT_RAND_TIMEOUT_SEC < e)
			break;

		PrintProgPct(TPR_GetPuttedRate() * 50.0);
	}
	while(TryPutRand());

	switch(CallSudoku_I())
	{
	case RET_SUCCESSFUL:
		break;

	default:
		error_m("Please retry later."); // 解いた事があるはずなので、解けるはず。タイムアウトぎりぎりだったのかも。XXX
	}
	releaseDim(AnswerTable, 2);
	releaseDim(InputTable, 2);
	AnswerTable = NULL;
	InputTable = readCSVFileTR(OutputFile);

	errorCase(Ans_H != getCount(InputTable));
	errorCase(Ans_W != getCount(getList(InputTable, 0)));
}
static void K_Shuffle(autoList_t *list)
{
	uint index;

	for(index = 0; index + 1 < getCount(list); index++)
	{
		swapElement(list, index, GetRand(index, getCount(list) - 1));
	}
}
static int TryKezuru(uint x, uint y) // ret: ? タイムアウトだった。
{
	uint ansNumb = GetNumb(InputTable, x, y);
	uint numb;
	int ret = 0;

	for(numb = 1; numb <= NumbMax; numb++)
	{
		if(numb == ansNumb)
			continue;

		SetNumb(InputTable, x, y, numb);

		switch(CallSudoku_I())
		{
		case RET_FAILED:
			break;

		case RET_TIMED_OUT:
			ret = 1;
			// fall through

		case RET_SUCCESSFUL:
			goto restore;

		default:
			error();
		}
	}
	cout("削った: x=%u, y=%u, numb=%u\n", x, y, ansNumb);
	SetNumb(InputTable, x, y, 0);

	if(!Difficulty_Hard)
	{
		cout("CHECK-DIFFICULTY\n");

		switch(CallSudoku_I()) // 難しくなり過ぎてないかチェック
		{
		case RET_FAILED:
			error();

		case RET_TIMED_OUT:
			cout("難しくなり過ぎた様なので却下！\n");
			goto restore;

		case RET_SUCCESSFUL:
			break;

		default:
			error();
		}
	}
	Cnt_K++;
	goto endFunc;

restore:
	cout("削れなかった。\n");
	SetNumb(InputTable, x, y, ansNumb);

endFunc:
	cout("TK_ret: %d\n", ret);
	return ret;
}
static void Kezuru(void)
{
	autoList_t *pairs = newList();
	uint *pair;
	uint pair_index;
	uint x;
	uint y;

	for(x = 0; x < Ans_W; x++)
	for(y = 0; y < Ans_H; y++)
	{
		if(GetNumb(InputTable, x, y))
		{
			pair = (uint *)memAlloc(3 * sizeof(uint));

			pair[0] = x;
			pair[1] = y;
			pair[2] = 0; // リトライ回数

			addElement(pairs, (uint)pair);
		}
	}
	K_Shuffle(pairs);

	foreach(pairs, pair, pair_index)
	{
		cout("K: %u (%u) %u %u %u\n", pair_index, getCount(pairs), pair[0], pair[1], pair[2]);

		PrintProgPct(50.0 + pair_index * 50.0 / getCount(pairs));

		x = pair[0];
		y = pair[1];

		if(TryKezuru(x, y) && pair[2] < 1) // ? タイムアウトだった。&& リトライしていない。
		{
			// どんどん削って行くのだから、リトライしても意味無いかもしれないけど、ワンチャンあるかなくらいの気持ちで..
			// <- ワンチャン無いだろ。

			/*
			pair = (uint *)memClone(pair, 3 * sizeof(uint));
			pair[2]++;

			cout("タイムアウトだったので後でリトライします: x=%u, y=%u, tried=%u\n", pair[0], pair[1], pair[2]);

			addElement(pairs, (uint)pair); // リトライのため、追加
			*/
		}
	}
	releaseDim(pairs, 1);
}
static void MkSudoku(char *dir)
{
	DataDir = makeFullPath(dir);

	errorCase_m(!existDir(DataDir), "データディレクトリにアクセス出来ません。");

	InputFile = combine(DataDir, "Input.csv");
	OutputFile = combine(DataDir, "Output.csv");
	ErrorFile = combine(DataDir, "Error.csv");
	LoadedDataFile = combine(DataDir, "LoadedData.csv");

	removeFileIfExist(InputFile);

	cout("問題を作成しています...\n");

	switch(CallSudoku())
	{
	case RET_SUCCESSFUL:
		break;

	case RET_FAILED:
		error_m("入力データに誤りがあるようです。");

	case RET_TIMED_OUT:
		error_m("入力データが難し過ぎるか、誤りがあるようです。");

	default:
		error();
	}
	AnswerTable = readCSVFileTR(OutputFile);
	Ans_H = getCount(AnswerTable);
	errorCase(!Ans_H);
	Ans_W = getCount(getList(AnswerTable, 0));
	errorCase(!Ans_W);
	NumbMax = GetNumbMax();

	// set Cnt_X
	{
		uint x;
		uint y;

//		Cnt_X = 0;

		for(x = 0; x < Ans_W; x++)
		for(y = 0; y < Ans_H; y++)
		{
			if(GetNumb(AnswerTable, x, y))
				Cnt_X++;
		}
	}

	// ---- 問題作成メイン ----

	if(Is9x9(DataDir))
	{
		Mk9x9InputCsvFile(DataDir);
		InputTable = readCSVFileTR(InputFile);

		errorCase(Ans_H != getCount(InputTable));
		errorCase(Ans_W != getCount(getList(InputTable, 0)));

		Cnt_P = IMAX;
	}
	else
	{
		MkInputTable();
		PutRand();
	}
	Kezuru();

	// ----

	PrintProgPct(100.0);

	writeCSVFile(InputFile, InputTable);

	removeFileIfExist(OutputFile);
	removeFileIfExist(ErrorFile);
	removeFileIfExist(LoadedDataFile);

	cout("+---------------------+\n");
	cout("| 問題を作成しました。|\n");
	cout("+---------------------+\n");
}
static void Main2(void)
{
	uint mtxProc;

	if(argIs("/TIME-OUT-MONITOR"))
	{
		uint evBfStarted = eventOpen(EV_BRUTE_FORCE_STARTED);
		uint evTomStop = eventOpen(EV_TOM_STOP);
		uint timeout = 0;

		eventWakeup(EV_TOM_STARTED);

		while(!handleWaitForMillis(evTomStop, 2000))
		{
			if(!timeout)
			{
				if(handleWaitForMillis(evBfStarted, 0))
					timeout = now() + BRUTE_FORCE_TIMEOUT_SEC;
			}
			else if(timeout < now())
			{
				execute_x(xcout("\"%s\" /S", GetSudokuExe()));
				eventWakeup(EV_TIMED_OUT);
				handleWaitForMillis(evTomStop, TRUSTED_TIMEOUT_MILLIS); // EV_TOM_STOPPED をセットしたいので、EV_TOM_STOP を待つ。
				break;
			}
		}
		handleClose(evTomStop);
		eventWakeup(EV_TOM_STOPPED);
		return;
	}
	if(argIs("/TIME-OUT-MONITOR-STOP"))
	{
		uint evTomStopped = eventOpen(EV_TOM_STOPPED);
		eventWakeup(EV_TOM_STOP);
		handleWaitForMillis(evTomStopped, TRUSTED_TIMEOUT_MILLIS);
		handleClose(evTomStopped);
		return;
	}

	EvCancel = eventOpen("{93779a12-8e18-47c3-90f2-b99456a60326}");

	if(argIs("/S"))
	{
		eventWakeupHandle(EvCancel);
		return;
	}

	mtxProc = mutexTryProcLock("{387ec824-bcb9-4f35-98ec-f356407b6638}");

	// 2bs: ゾンビ TOM を止める。
	{
		eventWakeup(EV_TOM_STOP);
		sleep(100);
		eventWakeup(EV_TOM_STOP);
		sleep(100);
		eventWakeup(EV_TOM_STOP);
		sleep(100);
	}

	if(argIs("/H"))
	{
		Difficulty_Hard = 1;
	}
	if(hasArgs(1))
	{
		MkSudoku(nextArg());
	}
	else
	{
		MkSudoku(dropDir());
	}
	cmdTitle("MkSudoku");

	mutexUnlock(mtxProc);

	handleClose(EvCancel);
}
int main(int argc, char **argv)
{
	Main2();
	termination(0);
}
