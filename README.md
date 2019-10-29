# tctest

`tctest` is a simple 1 header unit testing framework for C/C++.

Simply include the header in a standard C file with your unit tests (matching this function signature: `void test_foo(void)`).

Use `TCTEST_CHECK(condition, message);` to check a `condition` that is believed to be `true`. The `message` is a C string which describes the test.

Compile the C file like this `cc -o demo demo.c -Wall -Werror -ldl -lelf -rdynamic` include any other C files you wish to test.

## Example

Drop `tctest.h` into your project, write some tests, compile it, and run it.

**demo.c**

```lang=C
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
```

**Makefile**

```
demo: tctest.h demo.c
	gcc -o demo demo.c -Wall -Werror -ldl -lelf -rdynamic

clean:
	rm -f demo
```

Execution:

```
$ make
gcc -o demo demo.c -Wall -Werror -ldl -lelf -rdynamic
$ ./demo 
PASS: basic multiplication works
FAIL: basic modulus works
PASS: basic subtraction works
PASS: basic division works
PASS: basic addition works
DONE: 5 Assertions, 4 Passed, 1 Failed

```

## Caveats

This likely only works on Linux, but it may work on other systems that support dynamic loading (`libdl`), ELF binary format (`libelf`), and have a `/proc` file system laid out with `/proc/${pid}/exe` linking to the executable with the given `pid`.
