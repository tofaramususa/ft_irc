# FT_IRC Project

## Overview
The FT_IRC project is an implementation of an Internet Relay Chat (IRC) server, following the RFC 1459 (Internet Relay Chat Protocol) specifications. This project is part of the curriculum at Ecole 42 and is designed to teach students about socket programming, network communication, and server-client architecture.

## Features
- Socket creation and management
- Client-server communication
- Channel operations (join, part, topic, etc.)
- User registration and authentication
- Message broadcasting and private messaging
- Server-side command parsing and processing
- Use of the Singleton design pattern for server instance management

## Concepts
The project covers several key concepts in network programming:
- **TCP/IP Sockets**: Establishing a connection-oriented communication channel between the server and clients.
- **Non-blocking I/O**: Using non-blocking sockets to handle multiple connections simultaneously.
- **Polling**: Monitoring multiple file descriptors to see if I/O is possible on any of them.
- **Signal Handling**: Managing server shutdown and cleanup through signal interrupts.
- **RFC Compliance**: Adhering to the IRC protocol specifications for message formats and command processing.

## Singleton Principle
The Singleton design pattern ensures that a class has only one instance and provides a global point of access to it. In the context of the FT_IRC project, the Singleton pattern is used for the server instance, ensuring that only one server is running and managing all client connections and channels.

Here is an example of how the Singleton pattern is implemented in the server:

```cpp
class Server {
private:
    static Server* _instance;
    // Other private members and methods

public:
    static Server* getInstance(void) {
        if (!_instance)
            _instance = new Server();
        return _instance;
    }
    // Other public members and methods
};

Server* Server::_instance = NULL;
```

## Getting Started
To run the FT_IRC server, compile the source code using the provided Makefile and execute the binary with the required arguments (port number and server password).

```bash
make
./irc_server <port> <password>
```

## Dependencies
- C++ Compiler with C++98 standard support
- POSIX compliant operating system
