#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <string>
#include <thread>
#include <stdio.h> 
#include <fcntl.h>
#include <iomanip>
#include <cstdlib>
#include <iostream>
#include <Winsock2.h>
#include <Ws2tcpip.h>
#include "console-color.hpp"

#pragma comment(lib, "Ws2_32.lib")

using namespace cs;

void recvfunc(SOCKET connection) {

		char buffer[12 + 1 + 450];

		std::string buffer_string;

		while (true) {

		int bytesReceived = recv(connection, buffer, sizeof(buffer), 0);
		buffer_string = buffer;

		if (bytesReceived > 0) {
			std::cout << "[#] " << buffer_string.substr(0, buffer_string.find("\x7F")) << ": " << buffer_string.substr(buffer_string.find("\x7F") + 1) << '\n';
		}
		else if (bytesReceived == 0) {
			std::cout << "Server closed. \n";

			if (closesocket(connection) == SOCKET_ERROR) {
				std::cerr << (("[x] "_cs << color(245, 44, 44)) << ("closesocket function failed \n"_cs << color(240, 105, 105)));
			}

			WSACleanup();
			exit(EXIT_FAILURE);
		}
		else {
			std::cerr << (("[x] "_cs << color(240, 105, 105)) << ("Server Error \n"_cs << color(240, 105, 105)));

			if (closesocket(connection) == SOCKET_ERROR) {
				std::cerr << (("[x] "_cs << color(240, 105, 105)) << ("closesocket function failed \n"_cs << color(240, 105, 105)));
			}
			WSACleanup();
			exit(EXIT_FAILURE);
		}

	}

}

int main() {
	// Entrada
	std::string ip;
	std::string port;
	std::string username;
	std::string confirmation;

	while (true) {
		std::cout << (("[-] "_cs << color(115, 173, 245)) << ("Internet Procotol: "_cs << color(115, 173, 245)));
		std::getline(std::cin, ip);

		std::cout << (("[-] "_cs << color(115, 173, 245)) << ("Server Port: "_cs << color(115, 173, 245)));
		std::getline(std::cin, port);

		while (true) {
			std::cout << (("[-] "_cs << color(115, 173, 245)) << ("Username(Max 12): "_cs << color(115, 173, 245)));
			std::getline(std::cin, username);

			if (username.length() >= 12) {
				std::cerr << (("[x] "_cs << color(240, 105, 105)) << ("Username can't be larger than 12 characters.\n"_cs << color(240, 105, 105)));
			}
			else {
				break;
			}
		}

		while (true) {

			// Verifica a confirmação do usuário
			std::cout << (("[?] "_cs << color(194, 168, 145)) << ("Confirm? (Y/N) "_cs << color(194, 168, 145)));
			std::getline(std::cin, confirmation);
			for (int i = 0; i < confirmation.length(); i++) {
				confirmation[i] = tolower(confirmation[i]);
			}

			if (confirmation == "y" || confirmation == "yes") {
				goto exit;

			}
			else if (confirmation == "n" || confirmation == "no") {
				break;
			}
			else {
				std::cerr << (("[x] "_cs << color(240, 105, 105)) << ("Please Provide A Valid Response. \n"_cs << color(240, 105, 105)));
			}
		}
	}

	// Label "exit", usado para escapar de todos os loops acima
exit:

	// Resultado de entrada
	std::cout << (("[*] "_cs << color(107, 181, 150)) << (color_string(ip) << color(107, 181, 150))) << ":";
	std::cout << (color_string(port) << color(107, 181, 150)) << '\n';

	//Carrega a DLL "Windows Sockets", que carrega as funções : socket, getaddrinfo, WSAGetLastError, usadas abaixo
	WSADATA wsaData;

	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
		std::cerr << (("[x] "_cs << color(240, 105, 105)) << ("WSAStartup failed \n"_cs << color(240, 105, 105)));
		return 1;
	}

	//Socket, guarda informações da conexão(IPv4, tipo, protocolo) em memória(RAM)
	SOCKET connection = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (connection == INVALID_SOCKET) {
		std::cerr << (("[x] "_cs << color(240, 105, 105)) << ("Socket creation failed \n"_cs << color(240, 105, 105)));
		WSACleanup();
		return 1;
	}

	// Testa o host da entrada do usuário, providenciando o "ip" e "port" em formato de C string (.c_str())
	addrinfo* get;
	int test = getaddrinfo(ip.c_str(), port.c_str(), NULL, &get);

	// Verificação de host
	if (WSAGetLastError() == WSAHOST_NOT_FOUND || test != 0) {
		std::cerr << (("[x] "_cs << color(240, 105, 105)) << ("Host Error. \n"_cs << color(240, 105, 105)));
		if (closesocket(connection) == SOCKET_ERROR) {
			std::cerr << (("[x] "_cs << color(240, 105, 105)) << ("closesocket function failed \n"_cs << color(240, 105, 105)));
		}

		WSACleanup();
		return 1;
	}
	else {
		std::cout << (("[+] "_cs << color(205, 107, 237)) << ("Host Found ! \n"_cs << color(205, 107, 237)));
	}

	unsigned short iport = (unsigned short)strtoul(port.c_str(), NULL, 0);

	sockaddr_in server;
	server.sin_family = AF_INET;
	server.sin_port = htons(iport);
	server.sin_addr.s_addr = inet_addr(ip.c_str());

	int teste = connect(connection, (SOCKADDR*)&server, sizeof(server));

	if (teste == SOCKET_ERROR) {
		std::cerr << (("[x] "_cs << color(240, 105, 105)) << ("Connection failed. \n"_cs << color(240, 105, 105)));

		if (closesocket(connection) == SOCKET_ERROR) {
			std::cerr << (("[x] "_cs << color(240, 105, 105)) << ("closesocket function failed \n"_cs << color(240, 105, 105)));
		}

		WSACleanup();
		return 1;
	}

	std::string message;
	std::string ncm;

	std::thread r(recvfunc, connection);

	r.detach();

	while (true) {

		std::getline(std::cin, message);
		if (message.length() > 450) {
			std::cerr << (("[x] "_cs << color(240, 105, 105)) << ("Messages can't be larger than 450 characters. \n"_cs << color(240, 105, 105)));
		}

		ncm = std::string(username + char(127) + message);
		send(connection, ncm.c_str(), ncm.length(), 0);
	}

	return 0;
}