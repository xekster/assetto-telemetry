
//precompiler headers are in pch
#include "pch.h"
#include <iostream>
#include <stdio.h>
#include <WinSock2.h>
#include <bitset>


#pragma comment(lib,"ws2_32.lib")
#define BUFLEN 408
#define SERVER "127.0.0.1"
#define PORT 9996

struct handshaker {
	int identifier;
	int version;
	int operationId;
};

struct handshakerResponse {
	char* carName;
	char* driverName;
	int identifier;
	int version;
	char* trackName;
	char* trackConfig;
};

struct gameInfo {
	float speed_Kmh;
	float engineRPM;
	int gear;
	int lapTime;
	int lastLap;
	int bestLap;
	int lapCount;
};


void assstring(char* big, char* ret) {
	int i;
	for (i = 0; i < 100; i += 2) {
		if (big[i] == '%') {
			ret[i / 2] = '\0';
			break;
		}
		else
			ret[i / 2] = big[i];
	}
}

int main()
{
	struct sockaddr_in si_other;
	int s, slen = sizeof(si_other);
	char buf[BUFLEN];
	char message[BUFLEN];
	WSADATA wsa;

	//Initialise winsock


	printf("Initialising Winsock...\n");
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		printf("Failed.Error Code : %d", WSAGetLastError());
		exit(EXIT_FAILURE);
	}
	printf("Initialised.\n");

	//create UDP socket
	if ((s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == SOCKET_ERROR)
	{
		printf(" socket() failed with error code : %d", WSAGetLastError());
		exit(EXIT_FAILURE);
	}

	//setup address structure

	memset((char *)&si_other, 0, sizeof(si_other));
	si_other.sin_family = AF_INET;
	si_other.sin_port = htons(PORT);
	si_other.sin_addr.S_un.S_addr = inet_addr(SERVER);

	//get car/driver/track info

	struct handshaker hs = { 0,0,0 };

	if (sendto(s,(const char*) &hs, sizeof(struct handshaker), 0, (struct sockaddr *) &si_other, slen) == SOCKET_ERROR)
	{
		printf("sendto() failed with error code : %d", WSAGetLastError());
		exit(EXIT_FAILURE);
	}

	memset(buf, '\0', BUFLEN);
	
	if (recvfrom(s, buf, BUFLEN, 0, (struct sockaddr *) &si_other, &slen) == SOCKET_ERROR)
	{
		printf(" recvfrom() failed with error code : %d", WSAGetLastError());
		exit(EXIT_FAILURE);
	}
	else {

		char* cnt = (char*) malloc( 50 * sizeof(char));
		assstring (buf, cnt);
		char* dnt = (char*)malloc(50 * sizeof(char));
		assstring(buf + 100, dnt);
		int ident = *reinterpret_cast<int32_t*>(buf + 200);
		int vert = *reinterpret_cast<int32_t*>(buf+204);
		char* tnt = (char*)malloc(50 * sizeof(char));
		assstring(buf + 208, tnt);
		char* tct = (char*)malloc(50 * sizeof(char));
		assstring(buf + 308, tct);
		
		handshakerResponse hsr= { cnt, dnt, ident, vert, tnt, tct };
		
		printf("car name: %s\n", hsr.carName);
		printf("driver name: %s\n", hsr.driverName);
		printf("car identifier: %d\n", hsr.identifier);
		printf("version: %d\n", hsr.version);
		printf("track name: %s\n", hsr.trackName);
		printf("track config: %s\n", hsr.trackConfig);

	}	

	Sleep(3000);

	/////////////////////////////////////////////////////////////////////////
	// game ticks

	handshaker kg = { 0,0,1 };
	gameInfo gi;
	gi = { 0,0,0,0,0,0,0 };
	sendto(s, (const char*)&kg, sizeof(struct handshaker), 0, (struct sockaddr *) &si_other, slen);

	size_t recbytes = 0;


	while(1){
		printf("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");
		if (recvfrom(s, buf, BUFLEN, 0, (struct sockaddr *) &si_other, &slen) == SOCKET_ERROR)
		{
			printf(" recvfrom() failed with error code : %d", WSAGetLastError());
			exit(EXIT_FAILURE);
		}
		else {
			gi.speed_Kmh= *reinterpret_cast<float*>(buf + 8);
			gi.engineRPM= *reinterpret_cast<float*>(buf + 68);
			gi.gear = *reinterpret_cast<int32_t*>(buf + 76) -1;
			printf("gear:%i//////enginerpm: %0.2f//////speed:%0.2f\n", gi.gear, gi.engineRPM, gi.speed_Kmh);
						
		}
		memset(buf, '\0', BUFLEN);

	}
	//close connection and exit
	closesocket(s);
	WSACleanup();

	return 0;
}