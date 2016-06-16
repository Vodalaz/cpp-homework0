#include <iostream>
#include <algorithm>
#include <vector>
#include "big_integer.h"

big_integer::big_integer() : big_integer(0) {}

big_integer::big_integer(int value)
	: sgn(0 <= value),
	  vec(1, std::abs((int64_t)value)) {}

big_integer::big_integer(big_integer const& other) {
	vec.resize(other.vec.size());
	copy(other.vec.begin(), other.vec.end(), vec.begin());
	sgn = other.sgn;
}

big_integer::~big_integer() {
	vec.clear();
}

big_integer::big_integer(std::string const& str) {
	vec = {0};
	ui64 base = 10;
	std::vector<ui64> demical;

	for (int i = (int) str.size()-1; i >= 0; --i) {
		if (str[i] != '-')
			demical.push_back(str[i] - '0');
	}

	ui64 ptr = 0;
	while (demical.size() > 1 || demical.back()) {
		if (demical[0] & 1) {
			vec.back() |= (1UL << ptr);
		}
		++ptr;
		div_long_short(demical, base, 2);
		if (ptr == 32) {
			ptr = 0;
			vec.push_back(0);
		}
	}
	sgn = (str[0] != '-');
	del_zeros(vec); verify_sign();
}

big_integer& big_integer::operator=(big_integer const& other) = default;

big_integer& big_integer::operator+=(big_integer const& other) {
	if (sgn == other.sgn) {

		vec.resize(std::max(vec.size(), other.vec.size())+1);
		for (size_t i = 0; i != vec.size()-1; ++i) {
			vec[i] += other.element(i);
			push(vec, i);
		}
		del_zeros(vec);

	} else {
		if (sgn) {
			*this -= (-other);
		} else {
			*this = -(-*this - other);
		}
	}
	return *this;
}

big_integer& big_integer::operator-=(big_integer const& other) {
	if (sgn == other.sgn) {

		int n = std::max(vec.size(), other.vec.size());
		vec.resize(n);

		int carry = 0;
		for (int i = 0; i < n; ++i) {
			int64_t val = (int64_t) element(i) - other.element(i) - carry;
			carry = (val < 0);
			vec[i] = (val + BASE) % BASE;
		}

		if (carry) {
			carry = 0;
			sgn ^= 1;
			for (int i = 0; i < n; ++i) {
				int64_t val = -(int64_t) element(i) - carry;
				carry = (val < 0);
				vec[i] = (val + BASE) % BASE;
			}
		}

		del_zeros(vec); verify_sign();
	} else {
		if (sgn) {
			*this += -other;
		} else {
			*this = -(-*this + other);
		}
	}
	return *this;
}




big_integer& big_integer::operator*=(big_integer const& other) {

	std::vector<ui64> buf(vec.size() + other.vec.size());

	for (size_t i = 0; i != vec.size(); ++i) {
		for (size_t j = 0; j != other.vec.size(); ++j) {
			buf[i+j] += vec[i] * other.vec[j];
			push(buf, i+j);
		}
	}
	vec = buf;
	for (size_t i = 0; i != vec.size()-1; ++i) {
		push(vec, i);
	}
	sgn = !(sgn ^ other.sgn);
	del_zeros(vec); verify_sign();

	return *this;
}



big_integer& big_integer::operator/=(big_integer const& other) {

	int n = vec.size();
	int m = other.vec.size();

	std::vector<ui64> cur;
	std::vector<ui64> ans(n-m+1);

	for (int i = n-m; i >= 0; --i) {
		ui64 l = 0;
		double a = other.element(m-1) + other.element(m-2) * 1.0 / BASE;
		double b = element(i+m-1) + element(i+m) * BASE;
		ui64 r = b / a + 2;

		l = std::max(0, int(r - 3));

		while (l + 1 < r) {
			ui64 m = (l+r) / 2;
			cur = other.vec;
			mul_long_short(cur, BASE, m);

			// fast check or ((m * other) << 32 * i) <= *this

			bool less_or_equal = true;
			if (cur.size() + i != vec.size()) {
				if (cur.size() + i > vec.size()) less_or_equal = false;
			} else {
				for (int j = cur.size()-1; j >= 0; --j) {
					if (vec[j+i] != cur[j]) {
						if (cur[j] > vec[j+i]) less_or_equal = false;
						break;
					}
				}
			}
			if (less_or_equal) {
				l = m;
			} else {
				r = m;
			}
		}

		// fast subtracts (cur << 32*i) from *this
		cur = other.vec;
		mul_long_short(cur, BASE, l);
		int64_t carry = 0;
		for (size_t j = 0; j != cur.size(); ++j) {
			int64_t val = (int64_t) vec[i+j] - cur[j] -carry;
			carry = (val < 0);
			vec[i+j] = (val + BASE) % BASE;
		}
		if (carry) {
			vec[cur.size()+i]--;
		}
		del_zeros(vec);

		ans[i] = l;
	}
	vec = ans;
	if (vec.empty()) {
		vec.push_back(0);
	}
	sgn = !(sgn ^ other.sgn);
	del_zeros(vec); verify_sign();
	return *this;
}

big_integer& big_integer::operator%=(big_integer const& other) {
	big_integer a = *this;
	a /= other;
	a *= other;
	return *this -= a;
}

ui64 _and(ui64 a, ui64 b) {
	return a & b;
}

ui64 _or(ui64 a, ui64 b) {
	return a | b;
}

ui64 _xor(ui64 a, ui64 b) {
	return a ^ b;
}

void big_integer::apply_binary(big_integer const& other, ui64 (*func)(ui64, ui64)) {
	big_integer b(other);
	ui64 first = (sgn ? 0 : BASE-1);
	ui64 second = (b.sgn ? 0 : BASE-1);
	to_byte();
	b.to_byte();
	size_t n = std::max(vec.size(), b.vec.size());
	for (size_t i = 0; i != n; ++i) {
		if (i == vec.size())
			vec.push_back(first);
		vec[i] = (*func)(vec[i], (i < b.vec.size() ? b.vec[i] : second));
	}
	from_byte();
	del_zeros(vec); verify_sign();
}

big_integer& big_integer::operator&=(big_integer const& other) {
	apply_binary(other, _and);
	return *this;
}

big_integer& big_integer::operator|=(big_integer const& other) {
	apply_binary(other, _or);
	return *this;
}

big_integer& big_integer::operator^=(big_integer const& other) {
	apply_binary(other, _xor);
	return *this;
}

big_integer big_integer::operator+() const {
	return *this;
}

big_integer big_integer::operator~() const {
	big_integer ret = *this;
	ret = -ret - 1;
	return ret;
}

big_integer big_integer::operator-() const {
	big_integer ret = *this;
	ret.sgn ^= 1;
	ret.verify_sign();
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
	int big_shift = shift / BASEPOW;
	int small_shift = shift % BASEPOW;
	ui64 val = (sgn ? 0 : BASE-1);
	to_byte();
	for (int i = 0; i + big_shift < (int) vec.size(); ++i) {
		vec[i] = vec[i + big_shift];
		if (big_shift) {
			vec[i + big_shift] = val;
		}
		if (i) {
			vec[i-1] %= (1UL << (32 - small_shift));
			vec[i-1] |= ((vec[i] % (1UL << small_shift)) << (32-small_shift));
		}
		vec[i] >>= small_shift;
		vec[i] |= (val << (32 - small_shift)) % BASE;
	}
	from_byte();
	del_zeros(vec);
	return *this;
}

big_integer& big_integer::operator<<=(int shift) {
	int big_shift = shift / BASEPOW;
	int small_shift = shift % BASEPOW;

	int n = vec.size() + big_shift + 1;
	vec.resize(n);

	for (int i = n-2; i >= big_shift; --i) {
		vec[i] = vec[i - big_shift];
		if (big_shift)
			vec[i - big_shift] = 0;
		vec[i + 1] |= (vec[i] >> (32 - small_shift));
		vec[i] = (vec[i] << small_shift)%BASE;
	}
	del_zeros(vec);
	return *this;
}

int compare(big_integer const& a, big_integer const& b) {
	big_integer ret = a - b;
	if (!ret.sgn) {
		return -1;
	} else if (ret.sz() > 1 || ret.element(0)) {
		return 1;
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

big_integer operator+(big_integer a, big_integer const& b) {
	return a += b;
}

big_integer operator-(big_integer a, big_integer const& b) {
	return a -= b;
}

big_integer operator*(big_integer a, big_integer const& b) {
	return a *= b;
}

big_integer operator/(big_integer a, big_integer const& b) {
	return a /= b;
}

big_integer operator%(big_integer a, big_integer const& b) {
	return a %= b;
}

big_integer operator&(big_integer a, big_integer const& b) {

	return a &= b;
}

big_integer operator|(big_integer a, big_integer const& b) {
	return a |= b;
}

big_integer operator^(big_integer a, big_integer const& b) {
	return a ^= b;
}

big_integer operator<<(big_integer a, int b) {
	return a <<= b;
}

big_integer operator>>(big_integer a, int b) {
	return a >>= b;
}

std::string to_string(big_integer const& a) {
	ui64 base = 10;
	std::vector<ui64> demical(1, 0);
	std::string ret;

	for (int i = a.sz()-1; i >= 0; --i) {
		mul_long_short(demical, base, BASE);
		add_long_short(demical, base, a.element(i));
	}

	std::reverse(demical.begin(), demical.end());
	for (auto digit : demical) {
		ret += std::to_string(digit);
	}

	if (!a.get_sgn()) {
		ret = "-" + ret;
	}

	return ret;
}

std::ostream& operator<<(std::ostream& s, big_integer const& a)
{
    return s << to_string(a);
}

ui64 big_integer::element(int pos) const {
	return ((0 <= pos && pos < (int) vec.size()) ? vec[pos] : 0);
}

int big_integer::sz() const {
	return vec.size();
}

bool big_integer::get_sgn() const {
	return sgn;
}

void push(std::vector<ui64>& a, int pos) {
	a[pos+1] += a[pos] / BASE;
	a[pos] %= BASE;
}

void del_zeros(std::vector<ui64>& a) {
	while (!a.back() && a.size() > 1) {
		a.pop_back();
	}
}

void big_integer::verify_sign() {
	if (vec.size() == 1 && !vec.back()) {
		sgn = true;
	}
}


void mul_long_short(std::vector<ui64>& a, ui64 base, ui64 b) {
	ui64 carry = 0;
	for (int i = 0; i < (int) a.size() || carry; ++i) {

		if (i == (int) a.size()) {
			a.push_back(0);
		}

		ui64 val = a[i] * b + carry;
		a[i] = val % base;
		carry = val / base;
	}
	del_zeros(a);
}

void div_long_short(std::vector<ui64>& a, ui64 base, ui64 b) {
	ui64 carry = 0;
	for (int i = a.size()-1; i >= 0; --i) {
		ui64 val = a[i] + carry * base;

		a[i] = val / b;
		carry = val % b;
	}
	del_zeros(a);
}

void add_long_short(std::vector<ui64>& a, ui64 base, ui64 b) {
	ui64 carry = b;
	for (int i = 0; carry; ++i) {

		if (i == (int) a.size()) {
			a.push_back(0);
		}

		ui64 val = a[i] + carry;
		a[i] = val % base;
		carry = val / base;
	}
	del_zeros(a);
}

void big_integer::to_byte() {
	if (!sgn) {
        vec.push_back(0);
		for (auto &x : vec) {
			x = uint32_t(~x);
		}
		sgn = true;
		++*this;
	}
}

void big_integer::from_byte() {
	if (vec.back() & (1UL << 31)) {
		--*this;
		for (auto &x : vec) {
			x = uint32_t(~x);
		}
		sgn = false;
	} else {
		sgn = true;
	}
}

