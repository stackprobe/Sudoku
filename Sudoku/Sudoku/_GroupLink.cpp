#include "all.h"

GroupLink_t *CreateGroupLink(void)
{
	GroupLink_t *i = nb_(GroupLink_t);

	return i;
}
void ReleaseGroupLink(GroupLink_t *i)
{
	memFree(i);
}
