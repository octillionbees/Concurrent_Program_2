OBJFILES = main.o qsort.o

main:
	gcc main.c -o main
qsort:
	gcc qsort.c -o qsort
merge:
	gcc merge.c -o merge

all:
	gcc main.c -o main
	gcc qsort.c -o qsort
clean:
	-rm -f ${OBJFILES} main qsort merge