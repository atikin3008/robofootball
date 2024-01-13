#include "CppSockets/sockets/TcpServerSocket.hpp"
#include<iostream>
#include<future>
#include<thread>

void Socket(){
    TcpServerSocket server("localhost", 5004);
    server.acceptConnection();
    char x[100];
    server.receiveData(x, 100);
    std::cout << x;
}


int main() {
    std::async(Socket);
    std::cout<<"2";
}
