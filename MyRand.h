#pragma once
#include <cmath>
extern inline int randposneg() {
	if (rand() % 2)
		return 1;
	else
		return -1;
}

extern inline int randint(int x, int y) {
	return rand() % (y - x + 1) + x;
}

extern inline double randfloat() {
	return rand() / (double)RAND_MAX;
}
