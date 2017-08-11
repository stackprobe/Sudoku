/*
	{
		aaa

		<-- AAA()

		bbb

		<-- BBB()

		ccc

		<-- CCC()

		ddd
	}

	aaa は AAA() 終了時までに初期化(データがセット)される。
	bbb は BBB() 終了時までに初期化(データがセット)される。
	ccc は CCC() 終了時までに初期化(データがセット)される。

	... という風に記述したかった。
*/

typedef struct Dc_st
{
	HANDLE EvStop;
	HANDLE EvBruteForceStarted;

	char *ProcessPhase;

	char *FrameCsvFile;
	char *SquareCsvFile;
	char *GroupCsvFile;
	char *XInequalCsvFile;
	char *YInequalCsvFile;
	char *XJointCsvFile;
	char *YJointCsvFile;
	char *CondCsvFile;
	char *RelCsvFile;
	char *InputCsvFile;
	char *OutputCsvFile;
	char *ErrorCsvFile;
	char *LoadedDataCsvFile;

	int ForceOutputLoadedData;

	// <-- main()

	autoTable<Cell_t *> *Field;
	autoList<Group_t *> *Groups;

	// <-- LoadData()

	autoList<Cell_t *> *Cells;
	int MaxValueMax;
	int *Possibles;
}
Dc_t;

extern Dc_t Dc;
