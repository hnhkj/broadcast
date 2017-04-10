#include <stdio.h>
#include <winsock.h>
#pragma comment(lib,"ws2_32.lib")

int main()
{
	WSADATA wsaData = {0};                                   
	if(0 != WSAStartup(MAKEWORD( 2, 2 ), &wsaData ))     
	{
		printf("WSAStartup failed.errno=[%d]", WSAGetLastError());//初始化失败返回-1
		return -1;
	}
	
	SOCKET sockClient	= socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (SOCKET_ERROR == sockClient)
	{
		printf ("socket create failed.errno=[%d]", WSAGetLastError());
		return	-2;
	}

	BOOL bBroadcast = TRUE;                             
	if (0 != setsockopt ( sockClient,SOL_SOCKET,SO_BROADCAST, (CHAR *)&bBroadcast, sizeof(BOOL)))
	{
		printf ("setsockopt failed.errno=[%d]", WSAGetLastError());
		return	-3;
	}

#if 1 /// 多网卡的需要自己制定用哪一个网卡来发送广播，单网卡的不需要也行
	SOCKADDR_IN	addrClient		= {0};
	addrClient.sin_family		= AF_INET;
	addrClient.sin_addr.s_addr	= inet_addr("192.168.1.2");
	addrClient.sin_port			= htons(10002);
	if (0 != bind (sockClient, (sockaddr*)&addrClient, sizeof(addrClient)))
	{
		printf ("bind failed. errno=[%d]\n", WSAGetLastError());
	}
#endif 

	SOCKADDR_IN addrServer = {0};                              
	addrServer.sin_family = AF_INET;
	addrServer.sin_addr.s_addr = htonl ( INADDR_BROADCAST );
	addrServer.sin_port = htons (10001);          
	int		nConunter = 0;	     
	
	while(1) 
	{
		char pcSendMsg[UCHAR_MAX] = {0};
		sprintf(pcSendMsg,"times: %d", nConunter++);
		sendto ( sockClient, pcSendMsg, strlen(pcSendMsg)+1, 0, (SOCKADDR *) &addrServer, sizeof ( SOCKADDR_IN ));
		printf("%s\n",pcSendMsg);
		Sleep(1000);
	}

	closesocket(sockClient);    
	WSACleanup();
	return 0;
}