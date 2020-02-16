#include "all.h"

CellCond_t *CreateCellCond(void)
{
	CellCond_t *i = nb_(CellCond_t);

	return i;
}
void ReleaseCellCond(CellCond_t *i)
{
	memFree(i);
}

int IsSameCellCond(CellCond_t *cond1, CellCond_t *cond2)
{
	return
		cond1->Kind == cond2->Kind &&
		cond1->OtherCell == cond2->OtherCell;
}
int IsPossibleValueInCellCond(CellCond_t *cond, int value)
{
	switch(cond->Kind)
	{
	case CCK_ODDNUM:
		return value % 2 == 1;

	case CCK_EVENNUM:
		return value % 2 == 0;

	case CCK_LESSTHAN:
		if(cond->OtherCell->FixedValue) // ? 確定
		{
			return value < cond->OtherCell->FixedValue;
		}
		break;

	case CCK_GREATERTHAN:
		if(cond->OtherCell->FixedValue) // ? 確定
		{
			return cond->OtherCell->FixedValue < value;
		}
		break;

	case CCK_JOINTED:
		if(cond->OtherCell->FixedValue) // ? 確定
		{
			return abs(value - cond->OtherCell->FixedValue) == 1;
		}
		break;

	case CCK_UNJOINTED:
		if(cond->OtherCell->FixedValue) // ? 確定
		{
			return 2 <= abs(value - cond->OtherCell->FixedValue);
		}
		break;
	}
	return 1; // ? 不明 -> 可能性はある。
}
int IsPossibleValueInCellConds(autoList<CellCond_t *> *conds, int value)
{
	for(int index = 0; index < conds->GetCount(); index++)
	{
		if(!IsPossibleValueInCellCond(conds->GetElement(index), value)) // ? 不可
		{
			return 0;
		}
	}
	return 1;
}
