typedef struct GroupLink_st
{
	Group_t *Group; // ref
	int Index;

	// <-- LoadData()
}
GroupLink_t;

GroupLink_t *CreateGroupLink(void);
void ReleaseGroupLink(GroupLink_t *i);
