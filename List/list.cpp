#include <iostream>
#include "list.h"

using std::string;

// NODE

list::node::node()
	: list::node("") {}

list::node::node(std::string const& value)
	: pre(nullptr),
	  nex(nullptr),
	  value(value) {}

list::node::node(node* pre, node* nex, std::string const& value)
	: pre(pre),
	  nex(nex),
	  value(value) {}

list::node::~node() {}

// ITERATOR

list::iterator::iterator()
	: link(nullptr) {}

list::iterator::iterator(node* a)
	: link(a) {}

list::iterator::~iterator() {}

list::iterator& list::iterator::operator++() {
	link = link -> nex;
	return *this;
}

list::iterator& list::iterator::operator--() {
	link = link -> pre;
	return *this;
}

std::string& list::iterator::operator*() {
	return link -> value;
}

bool operator==(list::iterator const& a, list::iterator const& b) {
	return a.link == b.link;
}

bool operator!=(list::iterator const& a, list::iterator const& b) {
	return !(a == b);
}

// CONST_ITERATOR

list::const_iterator::const_iterator()
	: link(nullptr) {}

list::const_iterator::const_iterator(node* a)
	: link(a) {}

list::const_iterator::~const_iterator() {}

list::const_iterator& list::const_iterator::operator++() {
	link = link -> nex;
	return *this;
}

list::const_iterator& list::const_iterator::operator--() {
	link = link -> pre;
	return *this;
}

std::string& list::const_iterator::operator*() const {
	return link -> value;
}

bool operator==(list::const_iterator const& a, list::const_iterator const& b) {
	return a.link == b.link;
}

bool operator!=(list::const_iterator const& a, list::const_iterator const& b) {
	return !(a == b);
}

// LIST

list::list()
	: fake_node(&fake_node, &fake_node, "") {}

list::list(list const& other)
	: fake_node(&fake_node, &fake_node, "") {
	list ret;
	for (const_iterator it = other._begin(); it != other._end(); ++it) {
		ret.push_back(*it);
	}
	auto it = ret.end();
	swap(*this, ret);
}

list& list::operator=(list const& other) {
	list ret(other);
	swap(*this, ret);
	return *this;
}

list::~list() {
	clear();
}

list::iterator list::begin() const {
	return iterator(fake_node.nex);
}

list::iterator list::end() const {
	return iterator(fake_node.nex -> pre);
}

list::const_iterator list::_begin() const {
	return const_iterator(fake_node.nex);
}

list::const_iterator list::_end() const {
	return const_iterator(fake_node.nex -> pre);
}

void list::push_back(std::string const& value) {
	insert(end(), value);
}

void list::pop_back() {
	erase(iterator(fake_node.pre));
}

void list::pop_front() {
	erase(begin());
}

void list::clear() {
	while (!empty()) {
		pop_back();
	}
}

bool list::empty() const {
	return &fake_node == fake_node.nex;
}

list::iterator list::erase(iterator pos) {
	node* a = pos.link->pre;
	node* b = pos.link->nex;
	delete pos.link;
	unite(a, b);
	return iterator(b);
}

list::iterator list::insert(iterator pos, std::string const& value) {
	node* a = pos.link->pre;
	node* b = pos.link;
	node* nw = new node(a, b, value);
	a->nex = nw;
	b->pre = nw;
	return iterator(nw);
}

void list::splice(iterator pos, iterator first, iterator last) {
	if (first == last) return;
	node* a = first.link;
	node* b = last.link->pre;
	node* p = pos.link;

	node* aa = a->pre;
	node* bb = b->nex;
	unite(aa, bb);
	unite(p->pre, a);
	unite(b, p);
//	unite(a->pre, b->nex);
	for (node* i = p->pre; i != p; i = i->nex) {
		std::cerr << i->value << '\n';
	}
}

std::string& list::back() {
	return fake_node.pre->value;
}

std::string& list::front() {
	return fake_node.nex->value;
}

void swap(list& a, list& b) {
	list::node& l = a.fake_node;
	list::node& r = b.fake_node;
	std::swap(l, r);
	if (l.nex == &r) {
		l.nex = l.pre = &l;
	} else {
		l.nex->pre = l.pre->nex = &l;
	}
	if (r.nex == &l) {
		r.nex = r.pre = &r;
	} else {
		r.nex->pre = r.pre->nex = &r;
	}
}

void list::unite(node* a, node* b) {
	a->nex = b;
	b->pre = a;
}
