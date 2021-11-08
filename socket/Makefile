ser: simpleServer.o warp.o
	gcc simpleServer.o warp.o -o server.out
	./server.out
	@echo "cleaning project"
	-rm *.o
	@echo "clean ok"
cli:

simpleServer.o:
	gcc -c server/simpleServer.c
warp.o:
	gcc -c warp.c


