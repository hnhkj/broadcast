#include <stdio.h>
#include<winsock.h>
#include <time.h>

#pragma comment(lib,"ws2_32.lib") 

int main()
{
	WSADATA wsaData = {0};                                   
	if(0 != WSAStartup(MAKEWORD( 2, 2 ), &wsaData ))           
	{
		printf("WSAStartup failed.errno=[%d]\n", WSAGetLastError());
		return -1;
	}

	SOCKET sockServer	= socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (SOCKET_ERROR == sockServer)
	{
		printf ("socket create failed.errno=[%d]", WSAGetLastError());
		return	-2;
	}
		
	SOCKADDR_IN	addrServer	= {0};
	addrServer.sin_family = AF_INET;
	addrServer.sin_addr.s_addr = htonl(INADDR_ANY);
	addrServer.sin_port = htons(10001);             
	if(bind( sockServer, (SOCKADDR*)&addrServer, sizeof(addrServer))!=0)
	{
		printf("bind failed.errno=[%d]\n", WSAGetLastError());//³õÊ¼»¯Ê§°Ü·µ»Ø-1
		return -2;
	}

	while(1)
	{
		SOCKADDR_IN	addrClient = {0};                          
		int nAddrLen	= sizeof(addrClient);

		char pcRecvBuff[UCHAR_MAX] = {0};                               
		
		if(SOCKET_ERROR == recvfrom (sockServer,pcRecvBuff, sizeof(pcRecvBuff), 0, (SOCKADDR FAR *) &addrClient, &nAddrLen)) 
		{
			printf("recvfrom failed.errno=[%d]", WSAGetLastError());
			break;
		}
		else
		{
			time_t	rawTime;
			time(&rawTime);
			tm *t = localtime(&rawTime);
	
			printf("%04d-%02d-%02d %02d-%02d-%02d | ip=[%s] recv=[%s]\n", t->tm_year+1900, t->tm_mon+1, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec, 
				inet_ntoa(addrClient.sin_addr), pcRecvBuff);
		}
	}

	closesocket(sockServer);    
	WSACleanup();

	return 0;
}