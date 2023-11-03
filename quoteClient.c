#include "socket.h"

SOCKET startConnectSocket();

int main(int argc, const char *argv[]){
	WSADATA wsa;
	SOCKET server;
	char quote[MESSAGE_SIZE];
	int result;
	int i;
	
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
	
	// receive quote from server
	receiveMessage(server, (char*)quote);
	printf("%s", quote);
	
	// close server socket
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
	if((result = getaddrinfo(hostName, QUOTE_PORT, &hints, &address)) != 0){
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