ser: simpleServer.o warp.o
	gcc simpleServer.o warp.o -o server.out
	@echo "cleaning project"
	-rm *.o
	@echo "clean ok"
	./server.out
select: selectServer.o warp.o
	gcc selectServer.o warp.o -o server.out
	@echo "cleaning project"
	-rm *.o
	@echo "clean ok"
	./server.out
epoll: epollServer.o warp.o
	gcc epollServer.o warp.o -o server.out
	@echo "cleaning project"
	-rm *.o
	@echo "clean ok"
	./server.out
cli: simpleClient.o warp.o
	gcc simpleClient.o warp.o -o client.out
	@echo "cleaning project"
	-rm *.o
	@echo "clean ok"
	./client.out
selectServer.o:
	gcc -c server/selectServer.c
epollServer.o:
	gcc -c server/epollServer.c
simpleServer.o:
	gcc -c server/simpleServer.c
warp.o:
	gcc -c warp.c
simpleClient.o:
	gcc -c client/simpleClient.c



