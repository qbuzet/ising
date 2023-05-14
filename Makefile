O_FILES = main.o
PROG = app
OUT = M E
OUT_FILES = out.txt out_B.txt

run: app
	./app

app: main.o
	gcc main.o -lm -o app

main.o: main.c
	gcc -Wall -c main.c

clean:
	rm $(O_FILES) $(PROG)
	rm -r $(OUT) $(OUT_FILES)
	mkdir $(OUT)

figures:
	gnuplot 'E(T).gpi';
	gnuplot 'M(T).gpi';
	gnuplot 'Cv(T).gpi';
	gnuplot 'Chi(T).gpi';

tcri:
	gnuplot 'Tcri.gpi';

mb:
	gnuplot 'M(B).gpi'