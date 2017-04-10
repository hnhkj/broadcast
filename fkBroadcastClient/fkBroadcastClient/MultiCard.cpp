#include <stdio.h>
#include <winsock.h>
#include <time.h>
#pragma comment(lib,"ws2_32.lib")


DWORD WINAPI ThreadProc(_In_  LPVOID lpParameter)
{
	char	szIp[16] = {0};
	strcpy (szIp, (char*)lpParameter);
	SOCKET sockClient	= socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (SOCKET_ERROR == sockClient)
	{
		printf ("socket create failed. ip=[%s] errno=[%d] ", szIp, WSAGetLastError());
		return	-2;
	}

	BOOL bBroadcast = TRUE;                             
	if (0 != setsockopt ( sockClient,SOL_SOCKET,SO_BROADCAST, (CHAR *)&bBroadcast, sizeof(BOOL)))
	{
		printf ("setsockopt failed. ip=[%s] errno=[%d]", szIp, WSAGetLastError());
		return	-3;
	}

	SOCKADDR_IN	addrClient		= {0};
	addrClient.sin_family		= AF_INET;
	addrClient.sin_addr.s_addr	= inet_addr(szIp);
	addrClient.sin_port			= 0;				/// 0 表示由系统自动分配端口号
	if (0 != bind (sockClient, (sockaddr*)&addrClient, sizeof(addrClient)))
	{
		printf ("bind failed.ip=[%s] errno=[%d]\n", szIp, WSAGetLastError());
	}

	SOCKADDR_IN addrServer = {0};                              
	addrServer.sin_family = AF_INET;
	addrServer.sin_addr.s_addr = htonl ( INADDR_BROADCAST );
	addrServer.sin_port = htons (10001);          
	int		nConunter = 0;	     

	while(1) 
	{
		char pcSendMsg[UCHAR_MAX] = {0};
		
		time_t	rawTime;
		time(&rawTime);
		tm *t = localtime(&rawTime);
		
		sprintf(pcSendMsg, "%04d-%02d-%02d %02d-%02d-%02d | ip=[%s]", t->tm_year+1900, t->tm_mon+1, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec, szIp);

		sendto ( sockClient, pcSendMsg, strlen(pcSendMsg)+1, 0, (SOCKADDR *) &addrServer, sizeof ( SOCKADDR_IN ));
		printf("%s\n",pcSendMsg);
		Sleep(1000);
	}

	closesocket(sockClient);  

	return	0;
}


int main()
{
	WSADATA wsaData = {0};                                   
	if(0 != WSAStartup(MAKEWORD( 2, 2 ), &wsaData ))     
	{
		printf("WSAStartup failed.errno=[%d]", WSAGetLastError());//初始化失败返回-1
		return -1;
	}

	char hostname[1024] = {0};
	gethostname(hostname, sizeof(hostname));    //获得本地主机名
	PHOSTENT hostinfo = gethostbyname(hostname);//信息结构体
	while(*(hostinfo->h_addr_list) != NULL)        //输出IP地址
	{
		char* ip = inet_ntoa(*(struct in_addr *) *hostinfo->h_addr_list);

		CreateThread(NULL, 0, ThreadProc, ip, 0, 0);
		Sleep(100);
		hostinfo->h_addr_list++;
	}

	Sleep(INFINITE);
	  
	WSACleanup();
	return 0;
}