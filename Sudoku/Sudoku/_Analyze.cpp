#include "all.h"

static void CheckChuudan(void)
{
	DWORD ret = WaitForSingleObject(Dc.EvStop, 0);

	if(ret == WAIT_OBJECT_0)
	{
		error_m("中断しました。");
	}
}
static int BFSC_GetScore(Cell_t *cell)
{
	int score = 0;

	for(int index = 0; index < cell->LinkedGroups->GetCount(); index++)
	{
		GroupLink_t *gl = cell->LinkedGroups->GetElement(index);

		for(int cellIndex = 0; cellIndex < gl->Group->Cells->GetCount(); cellIndex++)
		{
			if(cellIndex != gl->Index) // ? 自分ではない。
			{
				Cell_t *glCell = gl->Group->Cells->GetElement(cellIndex);

				if(glCell->BFSC_Ordered)
				{
					score += 10000;
				}
			}
		}
	}
	for(int value = 1; value <= cell->MaxValue; value++)
	{
		if(IsPossibleValueInCell(cell, value))
		{
			score -= 100;
		}
	}
	return score + cell->Conds->GetCount();
}
static void BF_SortCells(void)
{
	autoList<Cell_t *> *newOrder = new autoList<Cell_t *>();

	while(Dc.Cells->GetCount())
	{
		int bestPos = 0;
		int bestScore = BFSC_GetScore(Dc.Cells->GetElement(0));

		for(int index = 1; index < Dc.Cells->GetCount(); index++)
		{
			int score = BFSC_GetScore(Dc.Cells->GetElement(index));

			if(bestScore < score)
			{
				bestPos = index;
				bestScore = score;
			}
		}

		{
			Cell_t *cell = Dc.Cells->FastDesertElement(bestPos);
			newOrder->AddElement(cell);
			cell->BFSC_Ordered = 1;
		}
	}
	Dc.Cells->Change(newOrder);
	delete newOrder;
}
static void BruteForceSearch(void)
{
	Dc.ProcessPhase = "解析フェーズ3(BRUTE_FORCE)";

	for(int index = 0; index < Dc.Cells->GetCount(); index++)
	{
		Cell_t *cell = Dc.Cells->GetElement(index);

		if(cell->FixedValue) // ? 確定 -> 除外
		{
			Dc.Cells->FastDesertElement(index);
			index--;
		}
	}
	BF_SortCells();

	for(int index = 0; index < Dc.Cells->GetCount(); index++)
	{
		Cell_t *cell = Dc.Cells->GetElement(index);

		cell->BF_KnownPossibles = na(uchar, cell->MaxValue);

		for(int value = 1; value <= cell->MaxValue; value++)
		{
			cell->BF_KnownPossibles[value - 1] = IsPossibleValueInCell(cell, value);
		}
	}
	int seekPos = 0;
	int maxSeekPos = 0;

	SetEvent(Dc.EvBruteForceStarted);

	while(seekPos < Dc.Cells->GetCount())
	{
		Cell_t *cell = Dc.Cells->GetElement(seekPos);
		int value = cell->FixedValue;

		cell->FixedValue = 0;

		for(; ; )
		{
			value++;

			if(cell->MaxValue < value)
			{
				seekPos--;

				if(seekPos < 0) // ? 解ナシ
				{
					AddErrorPos(CellToPos(Dc.Cells->GetElement(maxSeekPos)));
					error_m("解無し。");
				}
				break;
			}
			if(cell->BF_KnownPossibles[value - 1] && IsPossibleValueInCell(cell, value))
			{
				cell->FixedValue = value;
				seekPos++;

				if(maxSeekPos < seekPos)
					cout("%.2f percent completed.\n", 50.0 + seekPos * 49.99 / Dc.Cells->GetCount());

				m_maxim(maxSeekPos, seekPos);
				break;
			}

			{
				static uint c;

				if((c++ & 0x3ffffff) == 0)
				{
					CheckChuudan();
				}
			}
		}
	}
	cout("100.00 percent completed.\n");
}
void Analyze(void)
{
	Dc.ProcessPhase = "解析フェーズ";

	Dc.Cells = new autoList<Cell_t *>();
	Dc.MaxValueMax = 0;

	for(int x = 0; x < Dc.Field->GetWidth(); x++)
	for(int y = 0; y < Dc.Field->GetHeight(); y++)
	{
		Cell_t *cell = Dc.Field->GetCell(x, y);

		if(cell->LinkedGroups->GetCount()) // ? 枠内
		{
			Dc.Cells->AddElement(cell);
			m_maxim(Dc.MaxValueMax, cell->MaxValue);
		}
	}
	Dc.Possibles = na(int, Dc.MaxValueMax);

	int presume = 0;

	for(; ; )
	{
		{
			int fixedCnt = 0;

			for(int index = 0; index < Dc.Cells->GetCount(); index++)
				if(Dc.Cells->GetElement(index)->FixedValue) // ? 確定
					fixedCnt++;

			cout("%.2f percent completed.\n", fixedCnt * 50.0 / Dc.Cells->GetCount());
		}

		int modified = 0;

		for(int index = 0; index < Dc.Cells->GetCount(); index++)
		{
			Cell_t *cell = Dc.Cells->GetElement(index);

			if(cell->FixedValue == 0) // ? 未確定
			{
				memset(Dc.Possibles, 0xff, cell->MaxValue * sizeof(int));

				for(int glIndex = 0; glIndex < cell->LinkedGroups->GetCount(); glIndex++)
				{
					GroupLink_t *gl = cell->LinkedGroups->GetElement(glIndex);

					for(int cellIndex = 0; cellIndex < gl->Group->Cells->GetCount(); cellIndex++)
					{
						if(cellIndex != gl->Index) // ? 自分ではない。
						{
							Cell_t *glCell = gl->Group->Cells->GetElement(cellIndex);

							if(glCell->FixedValue) // ? 確定
							{
								Dc.Possibles[glCell->FixedValue - 1] = 0; // cell->MaxValue 以降にも書いてしまうが、バッファはあるので問題無い。
							}
						}
					}
				}
				for(int possPos = 0; possPos < cell->MaxValue; possPos++)
				{
					if(Dc.Possibles[possPos])
					{
						Dc.Possibles[possPos] = IsPossibleValueInCellConds(cell->Conds, possPos + 1);
					}
				}

				{
					int possCnt = 0;
					int possPos;

					for(int posIndex = 0; posIndex < cell->MaxValue; posIndex++)
					{
						if(Dc.Possibles[posIndex])
						{
							possCnt++;

							if(possCnt == 2)
								break;

							possPos = posIndex;
						}
					}
					if(possCnt == 0) // ? このセルに入る値が無い。== 解ナシ
					{
						AddErrorPos(CellToPos(cell));
						error_m("解無し。(この升に入る値が無い)");
					}
					if(possCnt == 1)
					{
						cell->FixedValue = possPos + 1;
						modified = 1;
					}
				}
			}
		}
		for(int index = 0; index < Dc.Groups->GetCount(); index++)
		{
			Group_t *group = Dc.Groups->GetElement(index);

			for(int value = 1; value <= group->Cells->GetCount(); value++)
			{
				if(!IsFixedValueInGroup(group, value))
				{
					int possCnt = 0;
					int possPos;

					for(int cellIndex = 0; cellIndex < group->Cells->GetCount(); cellIndex++)
					{
						Cell_t *cell = group->Cells->GetElement(cellIndex);

						if(cell->FixedValue == 0 && IsPossibleValueInCell(cell, value) && (!presume || IsSolvableValueInCell(cell, value)))
						{
							possCnt++;

							if(possCnt == 2)
								break;

							possPos = cellIndex;
						}
					}
					if(possCnt == 0) // ? value が入るセルが group に存在しない。
					{
						for(int cellIndex = 0; cellIndex < group->Cells->GetCount(); cellIndex++)
						{
							AddErrorPos(CellToPos(group->Cells->GetElement(cellIndex)));
						}
						error_m("解無し。(このグループに入らない数字がある)");
					}
					if(possCnt == 1)
					{
						group->Cells->GetElement(possPos)->FixedValue = value;
						modified = 1;
					}
				}
			}
		}
		if(!modified)
		{
			if(presume)
				break;

			Dc.ProcessPhase = "解析フェーズ2(PRESUME)";
			presume = 1;
		}
		CheckChuudan();
	}
	BruteForceSearch();
}
