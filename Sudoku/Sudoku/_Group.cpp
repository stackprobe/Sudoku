#include "all.h"

Group_t *CreateGroup(void)
{
	Group_t *i = nb(Group_t);

	i->Cells = new autoList<Cell_t *>();

	return i;
}
void ReleaseGroup(Group_t *i)
{
	delete i->Cells;
	memFree(i);
}

int IsSameGroup(Group_t *group1, Group_t *group2) // (group1, group2): sorted
{
	if(group1->Cells->GetCount() != group2->Cells->GetCount())
		return 0;

	for(int index = 0; index < group1->Cells->GetCount(); index++)
	{
		Cell_t *cell1 = group1->Cells->GetElement(index);
		Cell_t *cell2 = group2->Cells->GetElement(index);

		if(cell1 != cell2)
			return 0;
	}
	return 1;
}
int IsFixedValueInGroup(Group_t *group, int value)
{
	for(int index = 0; index < group->Cells->GetCount(); index++)
	{
		if(group->Cells->GetElement(index)->FixedValue == value)
		{
			return 1;
		}
	}
	return 0;
}
void DeleteSameGroup(autoList<Group_t *> *groups)
{
restart:
	for(int ndx2 = 1; ndx2 < groups->GetCount(); ndx2++)
	for(int ndx1 = 0; ndx1 < ndx2; ndx1++)
	{
		if(groups->GetElement(ndx1) == groups->GetElement(ndx2))
		{
			groups->FastDesertElement(ndx2);
			goto restart;
		}
	}
}
int IsSolvableGroup(Group_t *group)
{
	for(int index = 0; index < group->Cells->GetCount(); index++)
	{
		Cell_t *cell = group->Cells->GetElement(index);

		if(cell->FixedValue == 0) // ? –¢Šm’è
		{
			for(int value = 1; value <= cell->MaxValue; value++)
				if(IsPossibleValueInCell(cell, value))
					goto foundPossValue;

			return 0;
foundPossValue:;
		}
	}
	for(int value = 1; value <= group->Cells->GetCount(); value++)
	{
		for(int index = 0; index < group->Cells->GetCount(); index++)
		{
			Cell_t *cell = group->Cells->GetElement(index);

			if(cell->FixedValue ?
				cell->FixedValue == value :
				IsPossibleValueInCell(cell, value)
				)
				goto foundPossCell;
		}
		return 0;
foundPossCell:;
	}
	return 1;
}
