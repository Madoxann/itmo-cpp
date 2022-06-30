#include "LN.h"

#include <climits>
#include <cmath>
#include <sstream>
#include <string>

LN::LN(const std::string_view str)
{
	if (str == "NaN")
	{
		isNaN = true;
		return;
	}
	std::string_view newStr = str;
	negate = newStr[0] == '-';
	if (negate)
		newStr = newStr.substr(1, str.size() - 1);
	size_t len = 0;
	num.resize(newStr.size() / 9 - ((newStr.size() / 9) * 9 == newStr.size()) + 1);
	uint64_t prev = newStr.size();
	for (uint64_t i = prev; prev >= i && i != 0; i -= 9)
	{
		char* tmp;
		if (i < 9)
		{
			char* tmp1 = const_cast< char* >(newStr.substr(0, i).data());
			tmp = new char[i + 1];
			std::copy(tmp1, tmp1 + i, tmp);
			tmp[i] = '\0';
		}
		else
		{
			char* tmp1 = const_cast< char* >(newStr.substr(i - 9, 9).data());
			tmp = new char[10];
			std::copy(tmp1, tmp1 + 9, tmp);
			tmp[9] = '\0';
		}
		num[len++] = strtol(tmp, nullptr, 10);
		delete[] tmp;
		prev = i;
	}
}

LN::LN(long long int num)
{
	std::ostringstream fix;
	fix << num;
	*this = LN(fix.str());
}

LN::LN(LN&& ln) noexcept
{
	num = ln.num;
	negate = ln.negate;
	isNaN = ln.isNaN;

	ln.num = nullptr;
	ln.negate = false;
	ln.isNaN = false;
}

LN::LN(const LN& ln)
{
	negate = ln.negate;
	num = ln.num;
	isNaN = ln.isNaN;
}

void LN::removeLZ()
{
	size_t finalSz = num.getSize(), prev = num.getSize() - 1;
	for (size_t i = num.getSize() - 1; i <= prev; i--)
	{
		if (!num[i])
			finalSz--;
		else
			break;
		prev = i;
	}
	num.resize(finalSz);
	if (!finalSz)
		num[0] = 0;
}

LN operator+(const LN& l, const LN& r)
{
	if (l.isNaN || r.isNaN)
		return LN("NaN");
	if (l.negate != r.negate)
		return (l.negate) ? (r - (-l)) : (l - (-r));
	if (l.negate)
		return -(-l + (-r));
	LN ret = l;
	if (l.isNaN || r.isNaN)
	{
		ret.isNaN = true;
		return ret;
	}
	int carry = 0;
	for (size_t i = 0; i < std::max(l.num.getSize(), r.num.getSize()) || carry; i++)
	{
		if (i == ret.num.getSize())
			ret.num.push_back(0);
		ret.num[i] += carry + (i >= r.num.getSize() ? 0 : r.num[i]);
		if (ret.num[i] >= LN::BASE)
		{
			carry = 1;
			ret.num[i] -= LN::BASE;
		}
		else
			carry = 0;
	}
	ret.removeLZ();
	return ret;
}

LN operator-(const LN& l, const LN& r)
{
	if (l.isNaN || r.isNaN)
		return LN("NaN");
	if (l.negate || r.negate)
		return (l.negate) ? -(-l + r) : l + (-r);
	if (l < r)
		return -(r - l);
	LN ret = l;
	if (l.isNaN || r.isNaN)
	{
		ret.isNaN = true;
		return ret;
	}
	int carry = 0;
	for (size_t i = 0; i < std::max(l.num.getSize(), r.num.getSize()) || carry; i++)
	{
		uint64_t prev = ret.num[i];
		ret.num[i] -= carry + (i >= r.num.getSize() ? 0 : r.num[i]);
		if (prev < ret.num[i])
		{
			carry = 1;
			ret.num[i] += LN::BASE;
		}
		else
			carry = 0;
	}
	ret.removeLZ();
	return ret;
}

LN LN::operator-() const
{
	LN ret = *this;
	ret.negate = !this->negate;
	return ret;
}

LN operator*(const LN& l, const LN& r)
{
	if (l.isNaN || r.isNaN)
		return LN("NaN");
	LN ret;
	ret.num = ReversedVector< uint64_t >(l.num.getSize() + r.num.getSize(), 0);
	ret.negate = l.negate ^ r.negate;
	for (size_t i = 0; i < l.num.getSize(); i++)
	{
		uint64_t carry = 0;
		for (size_t j = 0; j < r.num.getSize() || carry; j++)
		{
			uint64_t curr = carry + ret.num[i + j] + l.num[i] * (j >= r.num.getSize() ? 0 : r.num[j]);
			ret.num[i + j] = curr % LN::BASE;
			// int may be enough
			carry = curr / LN::BASE;
		}
	}
	ret.removeLZ();
	return ret;
}

LN operator/(const LN& l, const LN& r)
{
	if (l.isNaN || r.isNaN || r == LN("0"))
		return LN("NaN");
	LN ret, curr;
	ret.num.resize(l.num.getSize());
	size_t prev = l.num.getSize() - 1;
	for (size_t i = prev; i <= prev; i--)
	{
		curr.num.push_front(l.num[i]);
		curr.removeLZ();
		long long left = 0, right = LN::BASE;
		while (left <= right)
		{
			long long mid = (left + right) / 2;
			if ((r.negate ? -r : r) * LN(mid) > curr)
				right = mid - 1;
			else
			{
				left = mid + 1;
				ret.num[i] = mid;
			}
		}
		curr -= r * LN(ret.num[i]);
		prev = i;
	}
	ret.negate = l.negate ^ r.negate;
	ret.removeLZ();
	return ret;
}

LN operator~(const LN& ln)
{
	if (ln < LN("0") || ln.isNaN)
		return LN("NaN");
	LN l, r = ln, ret;
	while (l <= r)
	{
		LN mid = (l + r) / LN(2);
		if (mid * mid <= ln)
		{
			ret = mid;
			l = mid + LN(1);
		}
		else
			r = mid - LN(1);
	}
	return ret;
}

bool operator==(const LN& l, const LN& r)
{
	if (l.isNaN || r.isNaN)
		return false;
	if (l.num.getSize() != r.num.getSize())
		return false;
	if (l.num.getSize() == 1 && l.num[0] == r.num[0] && l.num[0] == 0)
		return true;
	if (l.negate != r.negate)
		return false;
	for (size_t i = 0; i < l.num.getSize(); i++)
		if (l.num[i] != r.num[i])
			return false;
	return true;
}

bool operator<(const LN& l, const LN& r)
{
	if (l.isNaN || r.isNaN)
		return false;
	if (l == r)
		return false;
	if (l.negate)
		return !r.negate || (-r) < (-l);
	if (r.negate)
		return false;
	if (l.num.getSize() != r.num.getSize())
		return l.num.getSize() < r.num.getSize();
	size_t prev = l.num.getSize() - 1;
	for (size_t i = prev; i <= prev; i--)
	{
		if (l.num[i] != r.num[i])
			return l.num[i] < r.num[i];
		prev = i;
	}
	return false;
}

LN& LN::operator=(const LN& other)
{
	num = other.num;
	negate = other.negate;
	isNaN = other.isNaN;
	return *this;
}

LN& LN::operator=(LN&& other) noexcept
{
	this->~LN();
	return *(new (this) LN(std::move(other)));
}

LN::operator long long() const
{
	if (isNaN)
		throw std::bad_cast();
	if (*this > LN(LLONG_MAX) || *this < LN(LLONG_MIN))
		throw std::bad_alloc();
	long long ret = 0, prev = 1;
	for (size_t i = 0; i < num.getSize(); i++)
	{
		ret += num[i] * prev * (negate ? -1 : 1);
		prev *= LN::BASE;
	}
	return ret;
}

std::ostream& operator<<(std::ostream& out, const LN& ln)
{
	if (ln == LN("0"))
		return out << "0";
	return (!ln.isNaN) ? (out << (ln.negate ? "-" : "") << ln.num) : (out << "NaN");
}

LN operator"" _ln(unsigned long long ln)
{
	return LN{ (long long)ln };
}
