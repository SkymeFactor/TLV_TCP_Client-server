#include <iostream>
#include <string.h>
#include "BER.h"
#include "Client.h"


#define MAX_SIZE 10


char *_message;


void client_workload (int socket) {
    char *message = _message;
    easyBER::BERmsg<char> msg(message, std::strlen(message));
    char *buffer;

    // Send some message to server
	send(socket, msg.translateToCString(), strlen(msg.translateToCString()), 0 );
	std::cout << "Greetings has been sent.\n";

    // Read the header
    char *header = new char[128];
    recv(socket, header, 128, MSG_PEEK);

    // Set the actual array size
    size_t total_length = easyBER::BERmsg<char>::getTotalLengthFromHeader(header);
    buffer = new char[total_length];
    
    recv(socket, buffer, total_length, 0);
    easyBER::BERmsg<char> received_message(buffer);

    delete buffer;

	std::cout << "Incoming message :> "<< received_message.getMessage() << "\n";
}


int main(int argc, const char *argv[])
{
    Client::Client client;

    if ( argv[2] != nullptr ) {
        _message = const_cast<char *>(argv[2]);
    } else {
        _message = new char[18];
        std::strcpy(_message, "Hello from client.");
    }

    client.setupTcpConnection(18000);

    std::cout << "Connection established.\n";

    if ( argv[1] != nullptr )
        client.connectToIP(client_workload, argv[1]);
    else
        client.connectToIP(client_workload);

	return 0;
}