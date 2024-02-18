# TCP Chat in C++

This is a TCP chat application in C++ that enables communication between a server and multiple clients.

---

## Features

- **TCP Server**: Allows configuring a server to listen on specific IP and port. The server administrator can set the room name.
- **TCP Client**: Clients can connect to the server by specifying the IP and port. They have the option to choose a username when entering the chat room.

---

## How to Use

### Compile the Code

Make sure you have a C++ compiler installed on your system to compile the code.

### Run the Server

To start the server, after compilation, execute the server.exe.

The server will prompt for the IP, port, and room name. After providing this information, the server will begin waiting for client connections.

### Run the Client

To start a client, execute the client.exe after compiling it.

The client will prompt for the server's IP and port. After providing this information, you will be prompted to enter a username. Once connected, you can send messages to the chat room.

---

### Example Usage

Running the server:
```bash
[+] Room Internet Protocol: 127.0.0.1
[+] Room Port: 12345
[+] Room Name (max 20): IDK

[-] 127.0.0.1:12345 | Room Name: IDK

[*] Server Created!

Executar o cliente:
```

```bash
[-] Internet Procotol: 127.0.0.1
[-] Server Port: 12345
[-] Username (max 12): IDK

[*] 127.0.0.1:12345

[+] Host Found !
```

---

## Author
By: Fxrgtt

*Obs:* Using color lib from another person.
