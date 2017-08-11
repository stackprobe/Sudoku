#include "all.h"

static oneObject(autoList<i2D_t>, new autoList<i2D_t>(), GetPosStack)

void AddErrorPos(i2D_t pos)
{
	GetPosStack()->AddElement(pos);
}
void UnaddErrorPos(void)
{
	GetPosStack()->UnaddElement();
}

void WriteErrorCsvFile(char *source, int lineno, char *function, char *errorcode)
{
	cout("ProcessPhase: %s\n", Dc.ProcessPhase ? Dc.ProcessPhase : "<null>");

	if(Dc.ErrorCsvFile == NULL) // ? 未設定
		return;

	GetPosStack()->Sort(compI2D);

	autoTable<char *> *errorCsv = new autoTable<char *>(getNullString, (void (*)(char *))memFree);
	int rowidx = 0;

	strz(*errorCsv->RefCellAt(0, rowidx++), Dc.ProcessPhase ? Dc.ProcessPhase : "不明なフェーズ");
	strz(*errorCsv->RefCellAt(0, rowidx++), errorcode);
	strz(*errorCsv->RefCellAt(0, rowidx), source);
	strz_x(*errorCsv->RefCellAt(1, rowidx), xcout("%d", lineno));
	strz(*errorCsv->RefCellAt(2, rowidx++), function);
	strz_x(*errorCsv->RefCellAt(0, rowidx++), xcout("%d", GetPosStack()->GetCount()));

	for(int index = 0; index < GetPosStack()->GetCount(); index++)
	{
		i2D_t pos = GetPosStack()->GetElement(index);

		strz_x(*errorCsv->RefCellAt(0, rowidx), xcout("%d", pos.X));
		strz_x(*errorCsv->RefCellAt(1, rowidx++), xcout("%d", pos.Y));
	}
#if 0
	strz(*errorCsv->RefCellAt(0, rowidx), source);
	strz_x(*errorCsv->RefCellAt(1, rowidx), xcout("%d", lineno));
	strz(*errorCsv->RefCellAt(2, rowidx), function);
	strz(*errorCsv->RefCellAt(3, rowidx++), reason);
#endif

	writeCsvFile(Dc.ErrorCsvFile, errorCsv);
	delete errorCsv;
}
void WriteLoadedDataCsvFile(void)
{
	if(Dc.LoadedDataCsvFile == NULL) // ? 未設定
		return;

	// LoadData() 実行中、実行前に呼び出される可能性があることに注意。

	autoTable<char *> *csv = new autoTable<char *>(getNullString, (void (*)(char *))memFree);

	if(Dc.Field != NULL)
	for(int x = 0; x < Dc.Field->GetWidth(); x++)
	for(int y = 0; y < Dc.Field->GetHeight(); y++)
	{
		Cell_t *cell = Dc.Field->GetCell(x, y);
		char *buff = getNullString();

		if(cell->FixedValue)
			buff = addLine_x(buff, xcout("number: %d", cell->FixedValue));

		for(int index = 0; index < cell->LinkedGroups->GetCount(); index++)
			buff = addLine_x(buff, xcout("included in [%08x] group", cell->LinkedGroups->GetElement(index)->Group));

		for(int index = 0; index < cell->Conds->GetCount(); index++)
		{
			CellCond_t *cond = cell->Conds->GetElement(index);
			i2D_t ocPos = { -1, -1 };

			if(cond->OtherCell)
				ocPos = CellToPos(cond->OtherCell);
			
			switch(cond->Kind)
			{
			case CCK_ODDNUM:
				buff = addLine(buff, "condition: odd-number");
				break;

			case CCK_EVENNUM:
				buff = addLine(buff, "condition: even-number");
				break;

			case CCK_LESSTHAN:
				buff = addLine_x(buff, xcout("relation: (this cell) < (%d, %d)", ocPos.X, ocPos.Y));
				break;

			case CCK_GREATERTHAN:
				buff = addLine_x(buff, xcout("relation: (this cell) > (%d, %d)", ocPos.X, ocPos.Y));
				break;

			case CCK_JOINTED:
				buff = addLine_x(buff, xcout("relation: (this cell) jointed (%d, %d)", ocPos.X, ocPos.Y));
				break;

			case CCK_UNJOINTED:
				buff = addLine_x(buff, xcout("relation: (this cell) not-jointed (%d, %d)", ocPos.X, ocPos.Y));
				break;
			}
		}
		strz_x(*csv->RefCellAt(x, y), buff);
	}
	writeCsvFile(Dc.LoadedDataCsvFile, csv);
	csv->Resize(0, 0);

	int baseX = 0;
	int baseY = 1;

	strz(*csv->RefCellAt(baseX, baseY++), "groups:");

	if(Dc.Groups != NULL)
	for(int index = 0; index < Dc.Groups->GetCount(); index++)
	{
		Group_t *group = Dc.Groups->GetElement(index);

		strz_x(*csv->RefCellAt(baseX + index, baseY), xcout("[%08x]", group));

		for(int cellIndex = 0; cellIndex < group->Cells->GetCount(); cellIndex++)
		{
			i2D_t pos = CellToPos(group->Cells->GetElement(cellIndex));

			strz_x(*csv->RefCellAt(baseX + index, baseY + 1 + cellIndex), xcout("(%d, %d)", pos.X, pos.Y));
		}
	}
	writeCsvFile(Dc.LoadedDataCsvFile, csv, "at");
	delete csv;
}
