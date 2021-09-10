#include <iostream>
#include <string.h>
#include "BER.h"
#include "Server.h"


#define MAX_SIZE 1024


void server_workload (int socket) {
    char *buffer;
    char *str = new char[37];
    std::strcpy(str, "HTTP/1.0 200 OK\r\n\r\nHello from server!");
    char *message = easyBER::BERmsg<char>(str, strlen(str)).translateToCString();
    
    // Read the header
    char *header = new char[128];
    recv(socket, header, 128, MSG_PEEK);

    // Set the actual array size
    size_t total_length = easyBER::BERmsg<char>::getTotalLengthFromHeader(header);
    buffer = new char[total_length];
    
    recv(socket, buffer, total_length, 0);
    easyBER::BERmsg<char> received_message(buffer);

    delete buffer;

    std::cout << "Incoming message :> " << received_message.getMessage() << "\n";
    send(socket, message, strlen(message), 0);
    std::cout << "Replying greetings has been sent.\n";
}

int main(int argc, char const *argv[])
{
    Server::Server server;
    
    server.setupTcpConnection(18000);

    std::cout << "Server started\n";

    server.handleConnection(server_workload);

    return 0;
}