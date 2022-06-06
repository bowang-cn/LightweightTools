#pragma once
#include <iterator>

// SkipList's iterator
template<class Node, class Value>
class CSkipListIterator : public std::iterator<std::forward_iterator_tag, Value>
{
	typedef Node node_t;
	typedef Value value_t;
	typedef CSkipListIterator<Node, value_t> iter_t;

public:
	typedef std::iterator<std::forward_iterator_tag, Value> base_t;
	typedef typename base_t::pointer pointer;
	typedef typename base_t::reference reference;

	CSkipListIterator() :m_pCurrent(nullptr) {}
	explicit CSkipListIterator(node_t *pCurrent) :m_pCurrent(pCurrent) {}

	template<class Node1, class Value1, class Node2, class Value2>
	friend bool operator== (const CSkipListIterator<Node1, Value1>&, const CSkipListIterator<Node2, Value2>&);
	template<class Node1, class Value1, class Node2, class Value2>
	friend bool operator!= (const CSkipListIterator<Node1, Value1>&, const CSkipListIterator<Node2, Value2>&);

	reference operator*() const
	{
		return m_pCurrent->m_aData;
	}

	pointer operator->() const
	{
		return &(m_pCurrent->m_aData);
	}

	iter_t& operator++ ()
	{
		m_pCurrent = m_pCurrent->next();
		return *this;
	}
	iter_t operator++ (int)
	{
		iter_t temp = *this;
		++(*this);
		return temp;
	}

private:
	node_t *m_pCurrent;
};

template<class Node1, class Value1, class Node2, class Value2>
bool operator== (const CSkipListIterator<Node1, Value1> &iter1, const CSkipListIterator<Node2, Value2> &iter2)
{
	return std::is_same<Node1, Node2>::value && std::is_same<Value1, Value2>::value && (iter1.m_pCurrent == iter2.m_pCurrent);
}
template<class Node1, class Value1, class Node2, class Value2>
bool operator!= (const CSkipListIterator<Node1, Value1> &iter1, const CSkipListIterator<Node2, Value2> &iter2)
{
	return !(iter1 == iter2);
}