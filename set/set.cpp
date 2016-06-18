#include "treap.h"

node::node(std::string const& key)
	: key(key),
	  rank(std::rand()),
	  l_son(nullptr),
	  r_son(nullptr),
	  prev(nullptr),
	  next(nullptr),
	  parent(nullptr) {}

node::node() {}

set::set()
	: _size(0),
	  root(nullptr) {
		fake_node.next = fake_node.prev = &fake_node;
	}

set::set(set const& other) {
	set ret;
	for (iterator it = other.begin(); it != other.end(); ++it) {
		ret.insert(*it);
	}
	_size = 0;
	fake_node.next = fake_node.prev = &fake_node;
	swap(*this, ret);
}

set& set::operator=(set const& other) {
	set ret(other);
	swap(*this, ret);
}

set::~set() {
	clear();
}

int set::size() const {
	return _size;
}

bool set::empty() const {
	return !_size;
}

void set::clear() {
	while (!empty()) {
		erase(begin());
	}
}

set::iterator set::find(std::string const& key) {
	node* cur = root;
	node* ret = &fake_node;
	while (cur) {
		if (cur->key == key) {
			ret = cur;
			break;
		}
		if (key < cur->key) {
			cur = cur->l_son;
		} else {
			cur = cur->r_son;
		}
	}
	return iterator(ret);
}

set::iterator set::erase(std::string const& key) {
	iterator pos = find(key);
	if (pos == end()) {
		return end();
	} else {
		return erase(pos);
	}
}

set::iterator set::erase(iterator pos) {
	node* a = pos.link;
	connect(a->prev, a->next);
	iterator ret = iterator(a->next);
	node* nw = merge(a->l_son, a->r_son);
	if (a->parent) {
		if (is_left_son(a, a->parent)) {
			a->parent->l_son = nw;
		} else {
			a->parent->r_son = nw;
		}
	} else {
		root = nw;
	}
	if (nw) {
		nw->parent = a->parent;
	}
	delete a;
	--_size;
	return ret;
}

set::iterator set::insert(std::string const& key) {
	iterator pos = find(key);
	if (pos != end()) {
		return pos;
	}
	node* b = upper_bound(key).link;
	node* a = b->prev;
	node* nw = new node(key);

	connect(a, nw);
	connect(nw, b);

	std::pair<node*, node*> tmp = split(root, key);
	root = merge(merge(tmp.first, nw), tmp.second);

	++_size;

	return iterator(nw);
}

set::iterator set::lower_bound(std::string const& key) {
	node* cur = root;
	node* ret = &fake_node;
	while (cur) {
		if (key <= cur->key) {
			ret = cur;
			cur = cur->l_son;
		} else {
			cur = cur->r_son;
		}
	}
	return iterator(ret);
}

set::iterator set::upper_bound(std::string const& key) {
	node* cur = root;
	node* ret = &fake_node;
	while (cur) {
		if (key < cur->key) {
			ret = cur;
			cur = cur->l_son;
		} else {
			cur = cur->r_son;
		}
	}
	return iterator(ret);
}

set::iterator set::begin() const {
	return iterator(fake_node.next);
}

set::iterator set::end() const {
	return iterator(fake_node.next->prev);
}

set::iterator::iterator()
	: link(nullptr) {}

set::iterator::iterator(node* a)
	: link(a) {}

std::string const& set::iterator::operator*() const {
	return link->key;
}

set::iterator& set::iterator::operator--() {
	link = link->prev;
	return *this;
}

set::iterator& set::iterator::operator++() {
	link = link->next;
	return *this;
}

set::iterator set::iterator::operator--(int) {
	iterator ret = iterator(link);
	--*this;
	return ret;
}

set::iterator set::iterator::operator++(int) {
	iterator ret = iterator(link);
	++*this;
	return ret;
}

bool operator==(set::iterator a, set::iterator b) {
	return a.link == b.link;
}

bool operator!=(set::iterator a, set::iterator b) {
	return !(a == b);
}

std::pair<node*, node*> split(node* root, std::string const& x) {
	if (!root) {
		return std::make_pair(nullptr, nullptr);
	}
	if (x < root->key) {
		std::pair<node*, node*> tmp = split(root->l_son, x);
		root->l_son = tmp.second;
		update(root);
		return std::make_pair(tmp.first, root);
	} else {
		std::pair<node*, node*> tmp = split(root->r_son, x);
		root->r_son = tmp.first;
		update(root);
		return std::make_pair(root, tmp.second);
	}
}

node* merge(node* a, node* b) {
	if (!a) return b;
	if (!b) return a;
	if (a->rank < b->rank) {
		a->r_son = merge(a->r_son, b);
		update(a);
		return a;
	} else {
		b->l_son = merge(a, b->l_son);
		update(b);
		return b;
	}
}

void update(node* a) {
	if (a->l_son) {
		a->l_son->parent = a;
	}
	if (a->r_son) {
		a->r_son->parent = a;
	}
}

void connect(node* a, node* b) {
	a->next = b;
	b->prev = a;
}

bool is_left_son(node* son, node* parent) {
	return parent->l_son == son;
}

void swap(set& a, set& b) {
	std::swap(a.root, b.root);
	std::swap(a._size, b._size);
	node& l = a.fake_node;
	node& r = b.fake_node;
	std::swap(l, r);
	if (l.next == &r) {
		l.next = l.prev = &l;
	} else {
		l.next->prev = l.prev->next = &l;
	}
	if (r.next == &l) {
		r.next = r.prev = &r;
	} else {
		r.next->prev = r.prev->next = &r;
	}
}

