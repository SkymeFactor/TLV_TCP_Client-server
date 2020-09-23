# Client-Server socket based application.
## Description
This is a simple example of client-server communication over TCP/IP protocol. Connection comes between multi-threaded server and single-threaded client. Core feature of this project is the BER ANS.1 based length encription, so the package length is being encoded as follows:
 - Eight's bit of the first octet is 0 in case of message length is less then 128 and the length itself is within this octet.
 - Eight's bit of the first octet is 1 in case of message length is greater then 127 and the rest of this byte is holding the number of additional bytes that are storing the actual length in big endian order base 256.

BER is pretty naive though, so some issues might appear.

## Set-Up
Building is pretty easy, just execute command `make` within the corresponding folder. It generates two executable files, which are the client and the server respectivly.

Not sure, how it works for windows.
## Ease-Of-Use
Server is designed to work in a background, so running it just once with no arguments is going to be enough. Client on the other hand is a run'n'die sort of application and it requires two parameters (which are not crusial, but recommended):
 - IP address (127.0.0.1 if no provided)
 - Message ("Hello from client" by default)
 ## System requirements:
 - g++/MSVC supporting c++11 at least (be careful, 17 is marked in the Makefile)
 - Multi-threading supporting OS


 