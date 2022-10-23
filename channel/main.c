#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <stdio.h>
#include<stdlib.h>
#include <string.h>
#include <winsock2.h>

///#include "SocketExampleShared.h"
#include "SocketSendRecvTools.h"
#define MSG_LEN 31
#define DATA_LEN 26
#define STRINGS_ARE_EQUAL( Str1, Str2 ) ( strcmp( (Str1), (Str2) ) == 0 )

// guesing port number between lower and upper using rand().
int guess_random_port(int lower, int upper)
{

	int num = 0;
	num = (rand() % (upper - lower + 1)) + lower;
	return num;
}

int main(int argc, char* argv[]) {

	srand(time(NULL));
	char* IP_address;
	char* host_name="";
	int sender_port, reciever_port;
	SOCKET S_Socket = INVALID_SOCKET;
	SOCKET R_Socket = INVALID_SOCKET;
	unsigned long Address;
	SOCKADDR_IN sender;
	SOCKADDR_IN reciever;
	int bindRes;
	int ListenRes;


	//IP_address = getIP();
	IP_address = "127.0.0.1";
	sender_port = guess_random_port(1024, 64000);
	reciever_port = guess_random_port(1024, 64000);
	// Initialize Winsock.
	WSADATA wsaData;
	int StartupRes = WSAStartup(MAKEWORD(2, 2), &wsaData);

	if (StartupRes != NO_ERROR)
	{
		printf("error %ld at WSAStartup( ), ending program.\n", WSAGetLastError());
		// Tell the user that we could not find a usable WinSock DLL.                                  
		return -1;
	}
	/* The WinSock DLL is acceptable. Proceed. */

   // Create a socket.    
	S_Socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (S_Socket == INVALID_SOCKET)
	{
		printf("Error at socket( ): %ld\n", WSAGetLastError());
		
	}
	R_Socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (R_Socket == INVALID_SOCKET)
	{
		printf("Error at socket( ): %ld\n", WSAGetLastError());

	}
	// Bind the socket.
	/*
		For a server to accept client connections, it must be bound to a network address within the system.
		The following code demonstrates how to bind a socket that has already been created to an IP address
		and port.
		Client applications use the IP address and port to connect to the host network.
		The sockaddr structure holds information regarding the address family, IP address, and port number.
		sockaddr_in is a subset of sockaddr and is used for IP version 4 applications.
   */
   // Create a sockaddr_in object and set its values.
   // Declare variables

	Address = inet_addr(IP_address);
	if (Address == INADDR_NONE)
	{
		printf("The string \"%s\" cannot be converted into an ip address. ending program.\n",IP_address);
	}

	sender.sin_family = AF_INET;
	sender.sin_addr.s_addr = Address;
	sender.sin_port = htons(sender_port); //The htons function converts a u_short from host to TCP/IP network byte order ( which is big-endian ).
	reciever.sin_family = AF_INET;
	reciever.sin_addr.s_addr = Address;
	reciever.sin_port = htons(reciever_port); //The htons function converts a u_short from host to TCP/IP network byte order ( which is big-endian ).



	printf("sender socket: IP address = %s port = %d\n", IP_address, sender_port);
	printf("receiver socket : IP address = %s port = %d\n", IP_address, reciever_port);
	// Call the bind function, passing the created socket and the sockaddr_in structure as parameters. 
// Check for general errors.
	bindRes = bind(S_Socket, (SOCKADDR*)&sender, sizeof(sender));
	if (bindRes == SOCKET_ERROR)
	{
		printf("bind( ) failed with error %ld. Ending program\n", WSAGetLastError());
	}


	bindRes = bind(R_Socket, (SOCKADDR*)&reciever, sizeof(reciever));
	if (bindRes == SOCKET_ERROR)
	{
		printf("bind( ) failed with error %ld. Ending program\n", WSAGetLastError());
	}
	char* message;
	while (1) {

		// Listen on the Socket.
		ListenRes = listen(S_Socket, SOMAXCONN);
		if (ListenRes == SOCKET_ERROR)
		{
			printf("Failed listening on socket, error %ld.\n", WSAGetLastError());
		}
		ListenRes = listen(R_Socket, SOMAXCONN);
		if (ListenRes == SOCKET_ERROR)
		{
			printf("Failed listening on socket, error %ld.\n", WSAGetLastError());
		}

		SOCKET sender_socket = accept(S_Socket, NULL, NULL);
		if (sender_socket == INVALID_SOCKET)
		{
			printf("Accepting connection with sender failed, error %ld\n", WSAGetLastError());

		}

		SOCKET reciever_socket = accept(R_Socket, NULL, NULL);
		if (reciever_socket == INVALID_SOCKET)
		{
			printf("Accepting connection with reciever failed, error %ld\n", WSAGetLastError());
		}

		message= NULL;
		ReceiveString(&message, sender_socket);
		int length = strlen(message);
		
		char mode[3] = { 0 };
		int n = atoi(argv[2]);
		strcpy(mode, argv[1]);
		int m = n;
		int flipped = 0;
		if ((STRINGS_ARE_EQUAL(mode, "-d")) && (n != 0)) {	/// Deterministic
			while ((m <= length)) {

				if (message[m] == '0') {
					message[m] = '1';
					flipped += 1;
				}
				else {
					message[m] = '0';
					flipped += 1;
				}
				m += n;

			}

		}

		else {			///random 
			int i = 0;
			int n = atoi(argv[2]);
			unsigned int seed = atoi(argv[3]);
			srand(seed);

			while (i < length) {
				if (rand() < n) {
					if (message[i] == '0') {
						message[i] = '1';
						flipped += 1;
					}
					else {
						message[i] = '0';
						flipped += 1;
					}
				}
				i++;
			}

		}

		SendString(message, reciever_socket);
		printf("retransmitted %d bytes, flipped %d bits\n",length/8,flipped);
		printf("continue? (yes/no)\n");
		char tmp[10] = { 0 };
		scanf("%s", tmp);
		if (STRINGS_ARE_EQUAL(tmp, "no")) {
			closesocket(reciever_socket);
			closesocket(sender_socket);
			exit(1);
		}
		closesocket(reciever_socket);


	}

	
}
