#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <vector>
#include <string>
#include <thread>
#include <iomanip>
#include <stdio.h>
#include <iostream>
#include <Winsock2.h>
#include <Ws2tcpip.h>
#include "console-color.hpp"

#pragma comment(lib, "Ws2_32.lib")

using namespace cs;

// Struct que serve pra guardar as infos dos clientes
struct client {
    SOCKET socket;
    sockaddr_in addr;
};

// Função de criação do chat, limite de characters etc...
// Recv = função que recebe oque o cliente enviou para o servidor
// Buffer = Criação do limite de characters que pode ser enviado
// Sizeof(buffer), argumento que pega o tamanho do Buffer
// Send = Pega a mensagem e envia pros outros clientes
void chat(SOCKET temp_socket, std::vector<client> *clients, size_t position) {
    
    int bytesReceived;
    
    do {
        char buffer[12 + 1 + 450] = { 0 };
        bytesReceived = recv(temp_socket, buffer, sizeof(buffer), 0);

        for (int i = 0; i < clients->size(); i++) {
            send(clients->at(i).socket, buffer, sizeof(buffer), 0);
        }
    } while (bytesReceived > 0);
    closesocket(temp_socket);
    clients->erase(clients->begin() + position);

};

int main() {
    // Informações de criação do server
    std::string ip;
    std::string port;
    std::string room;

    std::string confirmation;

    // Loop das strings principais
    while (true) {
        std::cout << (("[+] "_cs << color(122, 140, 137)) << ("Room Internet Procotol: "_cs << color(122, 140, 137)));
        std::getline(std::cin, ip);

        std::cout << (("[+] "_cs << color(122, 140, 137)) << ("Room Port: "_cs << color(122, 140, 137)));
        std::getline(std::cin, port);

        // Limite de characters do Room Name
        while (true) {
            std::cout << (("[+] "_cs << color(122, 140, 137)) << ("Room Name (Max 20): "_cs << color(122, 140, 137)));
            std::getline(std::cin, room);

            if (room.length() > 20) {
                std::cerr << (("[x] "_cs << color(240, 105, 105)) << ("Username can't be larger than 12 characters.\n"_cs << color(240, 105, 105)));
            } else {
                break;
            }
        }

        // Loop de confirmação
        while (true) {
            std::cout << (("[?] "_cs << color(194, 168, 145)) << ("Confirm? (Y/N) "_cs << color(194, 168, 145)));
            std::getline(std::cin, confirmation);
            for (int i = 0; i < confirmation.length(); i++) {
                confirmation[i] = tolower(confirmation[i]);
            }

            if (confirmation == "y" || confirmation == "yes") {
                goto exit;
            } else if (confirmation == "n" || confirmation == "no") {
                break;
            } else {

                std::cerr << (("[x] "_cs << color(240, 105, 105)) << ("Please Provide A Valid Response. \n"_cs << color(240, 105, 105)));
            }
        }
    }

    // Label usado pra sair do loop acima ^
    exit:

    // Resultado de entrada
    std::cout << (("[-] "_cs << color(107, 181, 150)) << (color_string(ip) << color(107, 181, 150))) << ":";
    std::cout << (color_string(port) << color(107, 181, 150)) << " | ";
    std::cout << "Room Name: " << room << '\n';

    // Windows DLL Call permissão
    WSADATA wsaData;

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << (("[x] "_cs << color(240, 105, 105)) << ("WSAStartup failed \n"_cs << color(240, 105, 105)));
        return 1;
    }

    // Tipos de conexões aceitas e verificadas
    SOCKET connection = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (connection == INVALID_SOCKET) {
        std::cerr << (("[x] "_cs << color(240, 105, 105)) << ("Socket creation failed \n"_cs << color(240, 105, 105)));
        WSACleanup();
        return 1;
    }

    // Convertendo uma string para um valor numérico
    unsigned short iport = (unsigned short)strtoul(port.c_str(), NULL, 0);

    // Criação de porta para receber e enviar informações
    sockaddr_in serverport;
    serverport.sin_family = AF_INET;
    serverport.sin_port = htons(iport);
    serverport.sin_addr.s_addr = inet_addr(ip.c_str());

    // Responsavel por criar a porta do servidor.
    if (bind(connection, (struct sockaddr*)&serverport, sizeof(serverport)) == SOCKET_ERROR) {
        std::cerr << (("[x] "_cs << color(240, 105, 105)) << ("Bind Failed. \n"_cs << color(240, 105, 105)));

        if (closesocket(connection) == SOCKET_ERROR) {
            std::cerr << (("[x] "_cs << color(240, 105, 105)) << ("closesocket function failed \n"_cs << color(240, 105, 105)));
        }

        WSACleanup();
        return 1;
    }

    // Função que abre a porta
    if (listen(connection, 10) == SOCKET_ERROR) {
        std::cerr << (("[x] "_cs << color(240, 105, 105)) << ("Connection queue limit exceded. \n"_cs << color(240, 105, 105)));

        if (closesocket(connection) == SOCKET_ERROR) {
            std::cerr << (("[x] "_cs << color(240, 105, 105)) << ("closesocket function failed \n"_cs << color(240, 105, 105)));
        }

        WSACleanup();
        return 1;
    }

    std::cout << (("[*] "_cs << color(115, 173, 245)) << ("Server Created ! \n "_cs << color(115, 173, 245)));

    // Cria o vector de clientes, responsavel por guardar tudo na struct clients
    // É aonde guarda todos os clientes
    std::vector<client> clients;

    sockaddr_in temp_addr;
    SOCKET temp_socket;

    int temp_addr_len = sizeof(temp_addr);

    while (true) {

        // Socket de cnnection guarda no servidor a entrada do cliente.
        // temp_addr guarda porta\ip que o cliente usou dentro do server.
        temp_socket = accept(connection, (sockaddr*)&temp_addr, &temp_addr_len);

        if (temp_socket == INVALID_SOCKET) {
            std::cerr << (("[x] "_cs << color(240, 105, 105)) << ("Client socket creation failed \n"_cs << color(240, 105, 105)));

            if (closesocket(connection) == SOCKET_ERROR) {
                std::cerr << (("[x] "_cs << color(240, 105, 105)) << ("closesocket function failed \n"_cs << color(240, 105, 105)));
            }

            WSACleanup();
            return 1;
        }

        // Estruturas que guardam infos das conexões dos usuarios
        client c;
        c.socket = temp_socket;
        c.addr = temp_addr;

        clients.push_back(c);

        // Função thread de comunicação com a função chat
        std::thread f(chat, temp_socket,&clients, clients.size()-1);

        f.detach();
    }

    return 0;
}