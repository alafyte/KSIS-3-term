﻿#include <iostream>
using namespace std;

#include <iostream>
#include <winsock2.h>
#include <iphlpapi.h>
#include <icmpapi.h>
#pragma comment(lib, "iphlpapi.lib")
#pragma comment(lib, "ws2_32.lib")
#pragma warning (disable: 4996)


IP_OPTION_INFORMATION option = { 255, 255, 240, 0, 0 };

void Ping(const char* cHost, //адрес хоста, который будет пинговаться
    unsigned int Timeout, //время ожидания ответа (в мс) после запроса
    unsigned int RequestCount) //количество посылаемых запросов
{
    // Создать файл сервиса
    HANDLE hIP = IcmpCreateFile();
    if (hIP == INVALID_HANDLE_VALUE) 
    {
        WSACleanup();
        return;
    }

    char SendData[32] = "Data for ping";    //буфер для передачи
    int LostPacketsCount = 0;               // кол-во потерянных пакетов
    unsigned int MaxMS = 0;                 // максимальное время ответа (мс)
    int MinMS = -1;                         // минимальное время ответа (мс)

    // Выделяем память под пакет – буфер ответа
    PICMP_ECHO_REPLY pIpe =
        (PICMP_ECHO_REPLY)GlobalAlloc(GHND, sizeof(ICMP_ECHO_REPLY) + sizeof(SendData));
        
    if (pIpe == 0) 
    {
        IcmpCloseHandle(hIP);
        WSACleanup();
        return;
    }
    pIpe->Data = SendData;
    pIpe->DataSize = sizeof(SendData);
    unsigned long ipaddr = inet_addr(cHost);

    for (unsigned int c = 0; c < RequestCount; c++) 
    {
        int dwStatus = IcmpSendEcho(hIP,
            ipaddr,
            SendData,
            sizeof(SendData),
            &option,
            pIpe,
            sizeof(ICMP_ECHO_REPLY) + sizeof(SendData),
            Timeout);
        if (dwStatus > 0) 
        {
            for (int i = 0; i < dwStatus; i++) 
            {
                unsigned char* pIpPtr =
                    (unsigned char*)&pIpe->Address;
                cout << "Ответ от " << (int)*(pIpPtr)
                    << "." << (int)*(pIpPtr + 1)
                    << "." << (int)*(pIpPtr + 2)
                    << "." << (int)*(pIpPtr + 3)
                    << ": число байт = " << pIpe->DataSize
                    << " время = " << pIpe->RoundTripTime
                    << "мс TTL = " << (int)pIpe->Options.Ttl;
                MaxMS = (MaxMS > pIpe->RoundTripTime) ?
                    MaxMS : pIpe->RoundTripTime;
                MinMS = (MinMS < (int)pIpe->RoundTripTime
                    && MinMS >= 0) ?
                    MinMS : pIpe->RoundTripTime;
                cout << endl;
            }
        }
        else 
        {
            if (pIpe->Status) 
            {
                LostPacketsCount++;
                switch (pIpe->Status) 
                {
                case IP_DEST_NET_UNREACHABLE:
                case IP_DEST_HOST_UNREACHABLE:
                case IP_DEST_PROT_UNREACHABLE:
                case IP_DEST_PORT_UNREACHABLE:
                    cout << "Remote host may be down." << endl;
                    break;
                case IP_REQ_TIMED_OUT:
                    cout << "Request timed out." << endl;
                    break;
                case IP_TTL_EXPIRED_TRANSIT:
                    cout << "TTL expired in transit." << endl;
                    break;
                default:
                    cout << "Error code: %ld"
                        << pIpe->Status << endl;
                    break;
                }
            }
        }
    }

    IcmpCloseHandle(hIP);
    WSACleanup();

    if (MinMS < 0) MinMS = 0;
    unsigned char* pByte = (unsigned char*)&pIpe->Address;
    cout << "Статистика Ping "
        << (int)*(pByte)
        << "." << (int)*(pByte + 1)
        << "." << (int)*(pByte + 2)
        << "." << (int)*(pByte + 3) << endl;
    cout << "\tПакетов: отправлено = " << RequestCount
        << ", поучено = "
        << RequestCount - LostPacketsCount
        << ", потеряно = " << LostPacketsCount
        << "<" << (int)(100 / (float)RequestCount) *
        LostPacketsCount
        << " % потерь>, " << endl;
    cout << "Приблизительное время приема-передачи:"
        << endl << "Минимальное = " << MinMS
        << "мс, Максимальное = " << MaxMS
        << "мс, Среднее = " << (MaxMS + MinMS) / 2
        << "мс" << endl;

}


int main(int argc, char** argv)
{
    setlocale(LC_ALL, "RUS");
    const char* par0 = argv[1];
    int par1 = atoi(argv[2]);
    int par2 = atoi(argv[3]);
    Ping(par0, par1, par2);
    //Ping("81.19.70.1", 60, 10);
    system("pause");
    return 0;
}

