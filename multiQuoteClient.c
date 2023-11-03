#include "socket.h"

SOCKET startConnectSocket();

int main(int argc, const char *argv[]){
	WSADATA wsa;
	SOCKET server;
	char buffer[MESSAGE_SIZE];
	int result;
	
	// initialise WinSock
	if((result = WSAStartup(MAKEWORD(2, 2), &wsa)) != 0){
		printf("Error: wsa failed (code %d)\n", result);
		return 1;
	}
	
	// start server socket
	if((server = startConnectSocket(argv[1])) == INVALID_SOCKET){
		WSACleanup();
		return 1;
	}
	
	// receive messages from server
	while(1){
		receiveMessage(server, (char*)buffer);
		printf("%s", buffer);
		
		// terminate
		if(strncmp("BYE\r\n", buffer, strlen("BYE\r\n")) == 0) break;
		
		// request to server
		printf("prompt for server: ");
		scanf("%s", (char*)&buffer);
		strcat(buffer, "\r\n");
		if((result = send(server, buffer, strlen(buffer), 0)) == SOCKET_ERROR){
			printf("Error: send failed (code %d)\n", WSAGetLastError());
			closesocket(server);
			WSACleanup();
			return 1;
		}
	}
	
	// close server socket connection
	if((result = shutdown(server, SD_SEND)) == SOCKET_ERROR){
		printf("Error: shutdown failed (code %d)\n", WSAGetLastError());
		closesocket(server);
		WSACleanup();
		return 1;
	}
	closesocket(server);
	
	// cleanup
	WSACleanup();
	return 0;
}

SOCKET startConnectSocket(char *hostName){
	SOCKET sock = INVALID_SOCKET;
	struct addrinfo hints, *address, *currentAddress;
	int result;
	
	// address information
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	if((result = getaddrinfo(hostName, MULTIQUOTE_PORT, &hints, &address)) != 0){
		printf("Error: getaddrinfo failed (code %d)\n", result);
		return INVALID_SOCKET;
	}
	
	// create socket
	if((sock = socket(address->ai_family, address->ai_socktype, address->ai_protocol)) == INVALID_SOCKET){
		printf("Error: socket failed (code %d)\n", WSAGetLastError());
		freeaddrinfo(address);
		return INVALID_SOCKET;
	}
	
	// connect to socket
	currentAddress = address;
	while(currentAddress != NULL){
		if((result = connect(sock, address->ai_addr, (int)address->ai_addrlen)) != SOCKET_ERROR) break;
		printf("Connection failed to %s (code %d)\n", address->ai_canonname, WSAGetLastError());
		currentAddress = currentAddress->ai_next;
	}
	freeaddrinfo(address);
	if(result == SOCKET_ERROR){
		printf("Error: no connection\n");
		closesocket(sock);
		return INVALID_SOCKET;
	}
	
	return sock;
}