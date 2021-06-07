all:
	g++ socket.cpp -o socket -pthread -std=c++11

server:
	./socket 0.0.0.0 54000

client:
	./socket 0.0.0.0 54000