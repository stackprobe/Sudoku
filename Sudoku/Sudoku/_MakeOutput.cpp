#include "all.h"

void MakeOutput(void)
{
	Dc.ProcessPhase = "Œ‹‰Ê‚Ìo—Í’†";

	autoTable<char *> *outputCsv = new autoTable<char *>(getNullString, (void (*)(char *))memFree);

	for(int x = 0; x < Dc.Field->GetWidth(); x++)
	for(int y = 0; y < Dc.Field->GetHeight(); y++)
	{
		int fixedValue = Dc.Field->GetCell(x, y)->FixedValue;

		if(fixedValue)
		{
			strz_x(*outputCsv->RefCellAt(x, y), xcout("%d", fixedValue));
		}
	}
	writeCsvFile(Dc.OutputCsvFile, outputCsv);
	delete outputCsv;
}
