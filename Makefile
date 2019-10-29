demo: tctest.h demo.c
	gcc -o demo demo.c -Wall -Werror -ldl -lelf -rdynamic

clean:
	rm -f demo
