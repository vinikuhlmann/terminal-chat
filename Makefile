all:
	g++ client.cpp -o client -pthread -std=c++11
	g++ server.cpp -o server -pthread -std=c++11

server:
	./server 0.0.0.0 54000

client:
	./client 0.0.0.0 54000