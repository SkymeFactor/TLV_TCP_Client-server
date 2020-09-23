#pragma once

#include <iostream>
#include <cstring>
#include <chrono>
#include <functional>


#ifdef _WIN32
	#pragma comment(lib, "Ws2_32.lib")
	
	#include <WinSock2.h>
	#include <io.h>
	#include <WS2tcpip.h>
	#define PROTOCOL 6
#endif
#ifdef __linux__
	#include <sys/socket.h>
	#include <arpa/inet.h>
	#include <unistd.h>
	#define PROTOCOL 0
#endif

#define PORT 8080
#define SA struct sockaddr

namespace Client {
    /**************************************************************************
     * Class declaration of Client class.
     *************************************************************************/

    class Client {
    private:
        int client_socket;
        struct sockaddr_in serv_addr;
        bool isConnected;

        void errorCheck(int, const char *);

    public:
        Client () {};
        ~Client();

        void setupTcpConnection (const int);
        void connectToIP (std::function<void (int)>, const char *);
    };

    /**************************************************************************
     * Methods definition of the Client class.
     *************************************************************************/

    void Client::errorCheck(int exp, const char *what) {
        // Form and give the error if necessary
        if (exp < 0) {
            time_t now = time(0);
            char buffer[20];
            strftime(buffer, 20, "%Y-%m-%d %H:%M:%S", localtime(&now));
            std::cerr << buffer << " ERROR: " << what << "\n";
            #ifdef _WIN32
                std::getchar();
            #endif
            exit(EXIT_FAILURE);
        }
    }

    Client::~Client () {
        if (this->isConnected) {
            #ifdef _WIN32
                closesocket(this->client_socket);
                WSACleanup();
                std::getchar();
            #elif __linux__
                close(this->client_socket);
            #endif
        }
    }

    void Client::setupTcpConnection (const int port = PORT) {
        // Initialize WinSock
        #ifdef _WIN32
            WSADATA wsaData = {0};
            errorCheck(WSAStartup(MAKEWORD(2, 2), &wsaData),
                "WSAStartup() failed.");
	    #endif

        // Create socket
        errorCheck(this->client_socket = socket(AF_INET, SOCK_STREAM, PROTOCOL), 
            "Unable to create the socket.");
        
        this->serv_addr.sin_family = AF_INET;
	    this->serv_addr.sin_port = htons(port);
    }

    void Client::connectToIP (std::function<void (int)> func, const char *ip_addr = "127.0.0.1") {

        // Convert IPv4 and IPv6 addresses from text to binary form
        errorCheck( (inet_pton(AF_INET, ip_addr, &serv_addr.sin_addr) - 1),
            "Invalid address / Address not supported.");
        
        // Establish connection to the server
	    errorCheck(connect(this->client_socket, (SA*)&serv_addr, sizeof(serv_addr)),
            "Connection failed.");
        
        func(this->client_socket);
	}

}