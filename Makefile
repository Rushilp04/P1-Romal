all: mymal

mymal: memtest.o libmylib.a #libmylib.a is the dependency for the executable
	gcc -o mymal memtest.o -L. -lmylib -lm

memtest.o: memtest.c 
	gcc -o memtest.o -c memtest.c 

mymalloc.o: mymalloc.c 
	gcc -o mymalloc.o -c -shared mymalloc.c


libmylib.a: mymalloc.o 
	ar rcs libmylib.a mymalloc.o

libs: libmylib.a

clean:
	rm -f mymal *.o *.a *.gch #This way is cleaner than your clean