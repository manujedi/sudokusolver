#include <iostream>
#include <assert.h>

//#define visualize

struct field
{
	//number for the field
	int num;
	//fixed number
	int fix;
	//pointer to the current number which is tested
	uint8_t* testing_num;
};

field sudoku[81];

uint8_t pos_num[81 * 10];

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
			if (row & (1 << sudoku[i * 9 + j].num))
				return 1;
			if (sudoku[i * 9 + j].num)
				row |= (1 << sudoku[i * 9 + j].num);

			if (col & (1 << sudoku[i * 9 + j].num))
				return 1;
			if (sudoku[i * 9 + j].num)
				col |= (1 << sudoku[i * 9 + j].num);

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
					if (subfield & (1 << sudoku[(i * 3 + k) * 9 + (j * 3 + l)].num))
						return 1;
					if (sudoku[(i * 3 + k) * 9 + (j * 3 + l)].num)
						subfield |= (1 << sudoku[(i * 3 + k) * 9 + (j * 3 + l)].num);
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

	uint8_t x = field % 9;
	uint8_t y = field / 9;

	//col
	col = 0;
	for (int i = field % 9; i < 81; i += 9)
	{
		if (col & (1 << sudoku[i].num) & 0x3FE)
			return 1;
		col |= (1 << sudoku[i].num);
	}

	row = 0;
	uint8_t start = (field / 9) * 9;
	for (int i = 0; i < 9; i++)
	{
		if (row & (1 << sudoku[start + i].num) & 0x3FE)
			return 1;
		row |= (1 << sudoku[start + i].num);
	}

	//the 3x3 fields
	uint16_t subfield = 0;

	x = (x / 3) * 3;
	y = (y / 3) * 3;

	for (uint8_t k = 0; k < 3; ++k)
	{
		for (uint8_t l = 0; l < 3; ++l)
		{
			if (subfield & (1 << sudoku[(y + k) * 9 + (x + l)].num) & 0x3FE)
				return 1;
			subfield |= (1 << sudoku[(y + k) * 9 + (x + l)].num);
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
			printf("%zu", sudoku[i * 9 + j].num);
		}
		std::cout << " |" << std::endl;
	}
	printf("-------------------------\n");
}

void setpos(int field, int num)
{

	uint8_t* tmp = &pos_num[field * 10];
	uint16_t helper = 0;

	//store possible numbers
	while (*tmp)
	{
		helper |= (1 << *tmp);
		tmp++;
	}

	//add new num
	helper |= (1 << num);

	//reset pointer
	tmp = &pos_num[field * 10];

	for (int i = 1; i < 10; ++i)
	{
		if (helper & (1 << i))
		{
			*tmp = i;
			tmp++;
		}
	}
	*tmp = 0;

}

void unsetpos(int field, int num)
{

	uint8_t* tmp = &pos_num[field * 10];
	uint16_t helper = 0;

	//store possible numbers
	while (*tmp)
	{
		helper |= (1 << *tmp);
		tmp++;
	}

	//delete num
	helper &= ~(1 << num);

	//reset pointer
	tmp = &pos_num[field * 10];

	for (int i = 1; i < 10; ++i)
	{
		if (helper & (1 << i))
		{
			*tmp = i;
			tmp++;
		}
	}
	*tmp = 0;
}

int ispos(int field, int num)
{
	uint8_t* tmp = &pos_num[field * 10];
	while (*tmp)
	{
		if (num == *tmp)
			return 1;
		tmp++;
	}
	return 0;

}

//a valid sudoku field is needed, only removes pos
int precalc_linesrows3x3(int fix = 1)
{
	int rerun = 1;
	int directfound = 0;

	while (rerun)
	{
		rerun = 0;
		for (int i = 0; i < 81; ++i)
		{
			if (sudoku[i].fix || sudoku[i].num)
				continue;
			for (int j = 1; j < 10; ++j)
			{
				sudoku[i].num = j;
				if (checkfield(i))
				{
					unsetpos(i, j);
				}
			}
			sudoku[i].num = 0;
		}

		int singlenumber = 0;
		int num;

		for (int i = 0; i < 81; ++i)
		{
			if (sudoku[i].fix || sudoku[i].num)
				continue;
			uint8_t* tmp = &pos_num[i * 10];
			while (*tmp)
			{
				singlenumber++;
				num = *tmp;
				tmp++;
			}
			if (singlenumber == 1)
			{
				sudoku[i].num = num;
				if(fix)
					sudoku[i].fix = 1;
				pos_num[i * 10] = 0;
				rerun = 1;
				directfound++;
			}
			singlenumber = 0;
		}
	}

#ifdef visualize
	if(directfound)
	{
		printf("precalc_linesrows3x3: could find %zu numbers direct!\n", directfound);
		printfield();
	}
#endif

	return directfound;
}

int search_missing(int fix = 1)
{
	int count = 0;

	uint16_t found;
	//rows
	for (int i = 0; i < 9; ++i)
	{
		found = 0;
		//cols in row
		for (int j = 0; j < 9; ++j)
		{
			found |= (1 << sudoku[i * 9 + j].num);
		}
		//try every number, if it works more than one time it is not good
		for (int k = 1; k < 10; ++k)
		{
			//the number is missing in the row
			if (!(found & (1 << k)))
			{
				int pos_times = 0;
				//every col
				for (int j = 0; j < 9; ++j)
				{
					if (sudoku[i * 9 + j].num != 0)
						continue;
					sudoku[i * 9 + j].num = k;
					if (!checkfield(i * 9 + j))
						pos_times++;
					sudoku[i * 9 + j].num = 0;
				}
				//we found a valid solution
				if (pos_times == 1)
				{
					count++;
					for (int j = 0; j < 9; ++j)
					{
						if (sudoku[i * 9 + j].num != 0)
							continue;
						sudoku[i * 9 + j].num = k;
						if (!checkfield(i * 9 + j))
						{
							if(fix)
								sudoku[i * 9 + j].fix = 1;
							break;
						}
						sudoku[i * 9 + j].num = 0;
					}
				}
			}
		}
	}

	//cols
	for (int i = 0; i < 9; ++i)
	{
		found = 0;
		for (int j = 0; j < 9; ++j)
		{
			found |= (1 << sudoku[j * 9 + i].num);
		}
		for (int k = 1; k < 10; ++k)
		{
			if (!(found & (1 << k)))
			{
				int pos_times = 0;
				for (int j = 0; j < 9; ++j)
				{
					if (sudoku[j * 9 + i].num != 0)
						continue;
					sudoku[j * 9 + i].num = k;
					if (!checkfield(j * 9 + i))
						pos_times++;
					sudoku[j * 9 + i].num = 0;
				}
				if (pos_times == 1)
				{
					count++;
					for (int j = 0; j < 9; ++j)
					{
						if (sudoku[j * 9 + i].num != 0)
							continue;
						sudoku[j * 9 + i].num = k;
						if (!checkfield(j * 9 + i))
						{
							sudoku[j * 9 + i].fix = 1;
							break;
						}
						sudoku[j * 9 + i].num = 0;
					}
				}
			}
		}
	}
#ifdef visualize
	if(count)
	{
		printf("search_missing: could find %zu numbers direct!\n", count);
		printfield();
	}
#endif
	return count;
}

int main()
{
	/*int input[9][9] = {
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

	//hard to brute force
	/*int input[9][9] = {
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

	//hard to bruteforce
	/*int input[9][9] = {
			{0, 0, 0, 0, 0, 0, 0, 0, 1},
			{0, 0, 0, 0, 0, 0, 0, 2, 3},
			{0, 0, 4, 0, 0, 5, 0, 0, 0},

			{0, 0, 0, 1, 0, 0, 0, 0, 0},
			{0, 0, 0, 0, 3, 0, 6, 0, 0},
			{0, 0, 7, 0, 0, 0, 5, 8, 0},

			{0, 0, 0, 0, 6, 7, 0, 0, 0},
			{0, 1, 0, 0, 0, 4, 0, 0, 0},
			{5, 2, 0, 0, 0, 0, 0, 0, 0}
	};*/

	//hard for humans
	/*int input[9][9] = {
			{8, 5, 0, 0, 0, 2, 4, 0, 0},
			{7, 2, 0, 0, 0, 0, 0, 0, 9},
			{0, 0, 4, 0, 0, 0, 0, 0, 0},

			{0, 0, 0, 1, 0, 7, 0, 0, 2},
			{3, 0, 5, 0, 0, 0, 9, 0, 0},
			{0, 4, 0, 0, 0, 0, 0, 0, 0},

			{0, 0, 0, 0, 8, 0, 0, 7, 0},
			{0, 1, 7, 0, 0, 0, 0, 0, 0},
			{0, 0, 0, 0, 3, 6, 0, 4, 0}
	};*/

	//wiki example 24 solutions
/*	int input[9][9] = {
			{5, 3, 0, 0, 7, 0, 0, 0, 0},
			{6, 0, 0, 1, 9, 5, 0, 0, 0},
			{0, 0, 0, 0, 0, 0, 0, 6, 0},

			{8, 0, 0, 0, 6, 0, 0, 0, 3},
			{4, 0, 0, 8, 0, 3, 0, 0, 1},
			{7, 0, 0, 0, 2, 0, 0, 0, 6},

			{0, 6, 0, 0, 0, 0, 2, 8, 0},
			{0, 0, 0, 4, 1, 9, 0, 0, 5},
			{0, 0, 0, 0, 8, 0, 0, 7, 9}
	};*/

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

	//very simple (for testing precalc_linesrows3x3)
/*	int input[9][9] = {
		{5, 1, 7, 6, 0, 0, 0, 3, 4},
		{2, 8, 9, 0, 0, 4, 0, 0, 0},
		{3, 4, 6, 2, 0, 5, 0, 9, 0},

		{6, 0, 2, 0, 0, 0, 0, 1, 0},
		{0, 3, 8, 0, 0, 6, 0, 4, 7},
		{0, 0, 0, 0, 0, 0, 0, 0, 0},

		{0, 9, 0, 0, 0, 0, 0, 7, 8},
		{7, 0, 3, 4, 0, 0, 5, 6, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0}
	};*/

	//simple (for testing precalc_linesrows3x3)
/*	int input[9][9] = {
			{9, 2, 0, 6, 1, 0, 0, 0, 0},
			{4, 6, 0, 0, 0, 3, 0, 0, 0},
			{0, 1, 0, 7, 0, 0, 9, 0, 6},

			{0, 5, 0, 0, 7, 8, 2, 0, 0},
			{6, 7, 2, 0, 0, 0, 5, 8, 0},
			{0, 0, 0, 2, 0, 6, 0, 0, 3},

			{0, 9, 6, 3, 2, 0, 0, 1, 4},
			{7, 0, 0, 0, 0, 0, 6, 0, 0},
			{0, 0, 1, 0, 6, 4, 0, 9, 0}
	};*/

	//find missing test (should find two 8)
/*	int input[9][9] = {
			{1, 2, 3, 4, 5, 6, 0, 0, 0},
			{4, 0, 0, 0, 0, 0, 0, 0, 0},
			{5, 0, 0, 0, 0, 0, 0, 0, 0},

			{2, 0, 0, 0, 0, 0, 0, 0, 0},
			{7, 0, 0, 0, 0, 0, 0, 0, 0},
			{6, 0, 0, 0, 0, 0, 0, 8, 0},

			{0, 0, 0, 8, 0, 0, 0, 0, 0},
			{0, 0, 0, 0, 0, 0, 0, 0, 0},
			{0, 0, 0, 0, 0, 0, 8, 0, 0}
	};*/

	//copy input
	for (int x = 0; x < 9; ++x)
	{
		for (int y = 0; y < 9; ++y)
		{
			sudoku[y * 9 + x].num = input[y][x];
			if (sudoku[y * 9 + x].num)
				sudoku[y * 9 + x].fix = 1;
		}
	}


	if (checkfield())
	{
		printf("error, input not valid\n");
		exit(1);
	}

	int field = 0;
	int reverting = 0;
	size_t solution = 0;
	size_t iterration = 0;
	size_t backtracks = 0;
	struct timespec totalstart={0,0}, tstart={0,0}, tend={0,0};

	//every number everywhere possible
	for (uint8_t i = 0; i < 81; ++i)
	{
		sudoku[i].testing_num = &pos_num[i * 10];
		for (uint8_t j = 1; j < 11; ++j)
		{
			setpos(i, j);
		}
	}

	clock_gettime(CLOCK_MONOTONIC, &totalstart);
	clock_gettime(CLOCK_MONOTONIC, &tstart);

	//start to work
	while (precalc_linesrows3x3() ||
			search_missing());

	clock_gettime(CLOCK_MONOTONIC, &tend);
	printf("precalc took about %.9f seconds\n",
		   ((double)tend.tv_sec + 1.0e-9*tend.tv_nsec) -
				   ((double)tstart.tv_sec + 1.0e-9*tstart.tv_nsec));
	clock_gettime(CLOCK_MONOTONIC, &tstart);

	while (1)
	{
		iterration++;

#ifdef visualize
		printfield();
		printf("\033[13;A");
		//nanosleep((const struct timespec[]) {{1L, 0}}, NULL);
#endif

		//finished with a solution
		if (field >= 81)
		{
			solution++;
			clock_gettime(CLOCK_MONOTONIC, &tend);
			printf("Solution: %zu  -  %.9f seconds \n", solution,((double)tend.tv_sec + 1.0e-9*tend.tv_nsec) -
					((double)tstart.tv_sec + 1.0e-9*tstart.tv_nsec));
			clock_gettime(CLOCK_MONOTONIC, &tstart);
			printfield();
			printf("Iterarions: %zu Backtracks: %zu\n", iterration, backtracks);
			iterration = 0;
			backtracks = 0;

#ifdef visualize
			getchar();
			printf("\033[14;B");
#endif

			//start working on the last field again
			while (1)
			{
				field--;
				if (field < 0)
					break;
				if (sudoku[field].fix)
					continue;
				if (!*sudoku[field].testing_num)
				{
					sudoku[field].num = 0;
					sudoku[field].testing_num = &pos_num[field * 10];
					continue;
				}
				sudoku[field].num = *sudoku[field].testing_num;
				sudoku[field].testing_num++;
				break;
			}
		}

		//no more solutions
		if (field < 0)
			break;

		//jump over fix fields
		if (!reverting)
		{
			if (sudoku[field].fix)
			{
				field++;
				continue;
			}
		}
		else
		{
			if (sudoku[field].fix)
			{
				field--;
				continue;
			}
		}

		//We increase the current field
		if (*(sudoku[field].testing_num))
		{
			reverting = 0;
			sudoku[field].num = *sudoku[field].testing_num;
			sudoku[field].testing_num++;
		}
		else
		{
			//somethings fishy
			reverting = 1;
			sudoku[field].num = 0;
			sudoku[field].testing_num = &pos_num[field * 10];
			field--;
			backtracks++;
			continue;
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

	clock_gettime(CLOCK_MONOTONIC, &tend);
	printf("Finished, Solutions found: %zu  -  %.9f seconds\n", solution, ((double)tend.tv_sec + 1.0e-9*tend.tv_nsec) -
			((double)totalstart.tv_sec + 1.0e-9*totalstart.tv_nsec));
	return 0;
}
