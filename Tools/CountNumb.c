#include "C:\Factory\Common\all.h"
#include "C:\Factory\Common\Options\csv.h"

int main(int argc, char **argv)
{
	autoList_t *table = readCSVFile(hasArgs(1) ? nextArg() : c_dropFile());
	autoList_t *row;
	char *cell;
	uint rowidx;
	uint colidx;
	uint count = 0;

	foreach(table, row, rowidx)
	foreach(row, cell, colidx)
	{
		if(*cell)
			count++;
	}
	cout("%u\n", count);
}
