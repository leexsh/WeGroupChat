CC = g++
CFLAGS= -std=c++11
all: MainClient.cpp serverMain.cpp Server.o Client.o
	$(CC) $(CFLAGS) serverMain.cpp	Server.o -o room_server
	$(CC) $(CFLAGS) MainClient.cpp	Client.o -o room_client
Server.o: Server.cpp Server.h Common.h
	$(CC) $(CFLAGS) -c Server.cpp

Client.o: Client.cpp Client.h Common.h
	$(CC) $(CFLAGS) -c Client.cpp

clean:
	rm -f *.o