#include "tctest.h"

void test_add(void) {
	TCTEST_CHECK(2+2 == 4, "basic addition works");
}

void test_sub(void) {
	TCTEST_CHECK(2-2 == 0, "basic subtraction works");
}

void test_mul(void) {
	TCTEST_CHECK(2*2 == 4, "basic multiplication works");
}

void test_div(void) {
	TCTEST_CHECK(2/2 == 1, "basic division works");
}

void test_mod(void) {
	/* voidentionally wrong to demonstrate failure */
	TCTEST_CHECK(2%2 == 1, "basic modulus works");
}
