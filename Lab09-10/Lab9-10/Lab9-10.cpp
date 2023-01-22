#define _WINSOCK_DEPRECATED_NO_WARNINGS 
#include <iostream> 
#include <iomanip>
using namespace std;
#include <winsock2.h> 
#include <iphlpapi.h>    
#include <stdio.h> 
#include <conio.h> 
#include <string.>
#include <Ws2tcpip.h> 
#pragma comment(lib , "iphlpapi.lib")  
#pragma comment(lib , "ws2_32.lib")  

void GetMacAddress(unsigned char* mac, struct in_addr destip)
{
	DWORD ret;
	IPAddr srcip;
	ULONG MacAddr[2];
	ULONG PhyAddrLen = 6;
	int i;

	srcip = 0;

	//Послать ARP пакет        
	ret = SendARP((IPAddr)destip.S_un.S_addr, srcip, MacAddr, &PhyAddrLen);

	//Преобразовать адрес  
	if (PhyAddrLen)
	{
		BYTE* bMacAddr = (BYTE*)&MacAddr;
		for (i = 0; i < (int)PhyAddrLen; i++)
		{
			mac[i] = (char)bMacAddr[i];
		}
	}
}


void print_ip(unsigned int ip)
{
	unsigned char bytes[4];
	bytes[0] = ip & 0xFF;
	bytes[1] = (ip >> 8) & 0xFF;
	bytes[2] = (ip >> 16) & 0xFF;
	bytes[3] = (ip >> 24) & 0xFF;
	printf("%d.%d.%d.%d\n", bytes[0], bytes[1], bytes[2], bytes[3]);
}

int main() 
{
	setlocale(LC_ALL, "RUS");
	unsigned char mac[6];
	struct in_addr srcip = { 0 };
	struct sockaddr_in sa;
	char ip_address[32];
	WSADATA firstsock;
	if (WSAStartup(MAKEWORD(2, 2), &firstsock) != 0) 
	{
		cout << "Ошибка инициализации winsock";
		cout << WSAGetLastError();
		return -1;
	}
	cout << "Введите IP : ";
	cin >> ip_address;
	//преобразование IP адреса другим способом  //srcip.s_addr = inet_addr(ip_address);  
	inet_pton(AF_INET, ip_address, &(sa.sin_addr));
	//Получение MAC по IP  
	GetMacAddress(mac, sa.sin_addr);
	//GetMacAddress(mac, srcip); 
	printf("MAC адрес  : %.2X-%.2X-%.2X-%.2X-%.2X-%.2X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]); printf("\n");

	MIB_IPNETTABLE* pIpNetTable = (MIB_IPNETTABLE*) new char[0xFFFF];
	ULONG cbIpNetTable = 0xFFFF;

	char a1[3];
	char a2[3];
	char a3[3];
	char a4[3];
	char a5[3];
	char a6[3];
	cout << "MAC адрес: " << endl;
	cin >> a1 >> a2 >> a3 >> a4 >> a5 >> a6;


	if (NO_ERROR == GetIpNetTable(pIpNetTable, &cbIpNetTable, true)) 
	{
		for (DWORD i = 0; i < pIpNetTable->dwNumEntries; i++) 
		{
			if ((int(pIpNetTable->table[i].bPhysAddr[0]) == stoi(a1, 0, 16)) && (int(pIpNetTable->table[i].bPhysAddr[1]) == stoi(a2, 0, 16)) && (int(pIpNetTable->table[i].bPhysAddr[2]) == stoi(a3, 0, 16)) && (int(pIpNetTable->table[i].bPhysAddr[3]) == stoi(a4, 0, 16)) && (int(pIpNetTable->table[i].bPhysAddr[4]) == stoi(a5, 0, 16)) && (int(pIpNetTable->table[i].bPhysAddr[5]) == stoi(a6, 0, 16))) 
			{
				print_ip(pIpNetTable->table[i].dwAddr);
			}
		}
	}

	system("pause");
	return 0;
}