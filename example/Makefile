# parameter for generating example.o
CFLAGS=-I..

example: example.o ../common.o
# $@ = example (the output variable)
# $^ = example.o ../common.o (the input variable)
	$(CC) -o $@ $^

conf: conf.o ../common.o
# -l for referencing libconfig
	$(CC) -o $@ $^ -lconfig

.PHONY: clean

clean:
	rm -f *.o example conf
