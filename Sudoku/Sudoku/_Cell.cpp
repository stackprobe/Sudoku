#include "all.h"

Cell_t *CreateCell(void)
{
	Cell_t *i = nb_(Cell_t);

	i->Conds = new autoList<CellCond_t *>();
	i->LinkedGroups = new autoList<GroupLink_t *>();

	return i;
}
void ReleaseCell(Cell_t *i)
{
	releaseList(i->Conds, ReleaseCellCond);
	releaseList(i->LinkedGroups, ReleaseGroupLink);

	memFree(i);
}

i2D_t CellToPos(Cell_t *cell)
{
	if(cell->PosGot)
		goto retPos;

	for(int x = 0; x < Dc.Field->GetWidth(); x++)
	for(int y = 0; y < Dc.Field->GetHeight(); y++)
	{
		if(Dc.Field->GetCell(x, y) == cell)
		{
			cell->Pos.X = x;
			cell->Pos.Y = y;
			cell->PosGot = 1;

			goto retPos;
		}
	}
	error(); // not found

retPos:
	return cell->Pos;
}
int CompCellPos(Cell_t *cell1, Cell_t *cell2)
{
	return compI2D(CellToPos(cell1), CellToPos(cell2));
}
int IsPossibleValueInCell(Cell_t *cell, int value)
{
	errorCase_m(cell->FixedValue, "数字が確定しているセルに数字を記入しようとした。");
	errorCase_m(cell->LinkedGroups->GetCount() == 0, "枠外のセルに数字を記入しようとした。");

	if(value < 1 || cell->MaxValue < value) // ? ここに入る値ではない。
		return 0;

	for(int index = 0; index < cell->LinkedGroups->GetCount(); index++)
	{
		GroupLink_t *gl = cell->LinkedGroups->GetElement(index);

		for(int cellIndex = 0; cellIndex < gl->Group->Cells->GetCount(); cellIndex++)
		{
			if(cellIndex != gl->Index) // ? 自分ではない。
			{
				Cell_t *glCell = gl->Group->Cells->GetElement(cellIndex);

				if(glCell->FixedValue == value) // ? 既に使われている。
				{
					return 0;
				}
			}
		}
	}
	return IsPossibleValueInCellConds(cell->Conds, value);
}
int IsSolvableValueInCell(Cell_t *cell, int value)
{
	autoList<Group_t *> *groups = GetAroundGroups(cell);
	int result = 1;

	cell->FixedValue = value; // 仮定

	for(int index = 0; index < groups->GetCount(); index++)
	{
		Group_t *group = groups->GetElement(index);

		if(!IsSolvableGroup(group)) // ? クリア不可
		{
			result = 0;
			break;
		}
	}
	cell->FixedValue = 0; // 元に戻す

	delete groups;
	return result;
}
autoList<Group_t *> *GetAroundGroups(Cell_t *cell)
{
	autoList<Group_t *> *groups = new autoList<Group_t *>();

	for(int index = 0; index < cell->LinkedGroups->GetCount(); index++)
	{
		GroupLink_t *gl = cell->LinkedGroups->GetElement(index);

		groups->AddElement(gl->Group); // 2bs

		for(int cellIndex = 0; cellIndex < gl->Group->Cells->GetCount(); cellIndex++)
		{
			Cell_t *glCell = gl->Group->Cells->GetElement(cellIndex);

			for(int glIndex = 0; glIndex < glCell->LinkedGroups->GetCount(); glIndex++)
			{
				groups->AddElement(glCell->LinkedGroups->GetElement(glIndex)->Group);
			}
		}
	}
	DeleteSameGroup(groups);
	return groups;
}
