#ifndef BIG_INTEGER_H
#define BIG_INTEGER_H
#include <iosfwd>

const uint64_t BASE = (1ll << 32);

struct big_integer {
public:
	big_integer();
	big_integer(big_integer const& other);
	big_integer(int a);
	explicit big_integer(std::string const&);
	~big_integer();

	big_integer& operator=(big_integer const&);

	big_integer& operator+=(big_integer const&);
	big_integer& operator-=(big_integer const&);
	big_integer& operator*=(big_integer const&);
	big_integer& operator/=(big_integer const&);
	big_integer& operator%=(big_integer const&);

	big_integer& operator&=(big_integer const&);
	big_integer& operator|=(big_integer const&);
	big_integer& operator^=(big_integer const&);

	big_integer& operator<<=(int);
	big_integer& operator>>=(int);

	big_integer operator+() const;
	big_integer operator-() const;
	big_integer operator~() const;

	big_integer& operator++();
    big_integer operator++(int);

    big_integer& operator--();
    big_integer operator--(int);

	friend int compare(big_integer const&, big_integer const&);
    friend bool operator==(big_integer const&, big_integer const&);
    friend bool operator!=(big_integer const&, big_integer const&);
    friend bool operator<(big_integer const&, big_integer const&);
    friend bool operator>(big_integer const&, big_integer const&);
    friend bool operator<=(big_integer const&, big_integer const&);
    friend bool operator>=(big_integer const&, big_integer const&);

	friend std::string to_string(big_integer const&);

	int sz() const;
private:
	void check_zero();
	void binary_oper(big_integer const&, uint64_t(*)(uint64_t, uint64_t));
	void reserve(int);
	void clean();
	int n_bits() const;
	uint64_t *num;
	uint32_t bits;
	bool sgn;
};

int compare(big_integer const&, big_integer const&);
bool operator==(big_integer const&, big_integer const&);
bool operator!=(big_integer const&, big_integer const&);
bool operator<(big_integer const&, big_integer const&);
bool operator>(big_integer const&, big_integer const&);
bool operator<=(big_integer const&, big_integer const&);
bool operator>=(big_integer const&, big_integer const&);

std::string to_string(big_integer const&);

big_integer operator+(big_integer const& a, big_integer const& b);
big_integer operator-(big_integer const& a, big_integer const& b);
big_integer operator*(big_integer const& a, big_integer const& b);
big_integer operator/(big_integer const& a, big_integer const& b);
big_integer operator%(big_integer const& a, big_integer const& b);
big_integer operator&(big_integer const&, big_integer const&);
big_integer operator|(big_integer const&, big_integer const&);
big_integer operator^(big_integer const&, big_integer const&);
big_integer operator>>(big_integer const&, int);
big_integer operator<<(big_integer const&, int);
uint64_t binary_and(uint64_t a, uint64_t b);
uint64_t binary_or(uint64_t a, uint64_t b);
uint64_t binary_xor(uint64_t a, uint64_t b);

std::ostream& operator<<(std::ostream& s, big_integer const& a);

#endif
