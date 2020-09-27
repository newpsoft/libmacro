#include <stdio.h>
#include <stdlib.h>
#include <time.h>

struct IntInt {
	int first;
	int second;
};

struct deref;
typedef void (*doo_fn)(struct deref *selfPt);

struct deref {
	struct deref *vp;
	doo_fn doo;
};

const int COUNT = 5000;

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
	int i;
	struct timespec t1, t2, diff;
	struct deref de1, de2, de3, de4;
	de1.vp = NULL;
	de1.doo = NULL;
	de2.doo = de3.doo = de4.doo = dooAgain;
	de2.vp = &de1;
	de3.vp = &de2;
	de4.vp = &de3;

	timespec_get(&t1, TIME_UTC);
	for (i = 0; i < COUNT; i++) {
		dooAgain(&de2);
	}
	timespec_get(&t2, TIME_UTC);
	diff.tv_sec = t2.tv_sec - t1.tv_sec;
	diff.tv_nsec = t2.tv_nsec - t1.tv_nsec;
	printf("dooAgain: %lld:%ld or %lf\n", diff.tv_sec, diff.tv_nsec, (double)diff.tv_nsec / (double)1000000);
	return 0;
}
