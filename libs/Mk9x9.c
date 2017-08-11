#include "Mk9x9.h"

#define BLOCK_WH_MAX 10
#define INPUT_TABLE_WH_MAX ((BLOCK_WH_MAX) * (BLOCK_WH_MAX))
#define RAND_BUFF_SIZE 8000000

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
static uint GetCellValue(autoList_t *rows, uint rowidx, uint colidx)
{
	if(rowidx < getCount(rows) && colidx < getCount(getList(rows, rowidx)))
	{
		return toValue(getLine(getList(rows, rowidx), colidx));
	}
	return UINTMAX;
}
static int Is9x9ByFrameCsvFile(char *frameCsvFile)
{
	autoList_t *rows = readCSVFile(frameCsvFile);
	int ret;

	ret =
		GetCellValue(rows, 0, 0) == 1 &&
		GetCellValue(rows, 1, 0) == 0 &&
		GetCellValue(rows, 1, 1) == 0 &&
		2 <= GetCellValue(rows, 2, 0) &&
		GetCellValue(rows, 2, 0) <= BLOCK_WH_MAX &&
		GetCellValue(rows, 2, 0) == GetCellValue(rows, 2, 1) &&
		GetCellValue(rows, 3, 0) == 0;

	releaseDim(rows, 2);
	return ret;
}
int Is9x9(char *dataDir)
{
	int ret;

	addCwd(dataDir);

	ret =
		existFile("Frame.csv") &&
		Is9x9ByFrameCsvFile("Frame.csv") &&
		!existFile("Square.csv") &&
		!existFile("Group.csv") &&
		!existFile("XInequal.csv") &&
		!existFile("YInequal.csv") &&
		!existFile("XJoint.csv") &&
		!existFile("YJoint.csv") &&
		!existFile("Condition.csv") &&
		!existFile("Relation.csv");

	unaddCwd();

	cout("Is9x9_ret: %d\n", ret);
	return ret;
}
static uint GetBlockWH(char *frameCsvFile)
{
	autoList_t *rows = readCSVFile(frameCsvFile);
	uint blockWH;

	blockWH = GetCellValue(rows, 2, 0);
	releaseDim(rows, 2);
	return blockWH;
}

static uint BlockWH;
static uint InputTableWH;
static uint InputTable[INPUT_TABLE_WH_MAX][INPUT_TABLE_WH_MAX];

static void MkInputTable(void)
{
	uint x;
	uint y;

	for(x = 0; x < InputTableWH; x++)
	for(y = 0; y < InputTableWH; y++)
	{
		uint bx = x / BlockWH;
		uint by = y / BlockWH;
		uint ix = x % BlockWH;
		uint iy = y % BlockWH;
		uint nx;
		uint ny;
		uint n;

		nx = (ix + by) % BlockWH;
		ny = (iy + bx) % BlockWH;

		n = nx * BlockWH + ny + 1;

		InputTable[x][y] = n;
	}
}
static void ShuffleColRandom(void)
{
	uint x1;
	uint x2;
	uint y;
	uint n;
	uint d;
	int ys[INPUT_TABLE_WH_MAX];

	{
		uint x = GetRand_Mod(BlockWH) * BlockWH;
		uint a = GetRand_Mod(BlockWH);
		uint b = GetRand_Mod(BlockWH - 1);

		if(x <= b)
			b++;

		x1 = x + a;
		x2 = x + b;
	}

	y = GetRand_Mod(InputTableWH);

	n = InputTable[x2][y];
	d = InputTable[x1][y];

	zeroclear(ys);
	ys[y] = 1;

	do
	{
		for(y = 0; y < InputTableWH; y++)
			if(InputTable[x1][y] == n)
				break;

		errorCase(y == InputTableWH); // 2bs
		n = InputTable[x2][y];
		ys[y] = 1;
	}
	while(n != d);

	for(y = 0; y < InputTableWH; y++)
		if(ys[y])
			m_swap(InputTable[x1][y], InputTable[x2][y], uint);
}
static void TwistInputTable(void)
{
	uint x;
	uint y;

	for(x = 1; x < InputTableWH; x++)
	for(y = 0; y < x; y++)
	{
		m_swap(InputTable[x][y], InputTable[y][x], uint);
	}
}
static void ShuffleInputTable(void)
{
	uint count;

	for(count = 3000000; count; count--)
	{
		ShuffleColRandom();
		TwistInputTable();
	}
}
static void WrInputCsvFile(char *inputCsvFile)
{
	FILE *fp = fileOpen(inputCsvFile, "wt");
	uint x;
	uint y;

	for(y = 0; y < InputTableWH; y++)
	{
		for(x = 0; x < InputTableWH; x++)
		{
			writeToken_x(fp, xcout("%u,", InputTable[x][y]));
		}
		writeToken(fp, "\n");
	}
	fileClose(fp);
}
void Mk9x9InputCsvFile(char *dataDir)
{
	char *frameCsvFile;
	char *inputCsvFile;

	frameCsvFile = combine(dataDir, "Frame.csv");
	inputCsvFile = combine(dataDir, "Input.csv");

	BlockWH = GetBlockWH(frameCsvFile);
	InputTableWH = BlockWH * BlockWH;

	MkInputTable();
	ShuffleInputTable();
	WrInputCsvFile(inputCsvFile);

	memFree(frameCsvFile);
	memFree(inputCsvFile);
}
void Mk9x9Test(uint bwh, char *wFile)
{
	BlockWH = bwh;
	InputTableWH = BlockWH * BlockWH;

	errorCase(!m_isRange(BlockWH, 2, BLOCK_WH_MAX));

	MkInputTable();
	ShuffleInputTable();
	WrInputCsvFile(wFile);
}
