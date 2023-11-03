#ifndef HEADER_SOCKET
#define HEADER_SOCKET

// windows libraries
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>

// standard libraries
#include <stdio.h>
#include <string.h>

#define QUOTE_PORT      "1717"
#define MULTIQUOTE_PORT "1818"
#define BACKLOG_LIMIT   15
#define QUOTE_SIZE      512
#define BUFFER_SIZE     (QUOTE_SIZE + 2)
#define MESSAGE_SIZE    (BUFFER_SIZE + 1)

void receiveMessage(SOCKET sock, char *buffer){
	char *messagePosition = buffer;
	int messageSize = 0;
	int result;
	ZeroMemory(buffer, BUFFER_SIZE);
	do{
		result = recv(sock, messagePosition, BUFFER_SIZE - messageSize, 0);
		if(result < 0) printf("Error: recv failed (code %d)\n", WSAGetLastError());
		else{
			messageSize += result;
			messagePosition = &buffer[messageSize];
			buffer[messageSize] = '\0';
		}
	} while(result > 0 && (messageSize == 0 || strncmp("\r\n", &buffer[messageSize - 2], 2)));
}

#endif