/*
	mkGroup.exe > Group.csv
*/

#include "C:\Factory\Common\all.h"

int main(int argc, char **argv)
{
	int n[] = { 1, 4, 7, 10, 13, 16, 19, 22, 25 };
	int x;
	int y;
	int xs;
	int ys;

	cout("27\n");

	for(y = 0; y < 9; y++)
	{
		cout("9\n");

		for(x = 0; x < 9; x++)
		{
			cout("%d,%d\n", n[x], n[y]);
		}
	}
	for(x = 0; x < 9; x++)
	{
		cout("9\n");

		for(y = 0; y < 9; y++)
		{
			cout("%d,%d\n", n[x], n[y]);
		}
	}
	for(y = 0; y <= 6; y += 3)
	for(x = 0; x <= 6; x += 3)
	{
		cout("9\n");

		for(ys = 0; ys < 3; ys++)
		for(xs = 0; xs < 3; xs++)
		{
			cout("%d,%d\n", n[x + xs], n[y + ys]);
		}
	}
}
