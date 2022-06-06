#pragma once
#include "../NonCopyable.h"
#include "SkipListIterator.h"
#include "SkipListLevelCreator.h"
#include "Range.h"

template<class Key,                                        // key
	class Value,                                           // value
	int nMaxLevel = 20,                                    // max level
	class Less = std::less<Key>,                           // Compare
	class LevelCreator = CSkipListLevelCreator,            // level creator
	template<class> class Alloc = std::allocator>          // allocator
class CSkipList : private CNonCopyable
{
	static_assert(nMaxLevel > 0, "maxLevel must be a positive integer");

	using key_t = std::add_const_t<Key>;
	using value_t = Value;

	struct Node : private CNonCopyable
	{
		typedef std::pair<key_t, value_t> value_type;

		int           m_nLevel;
		Node          **m_pNext;
		value_type    m_aData;

		Node(int nLevel, key_t k, value_t v) : m_nLevel(nLevel), m_aData(std::move(k), std::move(v)) {}

		const key_t& get_key() const { return m_aData.first; }
		const value_t& get_value() const { return m_aData.second; }

		Node* next() { return (nullptr == m_pNext ? nullptr : m_pNext[0]); }
		const Node* next() const { return const_cast<Node*>(this)->next(); }
	};

public:
	typedef Less key_compare_t;
	typedef LevelCreator level_creator_t;
	typedef CSkipListIterator<Node, typename Node::value_type> iterator;
	typedef CSkipListIterator<const Node, const typename Node::value_type> const_iterator;
	typedef CRange<iterator> range;
	typedef CRange<const_iterator> const_range;
	typedef Alloc<Node> allocator;
	typedef typename allocator::template rebind<Node*>::other ptr_allocator;

	CSkipList() :m_nCurrentLevel(0), m_pHead(nullptr), m_nCount(0)
	{
		create_head();
	}
	~CSkipList()
	{
		clear();
		release_head();
	}

	size_t size() const { return m_nCount; }

	bool insert(key_t aKey, value_t aValue)
	{
		if (nullptr == m_pHead) {
			return false;
		}

		auto pCurrentNode = find_pos_not_less(aKey, [this](int nLevel, Node *pNode) { m_arrSearchPath[nLevel] = pNode; });
		if (nullptr != pCurrentNode && pCurrentNode->get_key() == aKey) {
			return false;   // exist
		}

		int nLevel = m_aLevelCreator(nMaxLevel);

		Node *pInsert = create_node(std::move(aKey), std::move(aValue), nLevel + 1);
		if (nullptr == pInsert) {
			return false;
		}

		if (nLevel > m_nCurrentLevel) {
			m_nCurrentLevel = nLevel;
		}

		for (int i = 0; i <= nLevel; i++) {
			pInsert->m_pNext[i] = m_arrSearchPath[i]->m_pNext[i];
			m_arrSearchPath[i]->m_pNext[i] = pInsert;
		}
		++m_nCount;

		return true;
	}

	const_range find(const key_t &key) const { return const_range(find_key(key, true)); }
	range find(const key_t &key) { return range(find_key(key, true)); }

	const_range find(const key_t &kFrom, const key_t &kTo) const { return const_range(find_range(kFrom, kTo)); }
	range find(const key_t &kFrom, const key_t &kTo) { return range(find_range(kFrom, kTo)); }

	iterator begin() { return iterator(nullptr != m_pHead ? m_pHead->next() : nullptr); }
	const_iterator begin() const { return const_iterator(nullptr != m_pHead ? m_pHead->next() : nullptr); }

	iterator end() { return iterator(); }
	const_iterator end() const { return const_iterator(); }

	size_t erase(const key_t &key)
	{
		if (nullptr == m_pHead) {
			return 0;
		}

		auto pCurrentNode = find_pos_not_less(key, [this](int nLevel, Node *pNode) { m_arrSearchPath[nLevel] = pNode; });

		if (nullptr != pCurrentNode && pCurrentNode->get_key() == key) {
			for (int i = 0; i <= m_nCurrentLevel; i++) {
				if (m_arrSearchPath[i]->m_pNext[i] != pCurrentNode) {
					break;
				}

				m_arrSearchPath[i]->m_pNext[i] = pCurrentNode->m_pNext[i];
			}

			while (m_nCurrentLevel > 0 && nullptr == m_pHead->m_pNext[m_nCurrentLevel]) {
				--m_nCurrentLevel;
			}
			--m_nCount;

			release_node(pCurrentNode);

			return 1;
		}

		return 0;
	}

	void clear()
	{
		if (nullptr == m_pHead) {
			return;
		}

		auto pNode = m_pHead->next();
		while (nullptr != pNode) {
			auto *pCurrent = pNode;
			pNode = pNode->next();
			release_node(pCurrent);
		}

		memset(m_pHead->m_pNext, 0, sizeof(Node*) * nMaxLevel);
	}

	CSkipList(CSkipList &&r)
	{
		*this = std::move(r);
	}
	CSkipList& operator= (CSkipList &&r)
	{
		swap(r);
		return *this;
	}

	void swap(CSkipList &aSkipList)
	{
		if (this != &aSkipList) {
			std::swap(m_nCurrentLevel, aSkipList.m_nCurrentLevel);
			std::swap(m_pHead, aSkipList.m_pHead);
			std::swap(m_arrSearchPath, aSkipList.m_arrSearchPath);
			std::swap(m_nCount, aSkipList.m_nCount);
			std::swap(m_aKeyCompare, aSkipList.m_aKeyCompare);
			std::swap(m_aAllocator, aSkipList.m_aAllocator);
			std::swap(m_aLevelCreator, aSkipList.m_aLevelCreator);
			std::swap(m_aPtrAllocator, aSkipList.m_aPtrAllocator);
		}
	}

private:
	void create_head()
	{
		m_pHead = m_aAllocator.allocate(nMaxLevel);
		if (nullptr != m_pHead) {
			m_pHead->m_pNext = m_aPtrAllocator.allocate(nMaxLevel);
			if (nullptr != m_pHead->m_pNext) {
				m_pHead->m_nLevel = nMaxLevel;
				memset(m_pHead->m_pNext, 0, sizeof(Node*) * nMaxLevel);
			}
			else {
				m_aAllocator.deallocate(m_pHead, 1);
			}
		}
	}
	void release_head()
	{
		if (nullptr != m_pHead) {
			assert(nullptr != m_pHead->m_pNext); 
			m_aPtrAllocator.deallocate(m_pHead->m_pNext, m_pHead->m_nLevel);
			m_aAllocator.deallocate(m_pHead, 1);
			m_pHead = nullptr;
		}
	}

	Node* create_node(key_t key, value_t value, int nLevel)
	{
		Node *pNode = nullptr;
		do 
		{
			pNode = m_aAllocator.allocate(1);
			if (nullptr == pNode) {
				break;
			}

			pNode->m_pNext = m_aPtrAllocator.allocate(nLevel);
			if (nullptr == pNode->m_pNext) {
				m_aAllocator.deallocate(pNode, 1);
				break;
			}

			try {
				m_aAllocator.construct(pNode, nLevel, std::move(key), std::move(value));
			} catch (...) {
				m_aPtrAllocator.deallocate(pNode->m_pNext, pNode->m_nLevel);
				m_aAllocator.deallocate(pNode, 1);
				pNode = nullptr;
			}
		} while (false);

		return pNode;
	}
	void release_node(Node *pNode)
	{
		assert(nullptr != pNode && nullptr != pNode->m_pNext);
		assert(pNode->m_nLevel > 0);
		m_aPtrAllocator.deallocate(pNode->m_pNext, pNode->m_nLevel);
		m_aAllocator.destroy(pNode);
		m_aAllocator.deallocate(pNode, 1);
	}

	template<class Op>
	Node* find_pos_not_less(const key_t &key, Op fnOp) const
	{
		assert(nullptr != m_pHead);
		auto pCurrent = m_pHead;

		for (auto i = nMaxLevel - 1; i >= 0; i--) {
			while (nullptr != pCurrent->m_pNext[i] && m_aKeyCompare(pCurrent->m_pNext[i]->get_key(), key)) {
				pCurrent = pCurrent->m_pNext[i];
			}
			fnOp(i, pCurrent);
		}

		assert(nullptr != pCurrent);
		pCurrent = pCurrent->next();

		return pCurrent;
	}

	CRange<Node*> find_key(const key_t &key, bool bEqual) const
	{
		auto pCurrent = find_pos_not_less(key, [](int nLevel, Node *pNode) {});
		return (nullptr != pCurrent && (bEqual ? pCurrent->get_key() == key : true)) ? CRange<Node*>(pCurrent, pCurrent->next()) : CRange<Node*>(nullptr, nullptr);
	}

	CRange<Node*> find_range(const key_t &kFrom, const key_t &kTo) const
	{
		auto rBegin = find_key(kFrom, false);

		auto pEnd = rBegin.begin();
		while (nullptr != pEnd && m_aKeyCompare(pEnd->get_key(), kTo)) {
			pEnd = pEnd->next();
		}

		return CRange<Node*>(rBegin.begin(), pEnd);
	}

private:
	int               m_nCurrentLevel;
	Node              *m_pHead;
	Node*             m_arrSearchPath[nMaxLevel]; 
	size_t            m_nCount;
	key_compare_t     m_aKeyCompare;
	level_creator_t   m_aLevelCreator;
	allocator         m_aAllocator;
	ptr_allocator     m_aPtrAllocator;
};