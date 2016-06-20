
#ifndef DEQUE_H
#define DEQUE_H
#include <iostream>
struct deque {
	struct iterator {
		iterator();
		std::string& operator*() const;
		iterator& operator++();
		iterator& operator--();
		iterator operator++(int);
		iterator operator--(int);
		friend bool operator==(iterator a, iterator b);
		friend bool operator!=(iterator a, iterator b);
		iterator& operator+=(int a);
		iterator& operator-=(int a);
		iterator operator+(int a);
		iterator operator-(int a);
		size_t operator-(iterator other);
	private:
		friend deque;
		iterator(deque* d, int offset);
		deque *d;
		int offset;
	};

	struct const_iterator {
		const_iterator();
		std::string const& operator*() const;
		const_iterator& operator++();
		const_iterator& operator--();
		const_iterator operator++(int);
		const_iterator operator--(int);
		friend bool operator==(const_iterator a, const_iterator b);
		friend bool operator!=(const_iterator a, const_iterator b);
		const_iterator& operator+=(int a);
		const_iterator& operator-=(int a);
		const_iterator operator+(int a);
		const_iterator operator-(int a);
		size_t operator-(const_iterator other);
	private:
		friend deque;
		const_iterator(deque* d, int offset);
		deque *d;
		int offset;
	};
	deque();
	deque(size_t capacity);
	deque(deque const& other);
	deque& operator=(deque const& other);
	~deque();
	size_t size() const;
	bool empty() const;
	void clear();
	std::string& back();
	std::string const& back() const;
	void pop_back();
	void push_back(std::string const&);
	std::string& front();
	std::string const& front() const;
	void pop_front();
	void push_front(std::string const&);
	std::string& operator[](size_t pos);
	std::string const& operator[](size_t pos) const;
	iterator begin();
	iterator end();
	const_iterator cbegin() const;
	const_iterator cend() const;
	iterator erase(iterator pos);
	iterator insert(iterator pos, std::string const& x);
	iterator erase(iterator first, iterator last);
private:
	void ensure_capacity(size_t need_size);
	std::string* head;
	std::string* _first;

	friend void swap(deque& a, deque& b);
	size_t _size;
	size_t capacity;
};

bool operator==(deque::iterator a, deque::iterator b);
bool operator!=(deque::iterator a, deque::iterator b);
bool operator==(deque::const_iterator a, deque::const_iterator b);
bool operator!=(deque::const_iterator a, deque::const_iterator b);
void swap(deque& a, deque& b);

#endif
