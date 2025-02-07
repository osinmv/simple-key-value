install: keyvalue.c
	gcc -fPIC -c keyvalue.c -o keyvalue.o
	gcc -shared -o libkeyvalue.so keyvalue.o

clean:
	rm keyvalue.o keyvalue_test.o


test: libkeyvalue.so *_test.c
	 gcc -fsanitize=address -o test keyvalue_test.c -L. -lkeyvalue && ./test