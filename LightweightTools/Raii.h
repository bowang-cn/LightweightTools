#pragma once
#include "NonCopyable.h"

template<class init, class uninit>
class CRaii : private NonCopyable, private NonMoveable
{
	using init_t = init;
	using uninit_t = uninit;
public:
	CRaii(init_t i, uninit_t u) :m_bDismiss(false), m_init(i), m_uninit(u) { m_init(); }
	~CRaii() { if (!m_bDismiss) m_uninit(); }

	void Dismiss() { m_bDismiss = true; }

protected:
	bool m_bDismiss;
	init_t m_init;
	uninit_t m_uninit;
};