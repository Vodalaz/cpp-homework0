#ifndef BIG_INTEGER_H
#define BIG_INTEGER_H


typedef uint64_t ui64;

const ui64 BASE = (1ll << 32);
const int BASEPOW = 32;

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

	ui64 element(int pos) const;
	int sz() const;
	bool get_sgn() const;

private:
	bool sgn;
	std::vector< ui64 > vec;
	void apply_binary(big_integer const&, ui64(*)(ui64, ui64));
	void verify_sign();
    void to_byte();
    void from_byte();
};

int compare(big_integer const&, big_integer const&);
bool operator==(big_integer const&, big_integer const&);
bool operator!=(big_integer const&, big_integer const&);
bool operator<(big_integer const&, big_integer const&);
bool operator>(big_integer const&, big_integer const&);
bool operator<=(big_integer const&, big_integer const&);
bool operator>=(big_integer const&, big_integer const&);

std::string to_string(big_integer const&);

big_integer operator+(big_integer a, big_integer const& b);
big_integer operator-(big_integer a, big_integer const& b);
big_integer operator*(big_integer a, big_integer const& b);
big_integer operator/(big_integer a, big_integer const& b);
big_integer operator%(big_integer a, big_integer const& b);
big_integer operator&(big_integer, big_integer const&);
big_integer operator|(big_integer, big_integer const&);
big_integer operator^(big_integer, big_integer const&);
big_integer operator>>(big_integer, int);
big_integer operator<<(big_integer, int);

std::ostream& operator<<(std::ostream& s, big_integer const& a);

void push(std::vector<ui64>&a, int pos);
void del_zeros(std::vector<ui64>&);
void mul_long_short(std::vector<ui64>& a, ui64 base, ui64 b);
void div_long_short(std::vector<ui64>& a, ui64 base, ui64 b);
void add_long_short(std::vector<ui64>&a, ui64 base, ui64 b);
ui64 _and(ui64 a, ui64 b);
ui64 _or(ui64 a, ui64 b);
ui64 _xor(ui64 a, ui64 b);

#endif
