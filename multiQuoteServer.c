#include "socket.h"
#include <time.h>

SOCKET startListenSocket();

void getRandomQuote(char *buffer, FILE *fp);

int main(int argc, const char *argv[]){
	WSADATA wsa;
	SOCKET server, client;
	FILE *fp;
	char buffer[QUOTE_SIZE];
	int result;
	
	// initialise WinSock
	if((result = WSAStartup(MAKEWORD(2, 2), &wsa)) != 0){
		printf("Error: wsa failed (code %d)\n", result);
		return 1;
	}
	
	// start server socket
	if((server = startListenSocket()) == INVALID_SOCKET){
		WSACleanup();
		return 1;
	}
	
	// listen on socket
	if(listen(server, BACKLOG_LIMIT) == SOCKET_ERROR){
		printf("Error: listen failed (code %d)\n", WSAGetLastError());
		closesocket(server);
		WSACleanup();
		return 1;
	}
	
	// accept client socket connection
	client = INVALID_SOCKET;
	if((client = accept(server, NULL, NULL)) == INVALID_SOCKET){
		printf("Error: accept failed (code %d)\n", WSAGetLastError());
		closesocket(server);
		WSACleanup();
		return 1;
	}
	
	// close server socket
	closesocket(server);
	
	// open quote file
	if(!(fp = fopen(argv[1], "r"))){
		printf("Error: file open failed\n");
		closesocket(client);
		WSACleanup();
		return 1;
	}
	srand(time(NULL));
	
	// send first message
	getRandomQuote(buffer, fp);
	if((result = send(client, buffer, strlen(buffer), 0)) == SOCKET_ERROR){
		printf("Error: send failed (code %d)\n", WSAGetLastError());
		closesocket(client);
		WSACleanup();
		return 1;
	}
	
	// wait for requests from client
	while(1){
		receiveMessage(client, (char*)&buffer);
		
		// retrieve quote
		if(strncmp("ANOTHER\r\n", buffer, strlen("ANOTHER\r\n")) == 0){
			getRandomQuote(buffer, fp);
			if((result = send(client, buffer, strlen(buffer), 0)) == SOCKET_ERROR){
				printf("Error: send failed (code %d)\n", WSAGetLastError());
				closesocket(client);
				WSACleanup();
				return 1;
			}
		}
		
		// terminate
		else if(strncmp("CLOSE\r\n", buffer, strlen("CLOSE\r\n")) == 0){
			strcpy(buffer, "BYE\r\n");
			if((result = send(client, buffer, strlen(buffer), 0)) == SOCKET_ERROR){
				printf("Error: send failed (code %d)\n", WSAGetLastError());
				closesocket(client);
				WSACleanup();
				return 1;
			}
			break;
		}
		
		// request reattempt
		else{
			strcpy(buffer, "ERROR\r\n");
			if((result = send(client, buffer, strlen(buffer), 0)) == SOCKET_ERROR){
				printf("Error: send failed (code %d)\n", WSAGetLastError());
				closesocket(client);
				WSACleanup();
				return 1;
			}
		}
	}
	fclose(fp);
	
	// close client socket connection
	if((result = shutdown(client, SD_SEND)) == SOCKET_ERROR){
		printf("Error: shutdown failed (code %d)\n", WSAGetLastError());
		closesocket(client);
		WSACleanup();
		return 1;
	}
	closesocket(client);
	
	// cleanup
	WSACleanup();
	return 0;
}

SOCKET startListenSocket(){
	SOCKET sock = INVALID_SOCKET;
	struct addrinfo hints, *address;
	int result;
	
	// address information
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;
	if((result = getaddrinfo(NULL, MULTIQUOTE_PORT, &hints, &address)) != 0){
		printf("Error: getaddrinfo failed (code %d)\n", result);
		return INVALID_SOCKET;
	}
	
	// create socket
	if((sock = socket(address->ai_family, address->ai_socktype, address->ai_protocol)) == INVALID_SOCKET){
		printf("Error: socket failed (code %d)\n", WSAGetLastError());
		freeaddrinfo(address);
		return INVALID_SOCKET;
	}
	
	// bind socket
	if((result = bind(sock, address->ai_addr, (int)address->ai_addrlen)) == SOCKET_ERROR){
		printf("Error: bind failed (code %d)\n", WSAGetLastError());
		freeaddrinfo(address);
		closesocket(sock);
		return INVALID_SOCKET;
	}
	freeaddrinfo(address);
	
	return sock;
}

void getRandomQuote(char *buffer, FILE *fp){
	int id, i;
	ZeroMemory(buffer, BUFFER_SIZE);
	id = (rand() % 43) + 1;
	rewind(fp);
	for(i = 0; i < id; i++) fgets(buffer, BUFFER_SIZE, fp);
	strcat(buffer,"\r\n");
}