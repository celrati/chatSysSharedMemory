CC=gcc
OPTION= -Wall -g
LDLIBS = -lrt -pthread

all: run1 run2 run3 interface

run1: test1.o  ../lib/bit.o ../lib/helper.o ../msg_file.o ../data/data_handler.o ../lib/subs.o
	$(CC) $(OPTION) -o run1 test1.o ../lib/bit.o ../lib/helper.o ../msg_file.o ../data/data_handler.o ../lib/subs.o $(LDLIBS)

interface: interface.o  ../lib/bit.o ../lib/helper.o ../msg_file.o ../data/data_handler.o ../lib/subs.o
	$(CC) $(OPTION) -o interface interface.o ../lib/bit.o ../lib/helper.o ../msg_file.o ../data/data_handler.o ../lib/subs.o $(LDLIBS)

run2: test2.o  ../lib/bit.o ../lib/helper.o ../msg_file.o ../data/data_handler.o ../lib/subs.o
	$(CC) $(OPTION) -o run2 test2.o ../lib/bit.o ../lib/helper.o ../msg_file.o ../data/data_handler.o ../lib/subs.o $(LDLIBS)

run3: test3.o  ../lib/bit.o ../lib/helper.o ../msg_file.o ../data/data_handler.o ../lib/subs.o 
	$(CC) $(OPTION) -o run3 test3.o ../lib/bit.o ../lib/helper.o ../msg_file.o ../data/data_handler.o ../lib/subs.o $(LDLIBS)

test1.o: test1.c ../lib/subs.h
	$(CC) $(OPTION) -o test1.o -c test1.c $(LDLIBS) #to write to shared memory

interface.o: interface.c ../lib/subs.h
	$(CC) $(OPTION) -o interface.o -c interface.c $(LDLIBS) #to use an interface base line command

test2.o: test2.c ../lib/subs.h
	$(CC) $(OPTION) -o test2.o -c test2.c $(LDLIBS) #to read to shared memory

test3.o: test3.c ../lib/subs.h # to test the anonym file
	$(CC) $(OPTION) -o test3.o -c test3.c $(LDLIBS) #to read to shared memory

../lib/subs.o: ../lib/subs.c ../data/data_handler.h
	$(CC) $(OPTION) -o ../lib/subs.o -c ../lib/subs.c $(LDLIBS)	

../data/data_handler.o: ../data/data_handler.c ../data/data_handler.h
	$(CC) $(OPTION) -o ../data/data_handler.o -c ../data/data_handler.c $(LDLIBS)

../lib/bit.o: ../lib/bit.h ../lib/bit.c 
	$(CC) $(OPTION) -o ../lib/bit.o -c ../lib/bit.c $(LDLIBS)

../helper.o: ../lib/helper.h ../lib/helper.c ../lib/bit.h ../msg_file.h
	$(CC) $(OPTION) -o ../lib/helper.o -c ../lib/helper.c $(LDLIBS)

../msg_file.o: ../lib/helper.h  ../lib/bit.h ../msg_file.h ../msg_file.c ../lib/subs.h
	$(CC) $(OPTION) -o ../msg_file.o -c ../msg_file.c $(LDLIBS)

clean: 
	rm  -r ../*o ../data/*.o ../lib/*.o *.o run1 run2 run3 interface