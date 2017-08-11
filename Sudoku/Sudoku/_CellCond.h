typedef enum CellCondKind_et
{
	CCK_DUMMY,       // 初期値
	CCK_ODDNUM,      // 接続ナシ
	CCK_EVENNUM,     // 接続ナシ
	CCK_LESSTHAN,    // 接続アリ
	CCK_GREATERTHAN, // 接続アリ
	CCK_JOINTED,     // 接続アリ
	CCK_UNJOINTED,   // 接続アリ
}
CellCondKind_t;

typedef struct CellCond_st
{
	CellCondKind_t Kind;
	Cell_t *OtherCell; // NULL == 接続ナシ, ref

	// <-- LoadData()
}
CellCond_t;

CellCond_t *CreateCellCond(void);
void ReleaseCellCond(CellCond_t *i);

int IsSameCellCond(CellCond_t *cond1, CellCond_t *cond2);
int IsPossibleValueInCellCond(CellCond_t *cond, int value);
int IsPossibleValueInCellConds(autoList<CellCond_t *> *conds, int value);
