~~~ SocketQuotes ~~~

~ In This Project
	- socket.h header file for using Windows and Windows Sockets 2 functionality
	- quotes.txt for supplying quotes to the program
	- quoteServer.c and quoteClient.c for sending quotes between programs or machines
	- multiQuoteServer.c and multiQuoteClient.c for sending quotes on demand

~ How To Use:
	- compile using "gcc -o *.exe *.c -lws2_32"
	- allow quoteServer or multiQuoteServer through firewall if prompted when ran (private network sufficient for local machine or LAN-connected machines)
	- run quoteServer with "quote.txt" or alternative quotes file as an argument (e.g. ".\quoteServer.exe quotes.txt")
	- run quoteClient with the server's host name as an argument (e.g. just ".\quoteClient.exe" on local machine)
	- run multiQuoteServer with quotes file as an argument, as with quoteServer
	- run multiQuoteClient with host name as an argument, as with quoteServer, request a quote from the server by entering ANOTHER in the prompt, and quit using CLOSE in the prompt
	- multiQuoteServer responds with a quote (receives "ANOTHER"), "BYE" (receives "CLOSE", quits afterwards), and "ERROR" (receives anything else)
	- multiQuoteServer buffer is flushed after each request

~ TCP Sockets
	- create socket with given family, type and protocol
	- bind socket to listen for clients (server)
	- connect to a listening socket for communication (client)
	- accept communication socket using listening socket (server)
	- send packets reliably through a communication socket to the destination (either, using Transmission Control Protocol)
	- receive packets reliably through a communication socket at the destination (either, using TCP, halting the program until a packet is received)
	- close sockets (server and client, independently)