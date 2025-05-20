clean:
	rm keyvalue.o keyvalue_test.o test

test: keyvalue.h *_test.c
	gcc -g -O0 -fsanitize=address keyvalue_test.c -o test && ./test
