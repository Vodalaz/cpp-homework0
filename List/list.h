#ifndef LIST_H
#define LIST_H

struct list {
	struct node {
		node();
		node(std::string const& value);
		node(node* pre, node* nex, std::string const& value);
		~node();
	private:
		node* pre;
		node* nex;
		std::string value;
		friend list;
		friend void swap(list& a, list& b);
	};

	struct iterator {
		iterator();
		iterator(node* a);
		~iterator();
		iterator& operator++();
		iterator& operator--();
		std::string& operator*();
		friend bool operator==(iterator const& a, iterator const& b);
		friend bool operator!=(iterator const& a, iterator const& b);
		node* link;
	};

	struct const_iterator {
		const_iterator();
		const_iterator(node*);
		~const_iterator();
		const_iterator& operator++();
		const_iterator& operator--();
		std::string& operator*() const;
		bool operator==(iterator const& other);
		bool operator!=(iterator const& other);
		node* link;
	};


	list();
	list(list const&);
	list& operator=(list const&);
	~list();

	void push_back(std::string const& value);
	void pop_back();
	void pop_front();
	void clear();
	bool empty() const;
	void splice(iterator pos, iterator first, iterator last);
	iterator begin() const;
	iterator end() const;
	const_iterator _begin() const;
	const_iterator _end() const;
	iterator erase(iterator pos);
	iterator insert(iterator pos, std::string const& value);
	std::string& back();
	std::string& front();
	std::string const& back() const;
	std::string const& front() const;
	friend void swap(list& a, list& b);
private:
	node fake_node;
	void unite(node* a, node *b);
};

void swap(list& a, list& b);

void splice(list::iterator pos, list::iterator first, list::iterator last);

bool operator==(list::iterator const& a, list::iterator const& b);
bool operator!=(list::iterator const& a, list::iterator const& b);

bool operator==(list::const_iterator const& a, list::const_iterator const& b);
bool operator!=(list::const_iterator const& a, list::const_iterator const& b);
#endif
