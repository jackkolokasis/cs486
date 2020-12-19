MPICC = mpicc
CFLAGS = -g
MPIRUN = mpirun
EXECS = warehouse
NUM_PROCESS = 4
HOSTFILE = ./testfiles/Graduates/testfile2/np20_servers4.txt
IN_FILE = ./testfiles/Graduates/testfile2/np20_servers4.txt

all: $(EXECS)

warehouse: main.c
	$(MPICC) $(CFLAGS) -o $@ $<

run:
	#$(MPIRUN) -np $(NUM_PROCESS) --hostfile $(HOSTFILE) $(EXECS) $(NUMSERVERS)
	$(MPIRUN) -np $(NUM_PROCESS) ./$(EXECS) $(IN_FILE) > err 2>&1 > out

clean:
	rm $(EXECS)
