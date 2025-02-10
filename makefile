install: keyvalue.c
	gcc -c keyvalue.c -o keyvalue.o

clean:
	rm keyvalue.o keyvalue_test.o test


test: keyvalue.o *_test.c
	gcc -g -O0 -fsanitize=address keyvalue.c keyvalue_test.c -o test && ./test
