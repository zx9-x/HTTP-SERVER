#ifndef INCLUDED_HTTP_TCPSERVER_WINDOWS
#define INCLUDED_HTTP_TCPSERVER_WINDOWS
#include<WinSock2.h>
#include<iostream>
#include<fstream>
#include<string>
namespace http
{
	class TcpServer
	{
	public:
		TcpServer(std::string ip_address, int port);
		~TcpServer();
		void StartListen();
		void HandleClient();
		std::string readFile(const std::string& path);
		std::string GetContentFile(const std::string& path);

	private:
		WSAData wsaData;
		SOCKET srv_socket;
		SOCKET accept_socket;
		std::string srv_ip_address;
		int srv_port;
		std::string srv_InMessage;
		struct sockaddr_in srv_socketAddress;
		int srv_socketAddress_len;
		void ExitWithError(const std::string err_msg)
		{
			std::cout << std::endl << err_msg
				<< ", With error :" << WSAGetLastError() << std::endl;
			WSACleanup();
			exit(1);
		}
		int ServerStart();
		void ServerClose();
	};
}
#endif