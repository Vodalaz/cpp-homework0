#include "deque.h"

deque::iterator::iterator()
	: d(nullptr),
	  offset(0) {}

deque::iterator::iterator(deque* d, int offset)
	: d(d),
	  offset(offset) {}

std::string& deque::iterator::operator*() const {
	return (d->head)[offset];
}

deque::iterator& deque::iterator::operator++() {
	return *this += 1;
}

deque::iterator& deque::iterator::operator--() {
	return *this -= 1;
}

deque::iterator deque::iterator::operator++(int) {
	iterator ret = iterator(d, (offset + 1) % d->capacity);
	*this += 1;
	return ret;
}

deque::iterator deque::iterator::operator--(int) {
	int nof = (offset - 1 + d->capacity) % d->capacity;
	iterator ret = iterator(d, nof);
	*this -= 1;
	return ret;
}

deque::iterator& deque::iterator::operator+=(int a) {
	offset = (offset + a + d->capacity) % (d->capacity);
	return *this;
}

deque::iterator& deque::iterator::operator-=(int a) {
	offset = (offset - a + d->capacity) % (d->capacity);
	return *this;
}

deque::iterator deque::iterator::operator+(int a) {
	iterator ret = *this;
	ret += a;
	return ret;
}

deque::iterator deque::iterator::operator-(int a) {
	iterator ret = *this;
	ret -= a;
	return ret;
}

size_t deque::iterator::operator-(iterator other) {
	int a = offset;
	int b = other.offset;
	if (a >= b) {
		return a-b;
	} else {
		return a + (d->capacity - b);
	}
}

deque::const_iterator::const_iterator()
	: d(nullptr),
	  offset(0) {}

deque::const_iterator::const_iterator(deque* d, int offset)
	: d(d),
	  offset(offset) {}

std::string const& deque::const_iterator::operator*() const {
	return (d->head)[offset];
}

deque::const_iterator& deque::const_iterator::operator++() {
	return *this += 1;
}

deque::const_iterator& deque::const_iterator::operator--() {
	return *this -= 1;
}

deque::const_iterator deque::const_iterator::operator++(int) {
	const_iterator ret = const_iterator(d, (offset + 1) % d->capacity);
	*this += 1;
	return ret;
}

deque::const_iterator deque::const_iterator::operator--(int) {
	int nof = (offset - 1 + d->capacity) % (d-> capacity);
	const_iterator ret = const_iterator(d, nof);
	*this -= 1;
	return ret;
}

deque::const_iterator& deque::const_iterator::operator+=(int a) {
	offset = (offset + a + d->capacity) % (d->capacity);
	return *this;
}

deque::const_iterator& deque::const_iterator::operator-=(int a) {
	offset = (offset - a + d->capacity) % (d->capacity);
	return *this;
}

deque::const_iterator deque::const_iterator::operator+(int a) {
	const_iterator ret = *this;
	ret += a;
	return ret;
}

deque::const_iterator deque::const_iterator::operator-(int a) {
	const_iterator ret = *this;
	ret -= a;
	return ret;
}

size_t deque::const_iterator::operator-(const_iterator other) {
	int a = offset;
	int b = other.offset;
	if (a >= b) {
		return a-b;
	} else {
		return a + (d->capacity - b);
	}
}

deque::deque()
	: deque(5) {}

deque::deque(size_t capacity)
	: head((std::string*) operator new(capacity * sizeof(std::string))),
	  _first(head),
	  _size(0),
	  capacity(capacity) {}

deque::deque(deque const& other)
	:  deque(other.size())
{
	for (const_iterator it = other.cbegin(); it != other.cend(); ++it) {
		push_back(*it);
	}
}

deque& deque::operator=(deque const& other) {
	deque ret(other);
	swap(*this, ret);
	return *this;
}

deque::~deque() {
	clear();
	operator delete (head);
}

size_t deque::size() const {
	return _size;
}

bool deque::empty() const {
	return !_size;
}

void deque::clear() {
	while (!empty())
		erase(begin());
}

std::string& deque::back() {
	return *--end();
}

std::string const& deque::back() const {
	return *--cend();
}

void deque::pop_back() {
	erase(--end());
}

void deque::push_back(std::string const& x) {
	insert(end(), x);
}

std::string& deque::front() {
	return *begin();
}

std::string const& deque::front() const {
	return *cbegin();
}

void deque::pop_front() {
	erase(begin());
}

void deque::push_front(std::string const& x) {
	insert(begin(), x);
}

std::string& deque::operator[](size_t pos) {
	return *(begin() + pos);
}

std::string const& deque::operator[](size_t pos) const {
	return *(cbegin() + pos);
}

deque::iterator deque::begin() {
	return iterator(this, _first - head);
}

deque::iterator deque::end() {
	return begin() + size();
}

deque::const_iterator deque::cbegin() const  {
	return const_iterator((deque*)this, _first - head);
}

deque::const_iterator deque::cend() const {
	return cbegin() + size();
}

deque::iterator deque::insert(deque::iterator pos, std::string const& x) {
	if (_size + 1 == (size_t)capacity) {
		int num = pos - begin();
		ensure_capacity(_size+1);
		pos = begin() + num;
	}
	size_t left = pos - begin();
	size_t right = end() - pos;
	iterator ret;
	if (left < right) {
		int off = (int(_first - head) - 1 + capacity) % capacity;
		iterator cur = begin();
		new (head + off) std::string(x);
		while (cur != pos) {
			std::swap(*cur, *(cur-1));
			++cur;
		}
		_first = head + off;
		ret = (pos-1);
	} else {
		new (head + (_first - head + size()) % capacity) std::string(x); // end()
		iterator cur = end();
		while (cur != pos) {
			std::swap(*(cur - 1), *cur);
			--cur;
		}
		ret = pos;
	}
	++_size;
	return ret;
}

deque::iterator deque::erase(deque::iterator pos) {
	auto next = pos + 1;
	return erase(pos, next);
}

deque::iterator deque::erase(deque::iterator first, deque::iterator last) {
	int elements = last - first;
	int left = first - begin();
	int right = end() - last;
	iterator ret;
	if (left < right) {
		iterator cur = first;
		while (cur != begin()) {
			--cur;
			std::swap(*cur, *(cur + elements));
		}
		for (auto it = begin(); it != begin() + elements; ++it) {
			(*it).std::string::~string();
		}
		_first = head + (_first - head + elements) % capacity;
		ret = last;
	}
	else {
		for (auto it = last; it != end(); ++it) {
			std::swap(*it, *(it - elements));
		}
		for (auto it = end() - elements; it != end(); ++it) {
			(*it).std::string::~string();
		}
		ret = first;
	}
	_size -= elements;
	return ret;
}

bool operator==(deque::iterator a, deque::iterator b) {
	return a.d == b.d && a.offset == b.offset;
}

bool operator!=(deque::iterator a, deque::iterator b) {
	return !(a == b);
}

void deque::ensure_capacity(size_t need_size) {
	if (need_size >= capacity) {
		need_size *= 2;
		deque nw(need_size);
		for (const_iterator it = cbegin(); it != cend(); ++it) {
			nw.push_back(*it);
		}
		swap(*this, nw);
	}
}

bool operator==(deque::const_iterator a, deque::const_iterator b) {
	return a.d == b.d && a.offset == b.offset;
}

bool operator!=(deque::const_iterator a, deque::const_iterator b) {
	return !(a == b);
}

void swap(deque& a, deque& b) {
	std::swap(a.head, b.head);
	std::swap(a._first, b._first);
	std::swap(a._size, b._size);
	std::swap(a.capacity, b.capacity);
}
