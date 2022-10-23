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

/* This code has three parts
      1. Identifying whether the sequence has errors
      2. Navigating to the error, correcting them & removing the parity bits to get the original sequence
      3. If there is no error removing the parity bits to get the original sequence
*/

int parityRemove(int no_of_extra, int ltot, char* encyptedRedBitsq,char* original_msg) {
  
    int ss=0, sss, error=0, pos_of_orisig=0, pos_of_redsig = 0;

    // checking whether there are any errors
    for (int i = 0; i < no_of_extra; i++)
    {
        int count=0, value = 0;
        int position = (int)pow(2, i);
        ss = position - 1;
        while (ss < ltot)
        {
            for (sss = ss; sss < ss + position; sss++)
            {
                if (encyptedRedBitsq[sss] == '1')
                    count++;
            }
            ss = ss + 2 * position;
        }
        if (count % 2 != 0) { error += position; }
    }
    
    // navigating to the errornous bits & correct them
    if (error != 0) {
      
        if (encyptedRedBitsq[error - 1] == '1') { encyptedRedBitsq[error - 1] = '0'; }
        else { encyptedRedBitsq[error - 1] = '1'; }
        for (int i = 0; i < ltot; i++)
        {
            if (i == ((int)pow(2, pos_of_orisig) - 1))
            {
                pos_of_orisig++;
            }
            else
            {
                original_msg[pos_of_redsig] = encyptedRedBitsq[i];
                pos_of_redsig++;
            }
        }
    }

    // removing the excessive parity bits of the sequences without the errors
    else {
        for (int i = 0; i < ltot; i++)
        {
            if (i == ((int)pow(2, pos_of_orisig) - 1))
            {
                pos_of_orisig++;
            }
            else
            {
                original_msg[pos_of_redsig] = encyptedRedBitsq[i];
                pos_of_redsig++;
            }
        }
    }
    return error;
}
int main(int argc, char* argv[]) {

	SOCKET m_socket;
	char ip[13];
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

	

    char* perturbed_message ;
    while (1) {
        //Call the socket function and return its value to the m_socket variable. 
    // For this application, use the Internet address family, streaming sockets, and the TCP/IP protocol.
        perturbed_message = NULL;
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

        printf("enter file name:\n");
        char file_name[100] = { 0 };
        scanf("%s", file_name);
        if (STRINGS_ARE_EQUAL(file_name, "quit")) {
            exit(1);

        }

        ReceiveString(&perturbed_message, m_socket);
        printf("received: %d bytes\n", strlen(perturbed_message) / 8);

        int blocks = strlen(perturbed_message) / 31;
        int extra = blocks * 5;
        int original_length = strlen(perturbed_message) - extra;
        char* original_msg = (char*)calloc(original_length + 1, sizeof(char));
        if (original_msg == NULL) {
            printf("Memory allocation dailed!\n");
            exit(-1);
        }
        char* block = (char*)calloc(DATA_LEN + 1, sizeof(char));
        if (block == NULL) {
            printf("Memory allocation failed!\n");
            exit(-1);
        }
        FILE* fp = NULL;
        fp = fopen(file_name, "w");
        if (fp == NULL) {
            printf("opening file for writing failed!\n");
            exit(-1);
        }
        char temp[MSG_LEN + 1] = { 0 };
        int i = 0,error=0,total_errors=0;
        while (i < blocks) {

            strncpy(temp, perturbed_message + i * MSG_LEN, MSG_LEN);
           error= parityRemove(MSG_LEN - DATA_LEN, MSG_LEN, temp, block);
           total_errors += error;
            strcat(original_msg, block);
            i++;


        }

        fwrite(original_msg, sizeof(char), strlen(original_msg), fp);
        printf("corrected %d errors\n", total_errors);
        free(original_msg);
        free(block);
        fclose(fp);
        

    }
	
	return 0;
}