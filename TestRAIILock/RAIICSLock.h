#pragma once

// https://jrdodds.blogs.com/blog/2004/08/raii_in_c.html/
// written by Jonathan Dodds

#include "CriticalSection.h"

class RAIICSLock
{
public:
	RAIICSLock(CriticalSection& a_section)
		: m_Section(a_section) {
		m_Section.Lock();
	}
	~RAIICSLock()
	{
		m_Section.Unlock();
	}

private:
	// copy ops are private to prevent copying
	RAIICSLock(const RAIICSLock&);
	RAIICSLock& operator=(const RAIICSLock&);

	CriticalSection& m_Section;
};

