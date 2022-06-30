#pragma once

#include <vector>

typedef long long ll;
extern ll n;

template< typename T >
void swap(std::vector< T > &arr, ll l, ll r)
{
	T temp = arr[l];
	arr[l] = arr[r];
	arr[r] = temp;
}

template< typename T, bool descending >
ll partition(std::vector< T > &arr, ll l, ll r)
{
	T base = arr[(l + r) / 2];
	ll i = l;
	ll j = r;
	while (i <= j)
	{
		while (descending ? arr[i] > base : arr[i] < base)
			i++;
		while (descending ? arr[j] < base : arr[j] > base)
			j--;
		if (i >= j)
			break;
		swap(arr, i++, j--);
	}
	return j;
}

template< typename T, bool descending >
void quicksortRange(std::vector< T > &arr, ll l, ll r)
{
	while (l < r)
	{
		ll parted = partition< T, descending >(arr, l, r);
		if (parted - l < r - parted)
		{
			quicksortRange< T, descending >(arr, l, parted);
			l = parted + 1;
		}
		else
		{
			quicksortRange< T, descending >(arr, parted + 1, r);
			r = parted;
		}
	}
}

template< typename T, bool descending >
void quicksort(std::vector< T > &arr)
{
	return quicksortRange< T, descending >(arr, 0, n - 1);
}
