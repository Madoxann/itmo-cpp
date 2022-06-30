#pragma once
#include "ReversedVector.h"
#include <string_view>

#include <iostream>
#include <string>

class LN
{
	ReversedVector< uint64_t > num;
	bool negate, isNaN = false;

	void removeLZ();

  public:
	static const uint64_t BASE = 1e9;

	explicit LN(long long num = 0);
	explicit LN(std::string_view str);
	explicit LN(const char* str) : LN(std::string_view(str)){};
	LN(const LN& ln);
	LN(LN&& ln) noexcept;

	friend LN operator+(const LN& l, const LN& r);
	LN& operator+=(const LN& ln) { return *this = *this + ln; };
	friend LN operator-(const LN& l, const LN& r);
	LN& operator-=(const LN& ln) { return *this = *this - ln; };
	LN operator-() const;
	friend LN operator*(const LN& l, const LN& r);
	LN& operator*=(const LN& ln) { return *this = *this * ln; };
	friend LN operator/(const LN& l, const LN& r);
	LN& operator/=(const LN& ln) { return *this = *this / ln; };
	friend LN operator%(const LN& l, const LN& r) { return l - r * (l / r); };
	LN& operator%=(const LN& ln) { return *this = *this % ln; };
	friend LN operator~(const LN& ln);

	friend bool operator==(const LN& l, const LN& r);
	friend bool operator!=(const LN& l, const LN& r) { return !(l == r); };
	friend bool operator<(const LN& l, const LN& r);
	friend bool operator<=(const LN& l, const LN& r) { return l == r || l < r; };
	friend bool operator>(const LN& l, const LN& r) { return !l.isNaN && !r.isNaN && !(l <= r); };
	friend bool operator>=(const LN& l, const LN& r) { return !l.isNaN && !r.isNaN && !(l < r); };

	LN& operator=(const LN& other);
	LN& operator=(LN&& other) noexcept;

	explicit operator bool() const { return *this == LN("0"); };
	explicit operator long long() const;

	friend std::ostream& operator<<(std::ostream& out, const LN& ln);

	~LN(){};
};

LN operator"" _ln(unsigned long long ln);
