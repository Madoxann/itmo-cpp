#include "phonebook.h"
#include "quicksort.h"
#include "return_codes.h"
#include <fstream>
#include <iostream>
#include <vector>

using namespace std;
typedef long long ll;

ll n;

template< typename T >
int IOsort(ifstream &in, bool isDesc, char *streamName)
{
	in >> n;

	vector< T > inputData;
	try
	{
		inputData = vector< T >(n);
	} catch (const std::bad_alloc &e)
	{
		fprintf(stderr, "Couldn't allocate memory of the size mentioned, here's what cpp has to say\n%s", e.what());
		in.close();
		return ERROR_MEMORY;
	}
	for (ll i = 0; i < n; i++)
	{
		in >> inputData[i];
		if (in.eof() && i != n - 1)
		{
			in.close();
			return ERROR_INVALID_DATA;
		}
	}
	in.close();

	isDesc ? quicksort< T, 1 >(inputData) : quicksort< T, 0 >(inputData);

	ofstream out;
	out.open(streamName);
	if (!out)
		return ERROR_NOT_FOUND;

	for (T item : inputData)
		out << item << endl;
	out.close();

	return ERROR_SUCCESS;
}

int main(int argc, char **argv)
{
	if (argc != 3)
	{
		fprintf(stderr, "Wrong argument number!\n");
		return ERROR_INVALID_PARAMETER;
	}

	ifstream in;
	in.open(argv[1]);
	if (!in)
	{
		fprintf(stderr, "Failed to open file\n");
		return ERROR_NOT_FOUND;
	}

	string type, mode;
	in >> type >> mode;
	bool isDesc;
	if (mode == "descending")
		isDesc = true;
	else if (mode == "ascending")
		isDesc = false;
	else
	{
		fprintf(stderr, "Unavailable sorting option\n");
		in.close();
		return ERROR_INVALID_DATA;
	}

	int result;
	if (type == "int")
		result = IOsort< int >(in, isDesc, argv[2]);
	else if (type == "float")
		result = IOsort< float >(in, isDesc, argv[2]);
	else if (type == "phonebook")
		result = IOsort< Phonebook >(in, isDesc, argv[2]);
	else
	{
		fprintf(stderr, "Invalid parameter type\n");
		in.close();
		return ERROR_INVALID_DATA;
	}
	if (result)
	{
		fprintf(stderr, result == ERROR_INVALID_DATA ? "Received less values then expected\n" : "Failed to open file\n");
		return result;
	}

	return ERROR_SUCCESS;
}