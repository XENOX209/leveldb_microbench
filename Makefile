#Makefile

bench: process.o mybench.o
	g++ -o bench process.o mybench.o /usr/local/lib/libleveldb.a  -lpthread -I /usr/local/include/leveldb/
process: process.cc
	g++ -c process.cc
mybench: mybench.cc
	g++ -c mybench.cc /usr/local/lib/libleveldb.a  -lpthread -I /usr/local/include/leveldb/
.PHONY: clean
clean:
	rm process.o mybench.o bench
