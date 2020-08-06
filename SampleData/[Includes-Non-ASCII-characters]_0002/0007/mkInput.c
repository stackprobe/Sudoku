/*
	mkInput.exe > Input.csv
*/

#include "C:\Factory\Common\all.h"
#include "C:\Factory\Common\Options\CRRandom.h"

static void SwapColRandom(int table[25][25])
{
	int x1;
	int x2;
	int y;
	int ys[25];

	zeroclear(ys);

	{
		int x = mt19937_rnd(5);
		int a = mt19937_rnd(5);
		int b = mt19937_rnd(4);

		if(a <= b)
			b++;

		x1 = x * 5 + a;
		x2 = x * 5 + b;
	}

	y = mt19937_rnd(25);

//	cout("first_swap: [%d][%d] <-> [%d][%d]\n", x1, y, x2, y);

	{
		int d = table[x1][y];
		int n = table[x2][y];

		ys[y] = 1;

		do
		{
			for(y = 0; y < 25; y++)
				if(table[x1][y] == n)
					break;

			errorCase(y == 25);
			ys[y] = 1;
			n = table[x2][y];
		}
		while(d != n);
	}

//	cout("ys: ");
	for(y = 0; y < 25; y++)
	{
//		cout("%d", ys[y]);
	}
//	cout("\n");

	for(y = 0; y < 25; y++)
		if(ys[y])
			m_swap(table[x1][y], table[x2][y], int);
}
static void Twist(int table[25][25])
{
	int x;
	int y;

	for(x = 1; x < 25; x++)
	for(y = 0; y < x; y++)
	{
		m_swap(table[x][y], table[y][x], int);
	}
}
static void Check(int table[25][25])
{
	int cs[25];
	int c;
	int x;
	int y;
	int xi;
	int yi;

	for(x = 0; x < 25; x++)
	for(y = 0; y < 25; y++)
	{
		c = table[x][y];
		errorCase(c < 1 || 25 < c);
	}
	for(x = 0; x < 25; x++)
	{
		zeroclear(cs);

		for(y = 0; y < 25; y++)
		{
			cs[table[x][y] - 1] = 1;
		}
		for(c = 0; c < 25; c++)
		{
			errorCase(!cs[c]);
		}
	}
	for(y = 0; y < 25; y++)
	{
		zeroclear(cs);

		for(x = 0; x < 25; x++)
		{
			cs[table[x][y] - 1] = 1;
		}
		for(c = 0; c < 25; c++)
		{
			errorCase(!cs[c]);
		}
	}
	for(x = 0; x < 25; x += 5)
	for(y = 0; y < 25; y += 5)
	{
		zeroclear(cs);

		for(xi = 0; xi < 5; xi++)
		for(yi = 0; yi < 5; yi++)
		{
			cs[table[x + xi][y + yi] - 1] = 1;
		}
		for(c = 0; c < 25; c++)
		{
			errorCase(!cs[c]);
		}
	}
}
int main(int argc, char **argv)
{
	int table[25][25];
	int x;
	int y;
	int count;

	mt19937_initCRnd();

	for(x = 0; x < 25; x++)
	for(y = 0; y < 25; y++)
	{
		int bx = x / 5;
		int ix = x % 5;
		int by = y / 5;
		int iy = y % 5;
		int nx;
		int ny;
		int n;

		nx = (ix + by) % 5;
		ny = (iy + bx) % 5;

		n = ny * 5 + nx + 1;

		table[x][y] = n;
	}
	for(count = 0; count < 3000000; count++)
	{
		SwapColRandom(table);
		Twist(table);
	}
	Check(table);

	for(y = 0; y < 25; y++)
	{
		for(x = 0; x < 25; x++)
		{
			cout("%d,", table[x][y]);
		}
		cout("\n");
	}
}
