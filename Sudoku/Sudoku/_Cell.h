typedef struct Cell_st
{
	/*
		0 == 未確定
		1 ～ IMAX == 確定値
		1 ～ このセルが属する最小のグループのセル数 == 確定値 @ CheckData()
	*/
	int FixedValue;

	autoList<struct CellCond_st *> *Conds; // 条件の重複ナシ
	autoList<struct GroupLink_st *> *LinkedGroups; // 長さ: 0 == 枠外

	int MaxValue; // このセルが属する最小のグループのセル数 @ CheckData()

	// <-- LoadData()

	i2D_t Pos;
	int PosGot;

	int BFSC_Ordered;

	uchar *BF_KnownPossibles;
}
Cell_t;

Cell_t *CreateCell(void);
void ReleaseCell(Cell_t *cell);

i2D_t CellToPos(Cell_t *cell);
int CompCellPos(Cell_t *cell1, Cell_t *cell2);
int IsPossibleValueInCell(Cell_t *cell, int value);
int IsSolvableValueInCell(Cell_t *cell, int value);
autoList<struct Group_st *> *GetAroundGroups(Cell_t *cell);
