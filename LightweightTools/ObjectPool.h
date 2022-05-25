#pragma once
#include <type_traits>
#include <vector>
#include "NonCopyable.h"

template<class T, size_t nSize = 16>
class CObjectPool : private CNonCopyable
{
	struct SNode
	{
		SNode *m_pNext;
		SNode() :m_pNext(nullptr) {}
	};

	typedef typename std::conditional<sizeof(T) < sizeof(SNode), SNode, T>::type obj_type;

public:
	CObjectPool() :m_pFree(nullptr) { InitPool(); }
	~CObjectPool() { ReleasePool(); }

	template<class ...Args>
	T* Create(Args &&...args)
	{
		return CreateObjcet(std::forward<Args>(args)...);
	}

	template<class Base, typename std::enable_if<std::is_base_of<Base, T>::value, int>::type = 0>
	void Release(Base *pBase)
	{
		ReleaseObject((T*)pBase);
	}

	void ReleaseAll()
	{
		ReleasePool();
	}

private:
	void InitPool()
	{
		AddBlock(nSize);
	}

	void ReleasePool()
	{
		for (size_t i = 0; i < m_vecBlocks.size(); i++)
		{
			ReleaseBlock(m_vecBlocks[i]);
		}
	}

	unsigned char* CreateBlock(size_t nCapacity) const
	{
		return (unsigned char*)malloc(sizeof(obj_type) * nCapacity);
	}

	void ReleaseBlock(unsigned char *pBlock) const
	{
		free(pBlock);
	}

	SNode* CreateNodeChain(unsigned char *pBlock, size_t nCapacity) const
	{
		SNode *pNode = nullptr;
		if (nullptr != pBlock)
		{
			pNode = (SNode*)pBlock;
			SNode *pNodeNext = nullptr;
			for (size_t i = 0; i < nCapacity; i++)
			{
				pNode->m_pNext = pNodeNext;
				pNodeNext = pNode;
				pNode = (SNode*)((unsigned char*)pNode + sizeof(obj_type));
			}
			pNode = pNodeNext;
		}

		return pNode;
	}

	unsigned char* GetBuffer()
	{
		unsigned char *pBuffer = nullptr;

		if (nullptr == m_pFree)
		{
			ExtendPool();
		}

		if (nullptr != m_pFree)
		{
			pBuffer = (unsigned char*)m_pFree;
			m_pFree = m_pFree->m_pNext;
		}

		return pBuffer;
	}

	void FreeBuffer(unsigned char *pBuffer)
	{
		if (nullptr != pBuffer)
		{
			SNode *pNode = (SNode*)pBuffer;
			pNode->m_pNext = m_pFree;
			m_pFree = pNode;
		}
	}

	template<class ...Args>
	T* CreateObjcet(Args &&...args)
	{
		T *pObject = nullptr;

		unsigned char *pBuffer = GetBuffer();
		if (nullptr != pBuffer)
		{
			try
			{
				pObject = new(pBuffer) T(std::forward<Args>(args)...);
			}
			catch (...)
			{
				FreeBuffer(pBuffer);
				pObject = nullptr;
			}
		}

		return pObject;
	}

	void ReleaseObject(T *pObject)
	{
		if (nullptr != pObject)
		{
			pObject->~T();
			FreeBuffer((unsigned char*)pObject);
		}
	}

	void AddBlock(size_t nCapacity)
	{
		if (0 == nCapacity)
		{
			nCapacity = 4;
		}

		unsigned char *pBlock = CreateBlock(nCapacity);
		if (nullptr != pBlock)
		{
			try
			{
				m_vecBlocks.push_back(pBlock);
				m_pFree = CreateNodeChain(pBlock, nCapacity);
			}
			catch (...)
			{
				ReleaseBlock(pBlock);
			}
		}
	}

	void ExtendPool()
	{
		AddBlock(nSize < 10 ? 5 : nSize / 2);
	}

private:
	SNode *m_pFree;
	std::vector<unsigned char*> m_vecBlocks;
};