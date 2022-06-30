#include "LN.h"
#include "return_codes.h"

#include <fstream>
#include <iostream>
#include <map>
#include <stack>
#include <string>
using namespace std;

LN add(const LN& l, const LN& r) { return l + r; }
LN subtract(const LN& l, const LN& r) { return l - r; }
LN multiply(const LN& l, const LN& r) { return l * r; }
LN divide(const LN& l, const LN& r) { return l / r; }
LN remainder(const LN& l, const LN& r) { return l % r; }
LN sqrt(const LN& ln) { return ~ln; }
LN negateLN(const LN& ln) { return -ln; }

LN lesser(const LN& l, const LN& r) { return LN(l < r); }
LN lesserOrEq(const LN& l, const LN& r) { return LN(l <= r); }
LN great(const LN& l, const LN& r) { return LN(l > r); }
LN greatOrEq(const LN& l, const LN& r) { return LN(l >= r); }
LN equals(const LN& l, const LN& r){ return LN(l == r); }
LN notEquals(const LN& l, const LN& r) { return LN(l != r); }

int main(int argc, char** argv)
{
	if (argc != 3)
	{
		cerr << "Incorrect argument amount" << endl;
		return ERROR_INVALID_PARAMETER;
	}

	ifstream in(argv[1]);
	if (!in)
	{
		cerr << "Failed to open file" << endl;
		return ERROR_FILE_NOT_FOUND;
	}

	map< string, LN (*)(const LN&, const LN&) > mapArithmetics = {
		{ "+", add },		 { "-", subtract }, { "*", multiply },	  { "/", divide },
		{ "%", remainder },	 { "<", lesser },	{ "<=", lesserOrEq }, { ">", great },
		{ ">=", greatOrEq }, { "==", equals },	{ "!=", notEquals }
	};
	map< string, LN (*)(const LN&) > mapUnary = { { "_", negateLN }, { "~", sqrt } };

	stack< LN > st;
	string buff;
	while (getline(in, buff))
	{
		LN cpVal;
		LN* tmp = nullptr;
		bool possibleBinary = st.size() >= 2;
		if (!st.empty())
		{
			cpVal = st.top();
			tmp = &cpVal;
			st.pop();
		}
		if (auto hasBinFn = mapArithmetics.find(buff); hasBinFn != mapArithmetics.end() && possibleBinary)
		{
			try
			{
				*tmp = hasBinFn->second(st.top(), *tmp);
			} catch (bad_alloc& e)
			{
				cerr << "Insufficient memory" << endl;
				return ERROR_MEMORY;
			}
			st.pop();
		}
		else if (auto hasUnFn = mapUnary.find(buff); hasUnFn != mapUnary.end() && (!st.empty() || tmp))
		{
			try
			{
				*tmp = hasUnFn->second(*tmp);
			} catch (bad_alloc& e)
			{
				cerr << "Insufficient memory" << endl;
				return ERROR_MEMORY;
			}
		}
		else
		{
			bool negate = false;
			if (buff[0] == '-')
			{
				buff = buff.substr(1, buff.size() - 1);
				negate = true;
			}
			if (tmp) st.push(*tmp);
			try
			{
				st.push(negate ? LN("-" + buff) : LN(buff));
			} catch (bad_alloc& e)
			{
				cerr << "Insufficient memory" << endl;
				return ERROR_MEMORY;
			}
			continue;
		}
		st.push(*tmp);
	}
	in.close();

	ofstream out(argv[2]);
	if (!out)
	{
		cerr << "Failed to open file" << endl;
		return ERROR_FILE_NOT_FOUND;
	}

	while (!st.empty())
	{
		out << st.top() << endl;
		st.pop();
	}
	out.close();

	return 0;
}
