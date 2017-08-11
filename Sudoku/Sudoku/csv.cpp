#include "all.h"

autoTable<char *> *readCsvFile(char *file)
{
	FILE *fp = fileOpen(file, "rt");
	autoList<char> *cellBuff = new autoList<char>();
	autoTable<char *> *table = new autoTable<char *>((char *(*)(void))getZero, (void (*)(char *))memFree);
	int rowidx = 0;
	int colidx = 0;
	int chr;

	while((chr = readChar(fp)) != EOF)
	{
		if(chr == '\"')
		{
			for(; ; )
			{
				chr = readChar(fp);

				if(chr == EOF) // セルの途中でファイルが終了した。-> セルとファイルの終端と見なす。
					break;

				if(chr == '\"')
				{
					chr = readChar(fp);

					if(chr != '\"')
						break;
				}
				cellBuff->AddElement(chr);
			}
		}
		else
		{
			for(; ; )
			{
				if(chr == ',' || chr == '\n')
					break;

				cellBuff->AddElement(chr);
				chr = readChar(fp);

				if(chr == EOF)
					break;
			}
		}
		cellBuff->AddElement('\0');
		table->PutCell(colidx, rowidx, cellBuff->UnbindBuffer());

		if(chr == '\n')
		{
			rowidx++;
			colidx = 0;
		}
		else
			colidx++;
	}
	fileClose(fp);
	delete cellBuff;

	// NULL -> strx("")
	{
		for(int x = 0; x < table->GetWidth(); x++)
		for(int y = 0; y < table->GetHeight(); y++)
		{
			char **p = table->CellAt(x, y);

			if(!*p)
				*p = strx("");
		}
		table->SetCallBack(getNullString);
	}
	// Trim
	{
		int xMax = -1;
		int yMax = -1;

#if 1
		xMax = table->GetWidth() - 1;
		yMax = table->GetHeight() - 1;

		while(0 <= xMax)
		{
			int x = xMax;
			int y;

			for(y = 0; y <= yMax; y++)
				if(*table->GetCell(x, y))
					break;

			if(y <= yMax)
				break;

			xMax--;
		}
		while(0 <= yMax)
		{
			int x;
			int y = yMax;

			for(x = 0; x <= xMax; x++)
				if(*table->GetCell(x, y))
					break;

			if(x <= xMax)
				break;

			yMax--;
		}
#else // SAME CODE
		for(int x = 0; x < table->GetWidth(); x++)
		for(int y = 0; y < table->GetHeight(); y++)
		{
			if(*table->GetCell(x, y))
			{
				m_maxim(xMax, x);
				m_maxim(yMax, y);
			}
		}
#endif
		table->Resize(xMax + 1, yMax + 1);
	}
	return table;
}
void writeCsvFile(char *file, autoTable<char *> *table, char *mode)
{
	FILE *fp = fileOpen(file, mode);

	for(int rowidx = 0; rowidx < table->GetHeight(); rowidx++)
	{
		for(int colidx = 0; colidx < table->GetWidth(); colidx++)
		{
			char *cell = table->GetCell(colidx, rowidx);

			if(colidx)
				writeChar(fp, ',');

			if(strchr(cell, '"') || strchr(cell, ',') || strchr(cell, '\n')) // ? "" 必要
			{
				writeChar(fp, '"');

				for(char *p = cell; *p; p++)
				{
					if(*p == '"')
					{
						writeChar(fp, '"');
						writeChar(fp, '"');
					}
					else
						writeChar(fp, *p);
				}
				writeChar(fp, '"');
			}
			else
				writeToken(fp, cell);
		}
		writeChar(fp, '\n');
	}
	fileClose(fp);
}
