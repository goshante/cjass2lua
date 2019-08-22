#pragma once

template <class T>
class Singleton
{
private:
	T* ___pInst;
	Singleton() : ___pInst(nullptr) {}
	Singleton(Singleton const&) = delete;
	void operator=(Singleton const&) = delete;

public:
	static Singleton& Get()
	{
		if (!___pInst)
			___pInst = new T;
		return (*___pInst);
	}

	virtual ~Singleton() { if (!___pInst) delete ___pInst; }
};