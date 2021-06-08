all:
	g++ client.cpp -o client -pthread -std=c++11
	g++ server.cpp -o server -pthread -std=c++11

newserver:
	./server 0.0.0.0 54000

newclient:
	./client 0.0.0.0 54000