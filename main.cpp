#include <iostream>

struct field
{
	//number for the field
	int num;
	//fixed number
	int fix;
};

field sudoku[9][9];

int checkfield()
{
	size_t row;
	size_t col;
	size_t subfield;

	//every row/col
	for (int i = 0; i < 9; ++i)
	{
		row = 0;
		col = 0;
		for (int j = 0; j < 9; ++j)
		{
			if (row & (1 << sudoku[i][j].num))
				return 1;
			if (sudoku[i][j].num)
				row |= (1 << sudoku[i][j].num);

			if (col & (1 << sudoku[j][i].num))
				return 1;
			if (sudoku[j][i].num)
				col |= (1 << sudoku[j][i].num);

		}
	}

	for (int i = 0; i < 3; ++i)
	{
		for (int j = 0; j < 3; ++j)
		{
			subfield = 0;
			for (int k = 0; k < 3; ++k)
			{
				for (int l = 0; l < 3; ++l)
				{
					if (subfield & (1 << sudoku[i * 3 + k][j * 3 + l].num))
						return 1;
					if (sudoku[i * 3 + k][j * 3 + l].num)
						subfield |= (1 << sudoku[i * 3 + k][j * 3 + l].num);
				}
			}
		}
	}

	return 0;
}

int checkfield(int field)
{
	uint16_t row;
	uint16_t col;

	uint8_t x = field / 9;
	uint8_t y = field % 9;

	//current row/col
	row = 0;
	col = 0;
	for (uint8_t j = 0; j < 9; ++j)
	{
		if (row & (1 << sudoku[x][j].num) & 0x3FE)	//exclude the zero
			return 1;
		row |= (1 << sudoku[x][j].num);

		if (col & (1 << sudoku[j][y].num) & 0x3FE)
			return 1;
		col |= (1 << sudoku[j][y].num);

	}

	//the 3x3 fields
	uint16_t subfield = 0;

	for (uint8_t k = 0; k < 3; ++k)
	{
		for (uint8_t l = 0; l < 3; ++l)
		{
			if (subfield & (1 << sudoku[(x/3) * 3 + k][(y/3) * 3 + l].num) & 0x3FE)
				return 1;
			subfield |= (1 << sudoku[(x/3) * 3 + k][(y/3) * 3 + l].num);
		}
	}

	return 0;
}

void printfield()
{

	//some nice output
	printf("-------------------------\n");
	for (int i = 0; i < 9; ++i)
	{
		if (!(i % 3) && i)
			printf("--------+-------+--------\n");
		printf("|");
		for (int j = 0; j < 9; ++j)
		{
			if (!(j % 3) && j)
				printf(" | ");
			else
				printf(" ");
			printf("%zu", sudoku[i][j].num);
		}
		std::cout << " |" << std::endl;
	}
	printf("-------------------------\n");
}

int main()
{
	//should produce 2208 solutions
	int input[9][9] = {
			{0, 0, 0, 7, 0, 0, 0, 0, 0},
			{1, 0, 0, 0, 0, 0, 0, 0, 0},
			{0, 0, 0, 4, 3, 0, 2, 7, 0},

			{0, 0, 0, 0, 0, 0, 0, 0, 6},
			{0, 0, 0, 5, 0, 9, 0, 0, 0},
			{0, 0, 0, 0, 0, 0, 4, 1, 8},

			{0, 0, 0, 0, 8, 1, 0, 0, 4},
			{0, 0, 2, 0, 0, 0, 0, 5, 0},
			{0, 4, 0, 0, 0, 0, 0, 0, 0}
	};

	//hard to brute force
/*	int input[9][9] = {
			{0, 0, 0, 0, 0, 0, 0, 0, 0},
			{0, 0, 0, 0, 0, 3, 0, 8, 5},
			{0, 0, 1, 0, 2, 0, 0, 0, 0},

			{0, 0, 0, 5, 0, 7, 0, 0, 0},
			{0, 0, 4, 0, 0, 0, 1, 0, 0},
			{0, 9, 0, 0, 0, 0, 0, 0, 0},

			{5, 0, 0, 0, 0, 0, 0, 7, 3},
			{0, 0, 2, 0, 1, 0, 0, 0, 0},
			{0, 0, 0, 0, 4, 0, 0, 0, 9}
	};*/

/*	int input[9][9] = {
			{0, 0, 0, 0, 0, 0, 0, 0, 0},
			{0, 0, 0, 0, 0, 0, 0, 0, 0},
			{0, 0, 0, 0, 0, 0, 0, 0, 0},

			{0, 0, 0, 0, 0, 0, 0, 0, 0},
			{0, 0, 0, 0, 0, 0, 0, 0, 0},
			{0, 0, 0, 0, 0, 0, 0, 0, 0},

			{0, 0, 0, 0, 0, 0, 0, 0, 0},
			{0, 0, 0, 0, 0, 0, 0, 0, 0},
			{0, 0, 0, 0, 0, 0, 0, 0, 0}
	};*/

	//copy input
	for (int x = 0; x < 9; ++x)
	{
		for (int y = 0; y < 9; ++y)
		{
			sudoku[x][y].num = input[x][y];
			if (sudoku[x][y].num)
				sudoku[x][y].fix = 1;
		}
	}


	if (checkfield())
	{
		std::cout << "error, input not valid" << std::endl;
		exit(1);
	}

	int field = 0;
	int reverting = 0;
	size_t solution = 0;

	while (1)
	{

		//finished with a solution
		if (field >= 81)
		{
			solution++;
			printf("Solution: %zu\n", solution);
			printfield();

			if(solution >= 100000)
				break;

			//start working on the last field again
			while (1)
			{
				field--;
				if(field < 0)
					break;
				if (sudoku[field / 9][field % 9].fix)
					continue;
				if (sudoku[field / 9][field % 9].num + 1 > 9)
				{
					sudoku[field / 9][field % 9].num = 0;
					continue;
				}
				sudoku[field / 9][field % 9].num++;
				break;
			}
		}

		//no more solutions
		if (field < 0)
			break;

		//jump over fix fields
		if (!reverting)
		{
			if (sudoku[field / 9][field % 9].fix)
			{
				field++;
				continue;
			}
		}
		else
		{
			if (sudoku[field / 9][field % 9].fix)
			{
				field--;
				continue;
			}
		}

		//We increase the current field
		if (sudoku[field / 9][field % 9].num + 1 > 9)
		{
			//somethings fishy
			reverting = 1;
			sudoku[field / 9][field % 9].num = 0;
			field--;
			continue;
		}
		else
		{
			reverting = 0;
			sudoku[field / 9][field % 9].num++;
		}

		//this didn't work, lets try again on this field
		if (checkfield(field))
		{
			reverting = 0;
			continue;
		}
			//looks good, next field
		else
		{
			field++;
			reverting = 0;
		}

	}

	printf("Finished, Solutions found: %zu\n", solution);
	return 0;
}
