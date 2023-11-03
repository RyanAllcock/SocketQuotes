#include "socket.h"
#include <time.h>

SOCKET startListenSocket();

int main(int argc, const char *argv[]){
	WSADATA wsa;
	SOCKET server, client;
	char quote[QUOTE_SIZE];
	FILE *fp;
	int result;
	int id;
	int i;
	
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
	
	// retrieve quote from file
	if(!(fp = fopen(argv[1], "r"))){
		printf("Error: file open failed\n");
		closesocket(client);
		WSACleanup();
		return 1;
	}
	srand(time(NULL));
	id = (rand() % 43) + 1;
	for(i = 0; i < id; i++) fgets(quote, BUFFER_SIZE, fp);
	strcat(quote,"\r\n");
	fclose(fp);
	
	// send quote
	if((result = send(client, quote, strlen(quote), 0)) == SOCKET_ERROR){
		printf("Error: send failed (code %d)\n", WSAGetLastError());
		closesocket(client);
		WSACleanup();
		return 1;
	}
	
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
	if((result = getaddrinfo(NULL, QUOTE_PORT, &hints, &address)) != 0){
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