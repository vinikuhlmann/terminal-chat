all:
	g++ socket.cpp -o socket -pthread -std=c++11

server:
	./socket server 0.0.0.0 54000

client:
	./socket client 0.0.0.0 54000