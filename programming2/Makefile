MPICC = mpicc
CFLAGS = -g
MPIRUN = mpirun
EXECS = warehouse
NUM_PROCESS = 20
HOSTFILE = ./testfiles/Graduates/testfile2/np20_servers4.txt

TEST1 = ./testfiles/Graduates/testfile1/np90_servers20.txt
TEST2 = ./testfiles/Graduates/testfile2/np20_servers4.txt
TEST3 = ./testfiles/Graduates/testfile3/np90_servers20.txt

all: $(EXECS)

warehouse: server.o client.o main.c send_receive.c
	$(MPICC) $(CFLAGS) -o $@ $^ -lm

server.o: server.c
	$(MPICC) $(CFLAGS) -c $< -lm

client.o: client.c
	$(MPICC) $(CFLAGS) -c $< -lm

test1:
	#$(MPIRUN) -np $(NUM_PROCESS) --hostfile $(HOSTFILE) $(EXECS) $(NUMSERVERS)
	$(MPIRUN) --mca orte_base_help_aggregate 0 --use-hwthread-cpus -np 90 ./$(EXECS) $(TEST1) 20 > err 2>&1 > out

test2:
	$(MPIRUN) --mca orte_base_help_aggregate 0 --use-hwthread-cpus -np 20 ./$(EXECS) $(TEST2) 4 > err 2>&1 > out

test3:
	$(MPIRUN) --mca orte_base_help_aggregate 0 --use-hwthread-cpus -np 90 ./$(EXECS) $(TEST3) 20 > err 2>&1 > out

clean:
	rm $(EXECS) out err *.o
