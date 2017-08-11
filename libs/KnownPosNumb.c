#include "KnownPosNumb.h"

static autoList_t *Knowns;

void KPN_Reset(void)
{
	releaseDim(Knowns, 1);
	Knowns = NULL;
}
static char *ToLine(uint x, uint y, uint numb)
{
	return xcout("%u_%u_%u", x, y, numb);
}
int KPN_IsKnown(uint x, uint y, uint numb)
{
	char *line;
	int ret;

	if(!Knowns)
		return 0;

	line = ToLine(x, y, numb);
	ret = findLine(Knowns, line) < getCount(Knowns);
	memFree(line);
	return ret;
}
void KPN_Add(uint x, uint y, uint numb)
{
	if(!Knowns)
		Knowns = newList();

	addElement(Knowns, (uint)ToLine(x, y, numb));
}
