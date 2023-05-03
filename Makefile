O_FILES = main.o
PROG = app
OUT = out

run: app
	./app

app: main.o
	gcc main.o -lm -o app

main.o: main.c
	gcc -Wall -c main.c

clean:
	rm $(O_FILES) $(PROG)
	rm -r $(OUT)

figures:
	gnuplot 'E(T).gpi';
	gnuplot 'M(T).gpi';
	gnuplot 'Cv(T).gpi';
	gnuplot 'Chi(T).gpi';