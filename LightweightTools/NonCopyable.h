#pragma once

class CNonCopyable
{
public:
	CNonCopyable(const CNonCopyable&) = delete;
	CNonCopyable& operator= (const CNonCopyable&) = delete;
	CNonCopyable() = default;
};

class CNonMoveable
{
public:
	CNonMoveable() = default;
	CNonMoveable(CNonMoveable&&) = delete;
	CNonMoveable& operator= (CNonMoveable&&) = delete;
};