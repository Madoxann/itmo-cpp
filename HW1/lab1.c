#include "return_codes.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

float precision = 0.00001f;

void getLinear(int n, float** matrix)
{
	for (int i = 0; i < n; i++)
	{
		int advance = 0;
		float max = 0;
		int ind = i;
		while (fabsf(max) < precision && i + advance < n)
		{
			for (int j = i + advance; j < n; j++)
			{
				if (fabsf(matrix[j][i + advance]) > max)
				{
					max = matrix[j][i + advance];
					ind = j;
				}
			}
			if (fabsf(max) < precision)
				advance++;
		}
		if (i + advance == n)
			return;

		float* temp = matrix[ind];
		matrix[ind] = matrix[i + advance];
		matrix[i + advance] = temp;

		for (int j = i + 1; j < n; j++)
		{
			if (fabsf(matrix[j][i + advance]) >= precision)
			{
				float multiplier = matrix[j][i + advance] / matrix[i][i + advance];
				for (int k = 0; k < n + 1; k++)
					matrix[j][k] -= matrix[i][k] * multiplier;
			}
		}
	}
}

void freeResources(FILE* file, float** matrix, int lastInd)
{
	if (file != NULL)
		fclose(file);
	for (int i = 0; i < lastInd; i++)
		free(matrix[i]);
	free(matrix);
}

int main(int argc, char** argv)
{
	if (argc != 3)
	{
		printf("Invalid argument amount");
		return ERROR_INVALID_PARAMETER;
	}

	FILE* in = fopen(argv[1], "r");
	if (in == NULL)
	{
		printf("Couldn't open the file");
		return ERROR_NOT_FOUND;
	}

	int n;
	fscanf(in, "%d", &n);

	float** matrix = malloc(sizeof(float*) * n);
	if (matrix == NULL)
	{
		printf("Couldn't allocate the memory");
		fclose(in);
		return ERROR_MEMORY;
	}

	for (int i = 0; i < n; i++)
	{
		matrix[i] = malloc(sizeof(float) * (n + 1));
		if (matrix[i] == NULL)
		{
			printf("Couldn't allocate the memory");
			freeResources(in, matrix, i);
			return ERROR_MEMORY;
		}
		for (int j = 0; j < n + 1; j++)
			fscanf(in, "%f", &matrix[i][j]);
	}
	fclose(in);

	getLinear(n, matrix);
	FILE* out = fopen(argv[2], "w");
	if (out == NULL)
	{
		printf("Couldn't allocate the memory");
		freeResources(out, matrix, n);
		return ERROR_MEMORY;
	}

	int nullStr = 0;
	for (int i = 0; i < n; i++)
	{
		int cnt = 0;
		for (int j = 0; j < n + 1; j++)
			if (fabsf(matrix[i][j]) < precision)
				cnt++;
		if (cnt == n + 1)
			nullStr++;
		if (cnt == n)
		{
			fprintf(out, "no solution");
			freeResources(out, matrix, n);
			return ERROR_SUCCESS;
		}
	}
	if (nullStr > 0)
	{
		fprintf(out, "many solutions");
		freeResources(out, matrix, n);
		return ERROR_SUCCESS;
	}

	float* solution = malloc(n * sizeof(float));
	if (solution == NULL)
	{
		printf("Couldn't allocate the memory");
		freeResources(out, matrix, n);
		return ERROR_MEMORY;
	}

	solution[n - 1] = matrix[n - 1][n] / matrix[n - 1][n - 1];

	for (int i = n - 2; i > -1; i--)
	{
		for (int j = i + 1; j < n; j++)
		{
			float subtract = solution[j] * matrix[i][j];
			matrix[i][n] -= subtract;
		}
		solution[i] = matrix[i][n] / matrix[i][i];
	}

	for (int i = 0; i < n; i++)
		fprintf(out, "%g\n", solution[i]);
	freeResources(out, matrix, n);
	free(solution);

	return ERROR_SUCCESS;
}