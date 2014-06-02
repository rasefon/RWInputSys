Target = test.out
Objects = rw_io.o
flags = -g -c -std=c99

$(Target): $(Objects)
	gcc -g -o $(Target) $(Objects)

rw_io.o: common.h rw_io.h
	gcc $(flags) rw_io.c 

clean:
	rm -rf *.o *.exe *.out
