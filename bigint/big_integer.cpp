
#include <iostream>
#include <vector>
#include <stdexcept>
#include <algorithm>
#include <memory.h>
#include <cstring>
#include <stdexcept>
#include <stdint.h>
#include "big_integer.h"

int big_integer::sz() const {
	return bits;
}

void big_integer::check_zero() {
	int z = true;
	for (int i = 0; i < sz(); ++i) {
		if (num[i]) {
			z = false; break;
		}
	}
	if (z) sgn = true;
}

int big_integer::n_bits() const {
	int k = bits - 1;
	while (k > 0 && !num[k]) --k;
	return ++k;
}

void big_integer::reserve(int k) {
	if (k > (int) bits) {
		uint64_t* buffer = new uint64_t[bits];
		memcpy(buffer, num, bits * sizeof(uint64_t));
		num = new uint64_t[k * 2];
		memset(num, 0, sizeof(uint64_t) * k * 2);
		memcpy(num, buffer, bits * sizeof(uint64_t));
		bits = k * 2;
		delete buffer;
	}
}

void big_integer::clean() {
	int non_empty = n_bits();
	non_empty = std::max(non_empty + 1, 2);
	if (non_empty * 4 < (int) bits) {
		uint64_t *buffer = new uint64_t[non_empty * 2];
		memcpy(buffer, num, non_empty * 2 * sizeof(uint64_t));
		num = new uint64_t[non_empty * 2];
		memcpy(num, buffer, non_empty * 2 * sizeof(uint64_t));
		bits = non_empty * 2;
		delete buffer;
	}
}

big_integer::big_integer() {
	*this = 0;
}

big_integer::big_integer(int value) {
	sgn = (value >= 0);
	num = new uint64_t[10];
	memset(num, 0, 10 * sizeof(uint64_t));
	num[0] = abs(value);
	bits = 10;
}

big_integer::big_integer(big_integer const& other) {
	*this = other;
}

big_integer::~big_integer() {
	delete num;
}

big_integer::big_integer(std::string const& str) {
	int local_base = 1000;
	int c = 0;
	uint64_t *dem = new uint64_t[str.size()];
	if (str[c] == '-') {
		sgn = false;
		++c;
	} else {
		sgn = true;
	}
	int iter = 0;
	for (int i = str.size() - 1; i >= c; i -= 3) {
		uint32_t dig = str[i] - '0';
		if (i - 1 >= c) dig += (str[i - 1] - '0') * 10;
		if (i - 2 >= c) dig += (str[i - 2] - '0') * 100;
		dem[iter++] = dig;
	}
	int mx = str.size() / 7 + 1;
	num = new uint64_t[mx];
	bits = mx;
	memset(num, 0, mx * sizeof(uint64_t));
	int ptr = 0;
	int ind = 1;
	while (iter > 0 && !dem[iter - 1]) --iter;
	while (iter > 0) {
		if (ptr == 32) {
			++ind;
			ptr = 0;
		}
		if (dem[0] & 1) {
			num[ind - 1] |= (1ll << ptr);
		}
		for (int i = iter - 1; i >= 0; --i) {
			if (i) dem[i - 1] += (dem[i] & 1) * local_base;
			dem[i] >>= 1;
		}
		while (iter > 0 && !dem[iter - 1]) --iter;
		++ptr;
	}
	delete dem;
	clean();
	check_zero();
}

big_integer& big_integer::operator=(big_integer const& other) {
	uint64_t *buffer = new uint64_t[other.sz()];
	memcpy(buffer, other.num, other.sz() * sizeof(uint64_t));
	num = new uint64_t[other.sz()];
	memcpy(num, buffer, sizeof(uint64_t) * other.sz());
	sgn = other.sgn;
	bits = other.bits;
	delete buffer;
	return *this;
}

big_integer& big_integer::operator+=(big_integer const& other) {
	if (other == 0) {
		return *this;
	}
	if (*this == 0) {
		return *this = other;
	}
	if (sgn == other.sgn) {
		int n = std::max(n_bits(), other.n_bits()) + 1;
		reserve(n);
		for (int i = 0; i < n; ++i) {
			uint64_t val = (i < other.sz() ? other.num[i] : 0);
			num[i] += val;
			num[i + 1] += num[i] / BASE;
			num[i] %= BASE;
		}
	} else {
		if (sgn) {
			*this -= (-other);
		} else {
			*this = -*this;
			*this -= other;
			*this = -*this;
		}
	}
	clean();
	check_zero();
	return *this;
}

big_integer& big_integer::operator-=(big_integer const& other) {
	if (other == 0) {
		return *this;
	}
	if (*this == 0) {
		return *this = -other;
	}
	if (sgn == other.sgn) {
		int n = std::max(n_bits(), other.n_bits()) + 1;
		int64_t *buffer = new int64_t[n];
		memset(buffer, 0, sizeof(int64_t) * n);
		reserve(n);
		for (int i = 0; i < n - 1; ++i) {
			buffer[i] += num[i] - (i < other.sz() ? other.num[i] : 0);
			if (buffer[i] < 0) {
				buffer[i + 1]--;
				buffer[i] += BASE;
			}
		}
		if (buffer[n - 1] < 0) {
			sgn ^= 1;
			for (int i = 0; i < n; ++i) {
				buffer[i] = -buffer[i];
			}
			for (int i = n-2; i >= 0; --i) {
				if (buffer[i] < 0) {
					buffer[i] += BASE;
					buffer[i + 1]--;
				}
			}
		}
		memcpy(num, buffer, sizeof(uint64_t) * n);
		delete buffer;
	} else {
		if (sgn) {
			*this += -other;
		} else {
			sgn ^= 1;
			*this += other;
			sgn ^= 1;
		}
	}
	clean();
	check_zero();
	return *this;
}

big_integer& big_integer::operator*=(big_integer const& other) {
	int x = n_bits();
	int y = other.n_bits();
	int n = x + y;
	uint64_t *buffer = new uint64_t[n];
	memset(buffer, 0, sizeof(uint64_t) * n);
	for (int i = 0; i < x; ++i) {
		for (int j = 0; j < y; ++j) {
			buffer[i + j] += num[i] * other.num[j];
			buffer[i + j + 1] += (buffer[i + j] >> 32);
			buffer[i + j] %= BASE;
		}
	}
	for (int i = 0; i < n - 1; ++i) {
		buffer[i + 1] += buffer[i] / BASE;
		buffer[i] %= BASE;
	}
	reserve(n);
	memcpy(num, buffer, sizeof(uint64_t) * n);
	delete buffer;
	if (sgn == other.sgn) {
		sgn = true;
	} else {
		sgn = false;
	}
	check_zero();
	return *this;
}

big_integer& big_integer::operator/=(big_integer const& other) {
	if (*this == 0) return *this;
	int x = n_bits();
	int y = other.n_bits();
	int n = x + y;
	uint64_t *cur_value = new uint64_t[n];
	uint64_t *buffer = new uint64_t[n];
	uint64_t *ans = new uint64_t[x];
	memset(cur_value, 0, sizeof(uint64_t) * n);
	memset(ans, 0, sizeof(uint64_t) * x);
	int c;
	for (int i = x - y; i >= 0; --i) {
		uint64_t l = 0, r = BASE;
		while (l + 1 < r) {
			uint64_t mid = (l + r) / 2;
			memcpy(buffer, cur_value, sizeof(uint64_t) * n);
			for (c = i; c - i < y; ++c) {
				buffer[c] += other.num[c - i] * mid;
				buffer[c + 1] += (buffer[c] >> 32);
				buffer[c] = (uint32_t) buffer[c];
			}
			if ((buffer[c] >> 32) != 0) {
				buffer[c + 1] += (buffer[c] >> 32);
				buffer[c] = (uint32_t) buffer[c];
				++c;
			}
			for (int j = n - 1; j >= 0; --j) {
				uint64_t number = (j < sz() ? num[j] : 0);
				if (number < buffer[j]) {
					r = mid;
					break;
				}
				if (number > buffer[j]) {
					l = mid;
					break;
				}
			}
			if (l != mid && r != mid) l = mid;
		}
		ans[i] = l;
		for (c = i; c < i + y; ++c) {
			cur_value[c] += other.num[c - i] * l;
			cur_value[c + 1] += (cur_value[c] >> 32);
			cur_value[c] = (uint32_t) cur_value[c];
		}
		if ((cur_value[c] >> 32) != 0) {
			cur_value[c + 1] += (cur_value[c] >> 32);
			cur_value[c] = (uint32_t) cur_value[c];
			++c;
		}
	}
	sgn = (sgn == other.sgn);
	memset(num, 0, sizeof(uint64_t) * sz());
	memcpy(num, ans, x * sizeof(uint64_t));
	check_zero();
	delete buffer;
	delete ans;
	delete cur_value;
	return *this;
}

big_integer& big_integer::operator%=(big_integer const& other) {
	if (other == 0) {
		throw std::runtime_error("taking modulo by zero!!!");
	}
	big_integer a = *this;
	a /= other;
	a *= other;
	*this -= a;
	clean();
	check_zero();
	return *this;
}

uint64_t binary_and(uint64_t a, uint64_t b) {
	return a & b;
}

uint64_t binary_or(uint64_t a, uint64_t b) {
	return a | b;
}

uint64_t binary_xor(uint64_t a, uint64_t b) {
	return a ^ b;
}

void big_integer::binary_oper(big_integer const& other, uint64_t (*func)(uint64_t, uint64_t)) {
	int n = std::max(n_bits(), other.n_bits()) + 1;
	big_integer b = other;
	b.reserve(n);
	reserve(n);
	for (int i = 0; i < n; ++i) {
		if (!sgn) num[i] = uint32_t(~num[i]);
		if (!b.sgn) b.num[i] = uint32_t(~b.num[i]);
	}
	if (!sgn) {
		sgn = true;
		++*this;
	}
	if (!b.sgn) {
		b.sgn = true;
		++b;
	}
	for (int i = 0; i < n; ++i) {
		num[i] = (*func)(num[i], b.num[i]);
	}
	if ((num[n - 1] & ((uint32_t)1 << 31)) != 0) {
		--*this;
		for (int i = 0; i < n; ++i) {
			num[i] = uint32_t(~num[i]);
		}
		sgn = false;
	}
	~b;
	clean();
	check_zero();
}

big_integer& big_integer::operator&=(big_integer const& other) {
	(*this).binary_oper(other, binary_and);
	return *this;
}

big_integer& big_integer::operator|=(big_integer const& other) {
	(*this).binary_oper(other, binary_or);
	return *this;
}

big_integer& big_integer::operator^=(big_integer const& other) {
	(*this).binary_oper(other, binary_xor);
	return *this;
}

big_integer big_integer::operator+() const {
	return *this;
}

big_integer big_integer::operator~() const {
	big_integer ret = *this;
	ret = -ret - 1;
	ret.clean();
	ret.check_zero();
	return ret;
}

big_integer big_integer::operator-() const {
	big_integer ret = *this;
	if (ret != 0) {
		ret.sgn ^= 1;
	}
	return ret;
}

big_integer& big_integer::operator++() {
	(*this) += 1;
	return *this;
}

big_integer big_integer::operator++(int) {
	big_integer ret = *this;
	++*this;
	return ret;
}

big_integer& big_integer::operator--() {
	(*this) -= 1;
	return *this;
}

big_integer big_integer::operator--(int) {
	big_integer ret = *this;
	--*this;
	return ret;
}

big_integer& big_integer::operator>>=(int shift) {
	int big_shift = shift / 32;
	int small_shift = shift % 32;
	int n = n_bits() + 1;
	reserve(n);
	bool c = sgn;
	sgn = true;
	if (!c) {
		for (int i = 0; i < n; ++i) {
			num[i] = uint32_t(~num[i]);
		}
		++*this;
	}
	for (int i = 0; i + big_shift < n; ++i) {
		num[i] = num[i + big_shift];
		uint64_t remain = num[i] % ((uint32_t) 1 << small_shift);
		if (i) {
			num[i - 1] |= (remain << (32 - small_shift));
		}
		num[i] >>= small_shift;
	}
	for (int i = n - big_shift; i < n; ++i) {
		if (c) num[i] = 0;
		else num[i] = BASE - 1;
	}
	int ind = n - 1 - big_shift;
	if (ind >= 0) {
		if (c) {
			num[ind] <<= (small_shift + 32);
			num[ind] >>= (small_shift + 32);
		} else {
			num[ind] |= uint32_t(((1 << small_shift) - 1) << (32 - small_shift));
		}
	}
	if ((num[n - 1] & (1ll << 31)) != 0) {
		--*this;
		for (int i = 0; i < n; ++i) {
			num[i] = uint32_t(~num[i]);
		}
	}
	sgn = c;
	clean();
	check_zero();
	return *this;
}

big_integer& big_integer::operator<<=(int shift) {
	int big_shift = shift / 32;
	int small_shift = shift % 32;
	int n = n_bits() + big_shift + 1;
	reserve(n);
	for (int i = n-2; i >= big_shift; --i) {
		num[i] = num[i - big_shift];
		uint32_t remain = num[i] >> (32 - small_shift);
		num[i + 1] |= remain;
		num[i] = uint32_t(num[i] << small_shift);
	}
	clean();
	check_zero();
	return *this;
}

int compare(big_integer const& a, big_integer const& b) {
    if (a.sgn != b.sgn) {
		return (a.sgn > b.sgn) ? 1 : -1;
    }
    int n = std::max(a.sz(), b.sz());
    for (int i = n - 1; i >= 0; --i) {
		uint64_t first = (i < a.sz() ? a.num[i] : 0),
		second = (i < b.sz() ? b.num[i] : 0);

		if (first < second) {
			return a.sgn ? -1 : 1;
		}
		if (first > second) {
			return a.sgn ? 1 : -1;
		}
	}
	return 0;
}

bool operator==(big_integer const& a, big_integer const& b) {
	return compare(a, b) == 0;
}

bool operator!=(big_integer const& a, big_integer const& b) {
	return compare(a, b) != 0;
}

bool operator<(big_integer const& a, big_integer const& b) {
	return compare(a, b) == -1;
}

bool operator>(big_integer const &a, big_integer const &b) {
	return compare(a, b) == 1;
}

bool operator<=(big_integer const& a, big_integer const& b) {
	return compare(a, b) != 1;
}

bool operator>=(big_integer const &a, big_integer const& b) {
	return compare(a, b) != -1;
}

big_integer operator+(big_integer const& a, big_integer const& b) {
	big_integer ret = a;
	ret += b;
	return ret;
}

big_integer operator-(big_integer const &a, big_integer const& b) {
	big_integer ret = a;
	ret -= b;
	return ret;
}

big_integer operator*(big_integer const& a, big_integer const& b) {
	big_integer ret = a;
	ret *= b;
	return ret;
}

big_integer operator/(big_integer const& a, big_integer const& b) {
	big_integer ret = a;
	ret /= b;
	return ret;
}

big_integer operator%(big_integer const& a, big_integer const& b) {
	big_integer ret = a;
	ret %= b;
	return ret;
}

big_integer operator&(big_integer const& a, big_integer const& b) {
	big_integer ret = a;
	return ret &= b;
}

big_integer operator|(big_integer const& a, big_integer const& b) {
	big_integer ret = a;
	return ret |= b;
}

big_integer operator^(big_integer const& a, big_integer const& b) {
	big_integer ret = a;
	return ret ^= b;
}

big_integer operator<<(big_integer const& a, int b) {
	big_integer ret = a;
	return ret <<= b;
}

big_integer operator>>(big_integer const& a, int b) {
	big_integer ret = a;
	return ret >>= b;
}

std::string to_string(big_integer const& a) {

	int local_base = 1000;
	int n = 4 * a.sz();
	uint32_t *dem = new uint32_t[n];
	memset(dem, 0, sizeof(uint32_t) * n);
	for (int i = a.sz() - 1; i >= 0; --i) {
		for (int j = 31; j >= 0; --j) {
			int carry = 0;
			for (int k = 0; k < n - 1; ++k) {
				(dem[k] <<= 1) += carry;
				carry = dem[k] / local_base;
				dem[k] %= local_base;
			}
			if (((1 << j) & a.num[i])) {
				dem[0]++;
			}
		}
	}
	int carry = 0;
	for (int i = 0; i < n; ++i) {
		dem[i] += carry;
		carry = dem[i] / local_base;
		dem[i] %= local_base;
	}
	while (n >= 1 && !dem[n - 1]) --n;
	std::string r("");
	if (!a.sgn) r = "-";
	r += std::to_string(dem[n - 1]);
	for (int i = n - 2; i >= 0; --i) {
		std::string buff("");
		for (int j = 0; j < 3; ++j) {
			buff += (dem[i] % 10 + '0');
			dem[i] /= 10;
		}
		std::reverse(buff.begin(), buff.end());
		r += buff;
	}
	delete dem;
	return r;
}

std::ostream& operator<<(std::ostream& s, big_integer const& a)
{
    return s << to_string(a);
}
