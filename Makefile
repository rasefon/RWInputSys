Target = test.out
Objects = rw_io.o

$(Target): $(Objects)
	gcc -g -o $(Target) $(Objects)

rw_io.o: common.h rw_io.h
	gcc -g -c rw_io.c 

clean:
	rm -rf *.o *.exe *.out
