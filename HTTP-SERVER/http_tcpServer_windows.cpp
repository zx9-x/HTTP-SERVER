#include "http_tcpServer_windows.h"
#include<iostream>
#include <WS2tcpip.h> 
#include<fstream>
#include<string>
namespace http
{
	TcpServer::TcpServer(std::string ip_address, int port)
		:srv_ip_address(ip_address),
		srv_port(port),
		accept_socket(),
		srv_InMessage(),
		srv_socketAddress(),
		srv_socketAddress_len(sizeof(srv_socketAddress))
		
	{
		ServerStart();
	}
	TcpServer::~TcpServer()
	{
		ServerClose();
	}
	int TcpServer::ServerStart()
	{
		WORD wVersionRequested = MAKEWORD(2, 0);
		if (WSAStartup(wVersionRequested, &wsaData) != 0)
		{
			ExitWithError("WSAStartup failed");
		}
		srv_socket = socket(AF_INET, SOCK_STREAM, 0);
		if (srv_socket == INVALID_SOCKET)
		{
			ExitWithError("Socket() creation failed");
		}
		srv_socketAddress.sin_family = AF_INET;
		srv_socketAddress.sin_port = htons(srv_port);
		if (inet_pton(AF_INET, "127.0.0.1", &srv_socketAddress.sin_addr) <= 0)
		{
			ExitWithError("Invalid IP address");
		}
		if (bind(srv_socket, (SOCKADDR*)&srv_socketAddress, srv_socketAddress_len) == SOCKET_ERROR)
		{
			closesocket(srv_socket);
			closesocket(accept_socket);
			ExitWithError("Bind is failed");
		}
		return 0;
	}
	void TcpServer::StartListen()
	{
		if (listen(srv_socket, 1) == SOCKET_ERROR)
		{
			closesocket(srv_socket);
			closesocket(accept_socket);
			ExitWithError("Listening failed");
		}
		else
		{
			std::cout << "Listening..." << std::endl;
		}
	}
	void TcpServer::HandleClient()
	{
		do {
			accept_socket = accept(srv_socket, NULL, NULL);
			if (accept_socket == INVALID_SOCKET)
			{
				closesocket(srv_socket);
				closesocket(accept_socket);
				ExitWithError("Accept failed");
			}


			char buffer[1024];
			memset(buffer, 0, sizeof(buffer));
			int bytes = recv(accept_socket, buffer, sizeof(buffer) - 1, 0);
			if (bytes > 0)
			{
				std::cout << "=== HTTP Request ===\n" << buffer << "\n====================\n";
				std::string request(buffer);
				size_t first_line_end = request.find('\n');
				std::string first_line = request.substr(0, first_line_end);
				size_t method_end = first_line.find(' ');
				size_t path_end = first_line.find(' ', method_end + 1);
				std::string path = first_line.substr(method_end + 1, path_end - method_end - 1);
				srv_InMessage = std::string(buffer, bytes);
				
				std::string filePath = "www";
				if (path == "/") {
					filePath += "/index.html";
				}
				else {
					filePath += path;
				}
				std::string content = readFile(filePath);
				std::string response;
				if (content.empty())
				{
					response = 
						"HTTP/1.1 404 Not Found\r\n"
						"Content-Type: text/html\r\n"
						"Connection: close\r\n"
						"\r\n"
						"<h1>404 - File Not Found</h1>";
				}
				else
				{
					std::string ContentType = GetContentFile(filePath);
					std::string ContentLength = std::to_string(content.length());
					response =
						"HTTP/1.1 200 OK\r\n"
						"Content-Type: " + ContentType + "\r\n"
						"Content-Length: " + ContentLength + "\r\n"
						"Connection: close\r\n"
						"\r\n" +
						content;
					std::cout << "Serving: " << filePath << " as Content-Type: " << ContentType << std::endl;
				}
				send(accept_socket, response.c_str(),
					static_cast<int>(response.length()), 0);
			}

			else if (bytes == 0)
			{
				ExitWithError("Client disconnected.");
			}
			else
			{
				ExitWithError("recv() error!");
			}
		} while (true);
	}
	std::string TcpServer::readFile(const std::string& path)
	{
	std::ifstream file(path, std::ios::binary);
	if (!file.is_open()) {
		return "";
	}
	return std::string((std::istreambuf_iterator<char>(file)),
		std::istreambuf_iterator<char>());
	}
	std::string TcpServer::GetContentFile(const std::string& path)
	{
		if (path.ends_with(".html")) return "text/html";
		if (path.ends_with(".png")) return "image/png";
		if (path.ends_with(".ico")) return "image/x-icon";

		return "application/octet-stream";
	}
	void TcpServer::ServerClose()
	{
		closesocket(srv_socket);
		closesocket(accept_socket);
		WSACleanup();
	}
}