typedef struct Group_st
{
	/*
		セルの重複ナシ
		固定値の重複ナシ
	*/
	autoList<Cell_t *> *Cells; // 長さ: 1～, { ref ... }

	// <-- LoadData();
}
Group_t;

Group_t *CreateGroup(void);
void ReleaseGroup(Group_t *i);

int IsSameGroup(Group_t *group1, Group_t *group2);
int IsFixedValueInGroup(Group_t *group, int value);
void DeleteSameGroup(autoList<Group_t *> *groups);
int IsSolvableGroup(Group_t *group);
