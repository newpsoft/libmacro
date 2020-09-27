#include <chrono>
#include <iostream>
#include <functional>
#include <map>
#include <vector>

#include <cstdlib>
#include <ctime>

extern "C" struct IntInt {
	int first;
	int second;
};

struct int_int_less : std::binary_function<IntInt, IntInt, bool>
{
	bool operator()(const IntInt &s1, const IntInt &s2) const
	{
		return s1.first < s2.first;
	}
};

class H {
public:
	virtual void hierarchy() = 0;
};

class I {
public:
	virtual void boo() = 0;
};

class N {
public:
	virtual void doo() = 0;
};

class Stance : public I, public N, public H {
public:
	int i;
	virtual void boo() override {
		i++;
	}
	virtual void doo() override {
		i--;
	}
	virtual void hierarchy() override
	{
		++i;
	}
};

extern "C" struct deref;
extern "C" typedef void (*doo_fn)(struct deref *selfPt);

extern "C" struct deref {
	struct deref *vp;
	doo_fn doo;
};

const int COUNT = 5000;

using namespace std;

static int intCompare(const void *lhs, const void *rhs)
{
	const int *lInt = (const int *)lhs, *rInt = (const int *)rhs;
	if (rhs) {
		if (lhs) {
			return *lInt < *rInt ? -1 : *lInt > *rInt;
		}
		return -1;
	}
	return 0;
}

int dokidoki = 0;
static void dooAgain(struct deref *selfPt)
{
	if (selfPt) {
		if (selfPt->doo) {
			selfPt->doo(selfPt->vp);
		} else {
			++dokidoki;
		}
	} else {
		--dokidoki;
	}
}

int main()
{
	map<int, int> m;
	vector<IntInt> vect;
	IntInt finder, *arr;
	int i, f;
	chrono::high_resolution_clock::time_point t1a, t1b, t2a, t2b;
	chrono::duration<double, std::milli> diffa, diffb;
	Stance stance;
//	I *iPt = &stance;
//	N *nPt = &stance;
	H *hPt = &stance;
	deref de1, de2, de3, de4;
	de1.vp = nullptr;
	de1.doo = nullptr;
	de2.doo = de3.doo = de4.doo = dooAgain;
	de2.vp = &de1;
	de3.vp = &de2;
	de4.vp = &de3;
	srand(time(nullptr));

	t1a = chrono::high_resolution_clock::now();
	for (i = 0; i < COUNT; i++) {
		m[i] = rand() % 500 + 1;
		vect.push_back({i, rand() % 500 + 1});
	}
	arr = &vect.front();
	t1b = t2a = chrono::high_resolution_clock::now();
	finder.first = 0;
	for (i = 0; i < COUNT; i++) {
		finder.first++;
	}
	t2b = chrono::high_resolution_clock::now();
	diffa = t2a - t1a;
	diffb = t2b - t2b;
	cout << "Assign loop took: " << diffa.count() << endl;
	cout << "NoOp loop took: " << diffb.count() << endl << endl;

	t1a = chrono::high_resolution_clock::now();
	for (i = 0; i < COUNT; i++) {
		f = m.find(i)->second;
	}
	t1b = t2a = chrono::high_resolution_clock::now();
	for (i = 0; i < COUNT; i++) {
		f = *(int *)bsearch(&i, arr, COUNT, sizeof(IntInt), intCompare);
	}
	t2b = chrono::high_resolution_clock::now();

	diffa = t2a - t1a;
	diffb = t2b - t2a;
	cout << "C++ find: " << diffa.count() << "\nC find: " << diffb.count() << endl << endl;

	t1a = chrono::high_resolution_clock::now();
	for (i = 0; i < COUNT; i++) {
		finder.first = i;
		f = lower_bound(vect.begin(), vect.end(), finder, int_int_less())->second;
	}
	t2a = chrono::high_resolution_clock::now();

	diffa = t2a - t1a;
	cout << "Hybrid: " << diffa.count() << endl << endl;

	t1a = chrono::high_resolution_clock::now();
	for (i = 0; i < COUNT; i++) {
		hPt->hierarchy();
//		iPt->boo();
//		nPt->doo();
	}
	t1b = t2a = chrono::high_resolution_clock::now();
	for (i = 0; i < COUNT; i++) {
		dooAgain(&de4);
	}
	t2b = chrono::high_resolution_clock::now();

	diffa = t2a - t1a;
	diffb = t2b - t2a;
	cout << "C++ call: " << diffa.count() << "\nC call: " << diffb.count() << endl << endl;

	return 0;
}
