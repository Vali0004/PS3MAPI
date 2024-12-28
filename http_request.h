#pragma once
#include "endian_swap.h"
#include "ps3mapi_types.h"
#if defined(_WIN32) || defined(WIN32)
#define W32
#endif
#ifdef W32
#include <windows.h>
#include <intrin.h>
#pragma comment(lib, "ws2_32.lib")
#else
#include <byteswap.h>
#include <unistd.h>
#include <cstring>
#endif
#define LITTLE_ENDIAN
#ifdef W32
#define close closesocket
#endif

inline void SendHTTPRequest(const std::string& ipAddress, const std::string& path)
{
    // Create a socket
    u64 sock{ socket(AF_INET, SOCK_STREAM, IPPROTO_TCP) };
    if (sock == NULL)
    {
        close(sock);
        return;
    }
    // Specify the server address and port
    in_addr inAddr{};
    inAddr.S_un.S_addr = inet_addr(ipAddress.c_str());
    sockaddr_in serverAddr{ AF_INET, htons(80), inAddr };
    // Connect to the server
    if (connect(sock, reinterpret_cast<sockaddr*>(&serverAddr), sizeof(serverAddr)) == SOCKET_ERROR)
    {
        close(sock);
        WSACleanup();
        return;
    }
    // Create the HTTP request
    std::string httpRequest = "GET " + path + " HTTP/1.1\r\nHost: " + ipAddress + "\r\n\r\n";
    // Send the HTTP request
    if (send(sock, httpRequest.c_str(), httpRequest.length(), 0) == SOCKET_ERROR)
    {
        close(sock);
        return;
    }
    // Receive and display the server's response
    char buffer[1024];
    int bytesReceived = recv(sock, buffer, sizeof(buffer), 0);
    if (bytesReceived != SOCKET_ERROR)
    {
        close(sock);
    }
}