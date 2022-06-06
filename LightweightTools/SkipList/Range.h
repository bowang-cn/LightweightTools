#pragma once

// range, [begin(),end()) is valid while begin() != end()
template<class Type>
class CRange
{
public:
	typedef Type type_t;

	CRange() :m_aEnd(m_aBegin) {}
	CRange(type_t aBegin, type_t aEnd) :m_aBegin(aBegin), m_aEnd(aEnd) {}

	template<class U>
	CRange(CRange<U> r) : m_aBegin(r.begin()), m_aEnd(r.end()) {}
	template<class U>
	CRange& operator= (CRange<U> r)
	{
		m_aBegin = type_t(r.begin());
		m_aEnd = type_t(r.end());
		return *this;
	}

	type_t begin() { return m_aBegin; }
	type_t end() { return m_aEnd; }

private:
	type_t m_aBegin;
	type_t m_aEnd;
};