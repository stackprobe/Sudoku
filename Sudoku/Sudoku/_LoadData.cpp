#include "all.h"

static void AddRectGroup(int x, int y, int w, int h)
{
	Group_t *group = CreateGroup();

	for(int xc = 0; xc < w; xc++)
	for(int yc = 0; yc < h; yc++)
	{
		group->Cells->AddElement(Dc.Field->RefCell(x + xc, y + yc));
	}
	Dc.Groups->AddElement(group);
}
static void AddRectGroupTable(int x, int y, int w, int h, int xNum, int yNum)
{
	for(int xc = 0; xc < xNum; xc++)
	for(int yc = 0; yc < yNum; yc++)
	{
		AddRectGroup(x + xc * w, y + yc * h, w, h);
	}
}
static void AddTaikakuGroup(int x, int y, int s)
{
	Group_t *group1 = CreateGroup();
	Group_t *group2 = CreateGroup();

	for(int c = 0; c < s; c++)
	{
		group1->Cells->AddElement(Dc.Field->RefCell(x + c, y + c));
		group2->Cells->AddElement(Dc.Field->RefCell(x + c, y + s - c - 1));
	}
	Dc.Groups->AddElement(group1);
	Dc.Groups->AddElement(group2);
}
static CellCondKind_t GetInequalKind(int value)
{
	switch(value)
	{
	default:
	case 0: return CCK_DUMMY;
	case 1: return CCK_LESSTHAN;
	case 2: return CCK_GREATERTHAN;
	}
}
static CellCondKind_t GetJointKind(int value)
{
	switch(value)
	{
	default:
	case 0: return CCK_DUMMY;
	case 1: return CCK_JOINTED;
	case 2: return CCK_UNJOINTED;
	}
}
static void ReadRelationTable(char *csvFile, CellCondKind_t (*getKindFunc)(int), int xRel, int yRel)
{
	if(existFile(csvFile))
	{
		autoTable<char *> *csv = readCsvFile(csvFile);

		for(int x = 0; x < csv->GetWidth(); x++)
		for(int y = 0; y < csv->GetHeight(); y++)
		{
			int csvCellValue = s2i(csv->GetCell(x, y));
			CellCondKind_t kind = getKindFunc(csvCellValue);

			if(kind != CCK_DUMMY)
			{
				CellCond_t *cond = CreateCellCond();

				cond->Kind = kind;
				cond->OtherCell = Dc.Field->RefCell(x + xRel, y + yRel);

				Dc.Field->RefCell(x, y)->Conds->AddElement(cond);
			}
		}
		delete csv;
	}
}
void LoadData(void)
{
	Dc.ProcessPhase = "データのロード中";

	Dc.Field = new autoTable<Cell_t *>(CreateCell, ReleaseCell);
	Dc.Groups = new autoList<Group_t *>();

	if(existFile(Dc.FrameCsvFile))
	{
		autoTable<char *> *frameCsv = readCsvFile(Dc.FrameCsvFile);

		int rowidx = 0;
		int frameNum = s2i(frameCsv->GetCell(0, rowidx++));

		errorCase(frameNum < 0 || IMAX < frameNum);

		for(int frameIndex = 0; frameIndex < frameNum; frameIndex++)
		{
			int x = s2i(frameCsv->GetCell(0, rowidx));
			int y = s2i(frameCsv->GetCell(1, rowidx++));
			int w = s2i(frameCsv->GetCell(0, rowidx));
			int h = s2i(frameCsv->GetCell(1, rowidx++));
			int t = s2i(frameCsv->GetCell(0, rowidx++));

			errorCase(x < 0 || IMAX < x);
			errorCase(y < 0 || IMAX < y);
			errorCase(w < 2 || IMAX < w);
			errorCase(h < 2 || IMAX < h);

			errorCase(IMAX / w < h); // ? IMAX < w * h
			int s = w * h;

			errorCase(IMAX - x < s);
			errorCase(IMAX - y < s);
			errorCase(IMAX / s < s); // ? IMAX < s * s

			AddRectGroupTable(x, y, s, 1, 1, s);
			AddRectGroupTable(x, y, 1, s, s, 1);
			AddRectGroupTable(x, y, w, h, h, w);

			if(t)
				AddTaikakuGroup(x, y, s);
		}
		delete frameCsv;
	}
	if(existFile(Dc.SquareCsvFile))
	{
		autoTable<char *> *squareCsv = readCsvFile(Dc.SquareCsvFile);

		int rowidx = 0;
		int squareNum = s2i(squareCsv->GetCell(0, rowidx++));

		errorCase(squareNum < 0 || IMAX < squareNum);

		for(int squareIndex = 0; squareIndex < squareNum; squareIndex++)
		{
			int x = s2i(squareCsv->GetCell(0, rowidx));
			int y = s2i(squareCsv->GetCell(1, rowidx++));
			int s = s2i(squareCsv->GetCell(0, rowidx++));
			int t = s2i(squareCsv->GetCell(0, rowidx++));

			errorCase(x < 0 || IMAX < x);
			errorCase(y < 0 || IMAX < y);
			errorCase(s < 1 || IMAX < s);

			errorCase(IMAX - x < s);
			errorCase(IMAX - y < s);
			errorCase(IMAX / s < s); // ? IMAX < s * s

			AddRectGroupTable(x, y, s, 1, 1, s);
			AddRectGroupTable(x, y, 1, s, s, 1);

			if(t)
				AddTaikakuGroup(x, y, s);
		}
		delete squareCsv;
	}
	if(existFile(Dc.GroupCsvFile))
	{
		autoTable<char *> *groupCsv = readCsvFile(Dc.GroupCsvFile);

		int rowidx = 0;
		int groupNum = s2i(groupCsv->GetCell(0, rowidx++));

		errorCase(groupNum < 0 || IMAX < groupNum);

		for(int groupIndex = 0; groupIndex < groupNum; groupIndex++)
		{
			Group_t *group = CreateGroup();
			int cellNum = s2i(groupCsv->GetCell(0, rowidx++));

			errorCase(cellNum < 0 || IMAX < cellNum);

			for(int cellIndex = 0; cellIndex < cellNum; cellIndex++)
			{
				int x = s2i(groupCsv->GetCell(0, rowidx));
				int y = s2i(groupCsv->GetCell(1, rowidx++));

				errorCase(x < 0 || IMAX < x);
				errorCase(y < 0 || IMAX < y);

				group->Cells->AddElement(Dc.Field->RefCell(x, y));
			}
			Dc.Groups->AddElement(group);
		}
		delete groupCsv;
	}
	if(existFile(Dc.XInequalCsvFile))
	{
		ReadRelationTable(Dc.XInequalCsvFile, GetInequalKind, 1, 0);
	}
	if(existFile(Dc.YInequalCsvFile))
	{
		ReadRelationTable(Dc.YInequalCsvFile, GetInequalKind, 0, 1);
	}
	if(existFile(Dc.XJointCsvFile))
	{
		ReadRelationTable(Dc.XJointCsvFile, GetJointKind, 1, 0);
	}
	if(existFile(Dc.YJointCsvFile))
	{
		ReadRelationTable(Dc.YJointCsvFile, GetJointKind, 0, 1);
	}
	if(existFile(Dc.CondCsvFile))
	{
		autoTable<char *> *condCsv = readCsvFile(Dc.CondCsvFile);

		for(int x = 0; x < condCsv->GetWidth(); x++)
		for(int y = 0; y < condCsv->GetHeight(); y++)
		{
			int condValue = s2i(condCsv->GetCell(x, y));

			errorCase(condValue < 0 || IMAX < condValue);

			CellCondKind_t kind = CCK_DUMMY;

			switch(condValue)
			{
			default:
			case 0: break;
			case 1: kind = CCK_ODDNUM; break;
			case 2: kind = CCK_EVENNUM; break;
			}
			if(kind)
			{
				CellCond_t *cond = CreateCellCond();

				cond->Kind = kind;

				Dc.Field->RefCell(x, y)->Conds->AddElement(cond);
			}
		}
		delete condCsv;
	}
	if(existFile(Dc.RelCsvFile))
	{
		autoTable<char *> *relCsv = readCsvFile(Dc.RelCsvFile);

		int rowidx = 0;
		int relNum = s2i(relCsv->GetCell(0, rowidx++));

		errorCase(relNum < 0 || IMAX < relNum);

		for(int relIndex = 0; relIndex < relNum; relIndex++)
		{
			int relValue = s2i(relCsv->GetCell(0, rowidx++));
			int x1 = s2i(relCsv->GetCell(0, rowidx));
			int y1 = s2i(relCsv->GetCell(1, rowidx++));
			int x2 = s2i(relCsv->GetCell(0, rowidx));
			int y2 = s2i(relCsv->GetCell(1, rowidx++));

			errorCase(relValue < 0 || IMAX < relValue);
			errorCase(x1 < 0 || IMAX < x1);
			errorCase(y1 < 0 || IMAX < y1);
			errorCase(x2 < 0 || IMAX < x2);
			errorCase(y2 < 0 || IMAX < y2);

			CellCondKind_t kind = CCK_DUMMY;

			switch(relValue)
			{
			default:
			case 0: break;
			case 1: kind = CCK_JOINTED; break;
			case 2: kind = CCK_UNJOINTED; break;
			case 3: kind = CCK_LESSTHAN; break;
			case 4: kind = CCK_GREATERTHAN; break;
			}
			if(kind)
			{
				CellCond_t *cond = CreateCellCond();

				cond->Kind = kind;
				cond->OtherCell = Dc.Field->RefCell(x2, y2);

				Dc.Field->RefCell(x1, y1)->Conds->AddElement(cond);
			}
		}
		delete relCsv;
	}
	if(existFile(Dc.InputCsvFile))
	{
		autoTable<char *> *inputCsv = readCsvFile(Dc.InputCsvFile);

		for(int x = 0; x < inputCsv->GetWidth(); x++)
		for(int y = 0; y < inputCsv->GetHeight(); y++)
		{
			int inputValue = s2i(inputCsv->GetCell(x, y));

			errorCase(inputValue < 0 || IMAX < inputValue);

			if(inputValue)
				Dc.Field->RefCell(x, y)->FixedValue = inputValue;
		}
		delete inputCsv;
	}

	remove(Dc.OutputCsvFile);
	remove(Dc.ErrorCsvFile);
	remove(Dc.LoadedDataCsvFile);

	CheckData();
}
