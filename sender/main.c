#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <stdio.h>
#include<stdlib.h>
#include <string.h>
#include <math.h>
#include <winsock2.h>
#include "SocketSendRecvTools.h"
#define MSG_LEN 31
#define DATA_LEN 26
#define STRINGS_ARE_EQUAL( Str1, Str2 ) ( strcmp( (Str1), (Str2) ) == 0 )

void addParity(int bitsq_length, char* bitsq,char pariadded_array[MSG_LEN+1]) {

	
	int no_of_parities=0, pos=0, position=0, count = 0;

	while (bitsq_length > (int)pow(2, pos) - (pos + 1)) { no_of_parities++;  pos++; }  // checking the number of parity positions using the ineqality

	int parityPos = 0;
	int nonparityPos = 0;
	for (int i = 0; i < no_of_parities + bitsq_length; i++)
	{
		if (i == ((int)pow(2, parityPos)-1))
		{
 			pariadded_array[i] = '0';  // assigning zeros to the indexes of powers of two
			parityPos++;
		}
		else
		{
			pariadded_array[i] = bitsq[nonparityPos];
			nonparityPos++;
		}
	}

	// checking even parity and assigning the relevent bit to the indexes of powers of two
	for (int i = 0; i < no_of_parities; i++) {
		position = (int)pow(2, i);
		int s, count = 0;
		s = position - 1;
		while (s < no_of_parities + bitsq_length)
		{
			for (int j = s; j < s + position; j++)
			{
				if (pariadded_array[j] == '1')
				{
					count++;
				}
			}
			s = s + 2 * position;
		}

		if (count % 2 == 0)
			pariadded_array[position - 1] = '0';
		else
			pariadded_array[position - 1] = '1';
	}
	
	pariadded_array[MSG_LEN] = '\0';
}

int main(int argc, char* argv[]) {
	SOCKET m_socket;
	FILE* fp = NULL;
	char ip[13] ;
	int port;
	port = atoi(argv[2]);
	strcpy(ip, argv[1]);
	SOCKADDR_IN clientService;
	// Initialize Winsock.
	WSADATA wsaData; //Create a WSADATA object called wsaData.
	//The WSADATA structure contains information about the Windows Sockets implementation.

	//Call WSAStartup and check for errors.
	int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != NO_ERROR)
		printf("Error at WSAStartup()\n");

	
	while (1) {
	//Call the socket function and return its value to the m_socket variable. 
	// For this application, use the Internet address family, streaming sockets, and the TCP/IP protocol.

	// Create a socket.
		m_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

		// Check for errors to ensure that the socket is a valid socket.
		if (m_socket == INVALID_SOCKET) {
			printf("Error at socket(): %ld\n", WSAGetLastError());
			WSACleanup();
			return -1;
		}
		char addr_1[20];
		strcpy(addr_1, ip);
		//Create a sockaddr_in object clientService and set  values.
		clientService.sin_family = AF_INET;
		clientService.sin_addr.s_addr = inet_addr(addr_1); //Setting the IP address to connect to
		clientService.sin_port = htons(port); //Setting the port to connect to.
		if (connect(m_socket, (SOCKADDR*)&clientService, sizeof(clientService)) == SOCKET_ERROR) {
			printf("Failed connecting to channel on %s:%d.\n", ip, port);
			exit(-1);
		}
		char file_name[100] = { 0 };
		printf("enter file name:\n");
		scanf("%s", file_name);
		if (STRINGS_ARE_EQUAL(file_name, "quit")) {
			closesocket(m_socket);
			exit(1);

		}
		fp = fopen(file_name, "r");
		if (fp == NULL) {
			printf("Reading %s failed!\n", file_name);
			exit(-1);
		}
		char data[DATA_LEN + 1] = { 0 };
		char* message = (char*)malloc((MSG_LEN + 1) * sizeof(char));
		if (message == NULL) {
			printf("Memory Allocation failed!\n");
			exit(-1);
		}
		int count = 0, file_length = 0, sent = 0;
		int k = 1;
		char* encoded_msg = (char*)calloc(1, sizeof(char));
		if (encoded_msg == NULL) {
			printf("Memory Allocation failed!\n");
			exit(-1);
		}
		while (!feof(fp)) {
			count = fread(&data, sizeof(char), DATA_LEN, fp);
			file_length += count;
			if (count == 0) {
				break;
			}
			addParity(DATA_LEN, data, message);
			sent += strlen(message);
			encoded_msg = realloc(encoded_msg, (k * MSG_LEN + 1) * sizeof(char));
			strcat(encoded_msg, message);
			fseek(fp, k * DATA_LEN, SEEK_SET);
			k++;
		}

		SendString(encoded_msg, m_socket);
		printf("file length : %d bytes\n", file_length / 8);
		printf("sent: %d bytes\n", sent / 8);
		free(message);
		free(encoded_msg);
		closesocket(m_socket);

	}

	return 0;
}