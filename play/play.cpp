#include <iostream>
#include <set>

extern "C" struct A {
	int a;
	int b;
	int c;
};

struct IPoly {
	IPoly() = default;
	virtual ~IPoly() = default;

	virtual void puly()
	{
		poly3 = poly + poly2;
	}

	int poly;
	int poly2;
	int poly3;
};

struct IPoly2 {
	IPoly2() = default;
	virtual ~IPoly2() = default;

	virtual void pouly()
	{
		poly6 = poly4 + poly5;
	}
	int poly4;
	int poly5;
	int poly6;
};

struct B : public A, public IPoly, public IPoly2 {
	int d;
	int e;
	int f;

	B() = default;
	virtual ~B() = default;

	virtual void puly() override
	{
		e = poly + poly2 + poly3;
	}
	virtual void pouly() override
	{
		f = poly4 + poly5 + poly6;
	}
};

void tst_cstruct()
{
	B b;
	A &a = b;
	B *c = static_cast<B *>(&a);
	A *aa = nullptr;
	B *bb = static_cast<B *>(aa);
	std::cout << "First off, moo, and then? " << bb << " is null? "
		  << (bb == nullptr) << std::endl;
	std::set<A *> set;
	std::cout << "\nSet contains ? " << (set.find(&b) != set.cend());
	set.insert(&b);
	set.insert(aa);
	set.insert(&b);
	set.insert(&b);
	set.insert(&b);
	set.insert(&b);
	set.insert(&b);
	set.insert(&b);
	std::cout << "\nSet now contains ? " << (set.find(&b) != set.cend())
		  << std::endl;
	b.a = 68;
	b.b = 93;
	b.c = 18449;
	b.d = 47338;
	b.e = 603826;
	b.f = 602862;
	std::cout << "\na: " << b.a << "\nb: " << b.b << "\nc: " << b.c
		  << "\nd: " << b.d << "\ne: " << b.e << "\nf: " << b.f
		  << std::endl;
	a.a = 359472;
	a.b = 279937;
	a.c = 9;
	std::cout << "\na: " << b.a << "\nb: " << b.b << "\nc: " << b.c
		  << "\nd: " << b.d << "\ne: " << b.e << "\nf: " << b.f
		  << std::endl;
	std::cout << "\nb: " << &b << "\na: " << &a << "\nc: " << c
		  << std::endl;
	std::cout << "\nTroothify: &b == c ? " << (&b == c ? "true" : "false")
		  << std::endl;
	std::cout << "\nTroothify: &b != &a ? " << (&b == &a ? "true" : "false")
		  << std::endl;
	int abc = 9;
	int def = 99;
	int &dee = abc;
	int &doo = def;
	*&doo = dee;
	std::cout << "\ndee: " << &dee << "\ndoo: " << &doo << "\ndef: " << def
		  << std::endl;
}

#define SHELPER(s) #s
#define STR(s) SHELPER(s)
void tst_chars()
{
	typedef struct {
		const char *str;
		const char c;
	} mpair;
#define PAIR(value) { STR(value), value }
	mpair chars[] = { PAIR('\e') };
	for (size_t i = 0; i < sizeof(chars) / sizeof(chars[0]); i++) {
		std::cout << chars[i].str << " = " << ((int)chars[i].c)
			  << std::endl;
	}
}

int main()
{
	tst_chars();
	return 0;
}