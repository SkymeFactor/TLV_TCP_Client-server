#pragma once

#include <iostream>
#include <cstring>
#include <chrono>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <vector>
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
	#include <netinet/in.h>
	#include <unistd.h>
	#define PROTOCOL 0
#endif

#define PORT 8080
#define SA struct sockaddr

namespace Server {
    /**************************************************************************
     * Class declaration of Server class.
     *************************************************************************/

    class Server {
    private:
        int server_socket;
        const int option_set = 1;
        int addrlen = sizeof(this->address);
        struct sockaddr_in address;
        bool isConnected = false;

        std::queue<int> open_connections;
        std::condition_variable cond_var;
        std::mutex mtx;

        void errorCheck(int, const char *);

    public:
        Server () {};
        ~Server();

        void setupTcpConnection (const int);
        void handleConnection (std::function<void (int)>, int);

    protected:
        friend void thread_handler (std::function<void (int)>, Server &);
        friend void thread_workload (int);
    };


    /**************************************************************************
     * Methods definition of the Server class.
     *************************************************************************/

    void Server::errorCheck(int exp, const char *what) {
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


    Server::~Server () {
        if (this->isConnected) {
            #ifdef _WIN32
                closesocket(this->server_socket);
                WSACleanup();
            #elif defined(__linux__)
                close(this->server_socket);
            #endif
        }
    }


    void Server::setupTcpConnection (const int port = PORT) {
		// Initialize WinSock
		#ifdef _WIN32
			WSAData wsaData = {0};
			errorCheck(WSAStartup(MAKEWORD(2, 2), &wsaData),
				"WSAStartup() failed.");
		#endif

        // Create socket
        errorCheck(this->server_socket = socket(AF_INET, SOCK_STREAM, PROTOCOL),
            "Unable to create the socket.");
        
        // Reuse address and port
        errorCheck(setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, (const char*)&option_set, sizeof(option_set)),
            "Unable to set-up the socket.");
        
        // Set up address
        address.sin_family = AF_INET;
        address.sin_addr.s_addr = htonl(INADDR_ANY);
        address.sin_port = htons(port);

        // Attach socket to port
        errorCheck(bind(server_socket, (SA*)&address, sizeof(address)),
            "Unable to bind the socket.");
        
        // Set listener
        errorCheck(listen(server_socket, 1),
            "Unable to listen the port on this socket.");
        
        this->isConnected = true;
    }

    // Unfortunately it's impossible to pass method as a thread function
    void thread_handler (std::function<void (int)> thread_func, Server &this_server) {
        while (true) {
            int socket = -1;

            // If no incoming connections, sleep
            std::unique_lock<std::mutex> lock(this_server.mtx);
            if (this_server.open_connections.empty())
                this_server.cond_var.wait(lock);
            socket = this_server.open_connections.front();
            this_server.open_connections.pop();
            lock.unlock();

            if (socket != -1) {
                thread_func(socket);

				#ifdef _WIN32
					closesocket(socket);
				#elif defined(__linux__)
					close(socket);
				#endif
            }
        }
    };

    // Dummy function that is used to demonstrate server's capabilities
    void thread_workload (int socket_fd) {
        std::cout << "Thread #" << std::this_thread::get_id() << "\n";
    };


    void Server::handleConnection (std::function<void (int)> thread_func = thread_workload, int num_threads = 6) {
        std::vector<std::thread> thread_pool;
        
        // Create bunch of threads
        for (int i = 0; i < num_threads; i++){
            std::thread thr(thread_handler, thread_func, std::ref(*this));
            thread_pool.push_back(std::move(thr));
        }

        int new_socket;

        while (true) {
            // Accept incoming connection
            errorCheck(new_socket = accept(this->server_socket, (SA*)&address, (socklen_t*)&addrlen),
                "Unable to establish connection with client.");
            
            // Add new socket to the executional queue
            std::lock_guard<std::mutex> lock(this->mtx);
            this->open_connections.push(new_socket);
            this->cond_var.notify_one();
        }

    }

}