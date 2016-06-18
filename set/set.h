#ifndef TREAP_H
#define TREAP_H
#include <iostream>

struct node {
	node(std::string const& key);
	node();
	std::string key;
	int rank;
	node* l_son;
	node* r_son;
	node* prev;
	node* next;
	node* parent;
};

struct set {
	struct iterator {
		iterator();
		iterator(node* link);
		std::string const& operator*() const;
		iterator& operator--();
		iterator& operator++();
		iterator operator--(int);
		iterator operator++(int);
		friend struct set;
		friend bool operator==(iterator a, iterator b);
		friend bool operator!=(iterator a, iterator b);
	private:
		node* link;
	};
	set();
	set(set const& other);
	set& operator=(set const& other);
	~set();
	int size() const;
	bool empty() const;
	void clear();
	iterator find(std::string const& key);
	iterator erase(std::string const& key);
	iterator erase(iterator pos);
	iterator insert(std::string const& key);
	iterator lower_bound(std::string const& key);
	iterator upper_bound(std::string const& key);
	iterator begin() const;
	iterator end() const;
	friend void swap(set& a, set& b);
private:
	int _size;
	node* root;
	node fake_node;
};

bool operator==(set::iterator a, set::iterator b);
bool operator!=(set::iterator a, set::iterator b);
std::pair<node*, node*> split(node* root, std::string const& x);
node* merge(node* a, node* b);
void update(node* a);
void connect(node* a, node* b);
bool is_left_son(node* son, node* parent);
void swap(set& a, set& b);

#endif
