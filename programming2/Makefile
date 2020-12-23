MPICC = mpicc
CFLAGS = -g
MPIRUN = mpirun
EXECS = warehouse
NUM_PROCESS = 5
HOSTFILE = ./testfiles/Graduates/testfile2/np20_servers4.txt
IN_FILE = ./testfiles/Graduates/testfile2/np20_servers4.txt

all: $(EXECS)

warehouse: server.o main.c
	$(MPICC) $(CFLAGS) -o $@ $^ -lm

server.o: server.c
	$(MPICC) $(CFLAGS) -c $< -lm

run:
	#$(MPIRUN) -np $(NUM_PROCESS) --hostfile $(HOSTFILE) $(EXECS) $(NUMSERVERS)
	$(MPIRUN) -np $(NUM_PROCESS) ./$(EXECS) $(IN_FILE) 4 > err 2>&1 > out

clean:
	rm $(EXECS) out err
