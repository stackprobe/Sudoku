/*
	Reverter.exe [データDIR]

		データDIRは、解析可能なデータであること。
*/

#include "C:\Factory\Common\all.h"
#include "C:\Factory\Common\Options\csv.h"
#include "C:\Factory\Common\Options\CRRandom.h"
#include "SudokuTomCommon.h"
#include "..\libs\KnownPosNumb.h"

#define SUDOKU_EXE "C:\\Dev\\Main\\Sudoku\\Sudoku\\Release\\Sudoku.exe"
#define SUDOKU_TOM_EXE "C:\\Dev\\Main\\Sudoku\\Tools\\SudokuTom.exe"

static char *MasterDataDir;
static char *WkDataDir;

static autoList_t *InputTbl;
static uint Input_W;
static uint Input_H;
static autoList_t *AnswerTbl;
static uint NumbMax;

enum
{
	RET_SUCCESSFUL = 1,
	RET_FAILED,
	RET_TIMED_OUT,
};

static int CallSudoku(void)
{
	int ret;

	coExecute("\"" SUDOKU_TOM_EXE "\" /T");
	coExecute_x(xcout("START \"\" /B /WAIT /HIGH \"" SUDOKU_EXE "\" /D \"%s\" > NUL", WkDataDir));
	coExecute("\"" SUDOKU_TOM_EXE "\" /E");

	if(existFile(SUDOKU_TIMED_OUT_FLAG))
	{
		ret = RET_TIMED_OUT;
	}
	else
	{
		addCwd(WkDataDir);

		if(existFile("Output.csv") && !existFile("Error.csv"))
			ret = RET_SUCCESSFUL;
		else
			ret = RET_FAILED;

		unaddCwd();
	}
	cout("[Reverter]_CallSucoku_ret: %d\n", ret);
	return ret;
}
static int CallSudoku_I(void)
{
	addCwd(WkDataDir);
	{
		writeCSVFile("Input.csv", InputTbl);
	}
	unaddCwd();

	return CallSudoku();
}
static void ResetWkDataDir(void)
{
	recurClearDir(WkDataDir);
	copyDir(MasterDataDir, WkDataDir);
}

static void SetInputTblWH(uint w, uint h)
{
	autoList_t *row;
	uint rowidx;

	while(getCount(InputTbl) < h)
	{
		addElement(InputTbl, (uint)newList());
	}
	foreach(InputTbl, row, rowidx)
	{
		while(getCount(row) < w)
		{
			addElement(row, (uint)strx(""));
		}
	}
}
static char **CellAt(autoList_t *table, uint x, uint y)
{
	return (char **)directGetPoint(getList(table, y), x);
}
static void SetCell_x(autoList_t *table, uint x, uint y, char *cell)
{
	strzp_x(CellAt(table, x, y), cell);
}
static uint GetNumbMax(void)
{
	uint ret = 0;
	uint x;
	uint y;

	for(x = 0; x < Input_W; x++)
	for(y = 0; y < Input_H; y++)
	{
		m_maxim(ret, toValue(*CellAt(AnswerTbl, x, y)));
	}
	cout("GNM_ret: %u\n", ret);
	return ret;
}

static uint Input_X;
static uint Input_Y;

static int FIXY_IsRevertable(uint x, uint y)
{
	return !**CellAt(InputTbl, x, y) && **CellAt(AnswerTbl, x, y);
}
static void FindInputXY(void)
{
	uint refindCount = 0;
	uint count;
	uint x;
	uint y;

refind:
	count = 0;

	for(x = 0; x < Input_W; x++)
	for(y = 0; y < Input_H; y++)
	{
		if(FIXY_IsRevertable(x, y))
			count++;
	}
	errorCase(!count);

	cout("%u -> ", count);
	count = mt19937_rnd(count);
	cout("%u\n", count);

	for(x = 0; x < Input_W; x++)
	for(y = 0; y < Input_H; y++)
	{
		if(FIXY_IsRevertable(x, y))
		{
			if(!count)
				goto foundPos;

			count--;
		}
	}
	error();
foundPos:

	// refind-chk >

	if(KPN_IsKnown(x, y, 0))
	{
		if(refindCount < 10) // 上限適当
		{
			refindCount++;
			goto refind;
		}
		KPN_Reset();
	}
	KPN_Add(x, y, 0);

	// < refind-chk

	Input_X = x;
	Input_Y = y;

	cout("Input_X: %u\n", Input_X);
	cout("Input_Y: %u\n", Input_Y);
}
static int TryRevertRand(void) // ret: ? 続行すべき。
{
	uint timedOutCount = 0;
	uint timedOutNumb = 0; // dummy
	uint numb;

	FindInputXY();

	for(numb = 1; numb <= NumbMax && timedOutCount < 2; numb++)
	{
		SetCell_x(InputTbl, Input_X, Input_Y, xcout("%u", numb));

		switch(CallSudoku_I())
		{
		case RET_SUCCESSFUL:
			cout("解析成功！\n");
			return 0;

		case RET_FAILED:
			break;

		case RET_TIMED_OUT:
			timedOutCount++;
			timedOutNumb = numb;
			break;

		default:
			error();
		}
	}
	errorCase(!timedOutCount); // 全ての数字で失敗 == 入力データの誤り。

	if(timedOutCount == 1)
	{
		cout("復活成功！\n");
		SetCell_x(InputTbl, Input_X, Input_Y, xcout("%u", timedOutNumb));
	}
	else
	{
		cout("復活失敗...\n");
		SetCell_x(InputTbl, Input_X, Input_Y, strx(""));
	}
	return 1;
}
static void Reverter(char *dataDir)
{
	errorCase_m(!existFile(SUDOKU_EXE), "Sudoku.exe をビルドしてね。");
	errorCase_m(!existFile(SUDOKU_TOM_EXE), "SudokuTom.exe をビルドしてね。");

	coExecute("\"" SUDOKU_TOM_EXE "\" /KZ"); // Kill TOM Zombines

	dataDir = makeFullPath(dataDir);

	errorCase(!existDir(dataDir));

	MasterDataDir = dataDir;
	WkDataDir = makeTempDir(NULL);

	ResetWkDataDir();

	addCwd(WkDataDir);
	{
		errorCase_m(!existFile("Input.csv"), "入力データに Input.csv が無いよ！");

		InputTbl = readCSVFileTR("Input.csv");
		removeFile("Input.csv");
	}
	unaddCwd();

	Input_H = getCount(InputTbl);
	errorCase_m(!m_isRange(Input_H, 1, IMAX), "Input.csv 高さ == 0");
	Input_W = getCount(getList(InputTbl, 0));
	errorCase_m(!m_isRange(Input_W, 1, IMAX), "Input.csv 幅 == 0");

	switch(CallSudoku())
	{
	case RET_SUCCESSFUL:
		break;

	default:
		error();
	}

	addCwd(WkDataDir);
	{
		AnswerTbl = readCSVFileTR("Output.csv");
	}
	unaddCwd();

	{
		uint ans_w;
		uint ans_h;

		ans_h = getCount(AnswerTbl);
		errorCase(!m_isRange(ans_h, 1, IMAX));
		ans_w = getCount(getList(AnswerTbl, 0));
		errorCase(!m_isRange(ans_w, 1, IMAX));

		errorCase(ans_w < Input_W);
		errorCase(ans_h < Input_H);

		SetInputTblWH(ans_w, ans_h);

		Input_W = ans_w;
		Input_H = ans_h;
	}

	NumbMax = GetNumbMax();

	ResetWkDataDir();

	do
	{
		LOGPOS();
	}
	while(TryRevertRand());
	LOGPOS();

	addCwd(WkDataDir);
	{
		copyFile("Input.csv", getOutFile("Rev-Input.csv"));
	}
	unaddCwd();

	recurRemoveDir(WkDataDir);

	openOutDir();

	cout("+----+\n");
	cout("| OK |\n");
	cout("+----+\n");
}
int main(int argc, char **argv)
{
	mt19937_initCRnd();

	Reverter(hasArgs(1) ? nextArg() : c_dropDir());
}
