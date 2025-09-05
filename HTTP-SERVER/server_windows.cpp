#include "http_tcpServer_windows.h"
#include<winsock2.h>
#include<Windows.h>
#include<iostream>

int main()
{
	using namespace http;
	TcpServer server("127.0.0.1", 8080);
	server.StartListen();
	server.HandleClient();
	return 0;
}