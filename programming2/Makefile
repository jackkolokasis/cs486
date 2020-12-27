MPICC = mpicc
CFLAGS = -g
MPIRUN = mpirun
EXECS = warehouse
NUM_PROCESS = 20
HOSTFILE = ./testfiles/Graduates/testfile2/np20_servers4.txt
IN_FILE = ./testfiles/Graduates/testfile2/np20_servers4.txt

all: $(EXECS)

warehouse: server.o client.o main.c send_receive.c
	$(MPICC) $(CFLAGS) -o $@ $^ -lm

server.o: server.c
	$(MPICC) $(CFLAGS) -c $< -lm

client.o: client.c
	$(MPICC) $(CFLAGS) -c $< -lm

run:
	#$(MPIRUN) -np $(NUM_PROCESS) --hostfile $(HOSTFILE) $(EXECS) $(NUMSERVERS)
	$(MPIRUN) --mca orte_base_help_aggregate 0 --use-hwthread-cpus -np $(NUM_PROCESS) ./$(EXECS) $(IN_FILE) 4 > err 2>&1 > out

clean:
	rm $(EXECS) out err *.o
