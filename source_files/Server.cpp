/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alsaeed <alsaeed@student.42abudhabi.ae>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/05 23:42:42 by alsaeed           #+#    #+#             */
/*   Updated: 2024/07/12 20:16:17 by alsaeed          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <Server.hpp>

bool signalInterrupt = false;

Server*	Server::getInstance(void) {

	if (!Server::_instance)
		Server::_instance = new Server();
	return Server::_instance;
}

void Server::initServer(void) {

	_listeningSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (_listeningSocket == -1) {

		throw IrcException("Can't create a socket!");
	}

	if ( fcntl( _listeningSocket, F_SETFL, O_NONBLOCK ) == -1 ) {
	
		throw IrcException("Can't set file descriptor flags");
	}

	_serverHint.sin_family = AF_INET;
	_serverHint.sin_addr.s_addr = INADDR_ANY;
	_serverHint.sin_port = htons(_serverPort);

	int opt = 1;
	if (setsockopt(_listeningSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) {
		
		close(_listeningSocket);
		throw IrcException("Can't set socket options");
	}

	if (bind(_listeningSocket, (struct sockaddr*)&_serverHint, sizeof(_serverHint)) == -1) {
		perror("bind");
		close(_listeningSocket);
		throw IrcException("Can't bind to IP/port");
	}

	if (listen(_listeningSocket, 1) == -1) {
		perror("listen");
		close(_listeningSocket);
		throw IrcException("Can't listen!");
	}

	_hintLen = sizeof(_serverHint);

	gethostname(_host, NI_MAXHOST);
	std::cout << "IRC server Listening on " << _host << " on port " << _serverPort << std::endl;
	std::cout << "Waiting for incoming connections..." << std::endl;
 
	pollfd listeningSocketPoll;
	memset(&listeningSocketPoll, 0, sizeof(listeningSocketPoll));
	listeningSocketPoll.fd = _listeningSocket;
	listeningSocketPoll.events = POLLIN;
	listeningSocketPoll.revents = 0;
	_fds.push_back( listeningSocketPoll );

	return;
}

void Server::signalHandler(int signal) {

	std::cerr << "Interrupt Signal (" << signal << ") received, Shutting down the Server..." << std::endl;
	signalInterrupt = true;

	return;
}

void    Server::runServer(void) {

	signal(SIGTSTP, signalHandler);
	signal(SIGINT, signalHandler);
	signal(SIGQUIT, signalHandler);

	while ( signalInterrupt == false ) {

		if ( poll((&_fds[0]), _fds.size(), 1000) == -1 ) {

			cleanupServer();
			perror("poll");
			throw IrcException("Poll error");
		}
		
		if ( _fds[0].revents & POLLIN )
		{
			handleNewConnection();
		}

		std::vector<pollfd>::iterator it = _fds.begin();
		while ( it != _fds.end() ) {

			if (it->fd != _listeningSocket && it->revents & POLLIN ) 
			{
				try
				{
					handleClientMessage(it->fd);	
				}
				catch(...)
				{
					it->fd = -1;
				}
				// for(std::size_t)
			} else if (it->fd != _listeningSocket && it->revents & POLLOUT ) {

				sendToClient( it->fd );
			}

			if ( it->fd == -1 ) {
			
				closeClient(it->fd);
			} else {
				
				++it;
			}
		}
	}

	cleanupServer();
	return;
}

void Server::sendToClient( int client_fd )
{
	Client* client = _clients[client_fd];
	std::vector<std::string>::iterator it = client->serverReplies.begin();

	for ( ; it != client->serverReplies.end(); ++it ) {

		std::cout << "............................................" << std::endl;
		std::cout << "Sending message to client " << client->getNickname() << ": " << *it << std::endl;
		std::cout << "............................................" << std::endl;
		if ( send( client_fd, it->c_str(), it->size(), 0 ) == -1 )
		{
			std::cerr << "Error sending message to client " << client->getNickname() << " (" << strerror(errno) << ")" << std::endl;
			return;
		}
	}

	client->serverReplies.clear();

	return;
}

void Server::handleNewConnection(void) {

	sockaddr_in clientHint;
	socklen_t clientSize = sizeof(clientHint);
	int clientSocket = accept(_listeningSocket, (sockaddr*)&clientHint, &clientSize);
	if (clientSocket == -1) {

		perror("accept");
		throw IrcException("Can't accept client connection");
	}

	int result = getnameinfo((sockaddr*)&clientHint, clientSize, _host, NI_MAXHOST, _svc, NI_MAXSERV, 0);
	if (result) {

		std::cout <<  _host << " connected on " << _svc << std::endl;
	} else {

		inet_ntop(AF_INET, &clientHint.sin_addr, _host, NI_MAXHOST);
		std::cout <<  _host << " connected on " << ntohs(clientHint.sin_port) << std::endl;
	}

	Client* tmpClient = new Client( clientSocket );
	_clients[clientSocket] = tmpClient;

	pollfd clientPoll;
	memset(&clientPoll, 0, sizeof(clientPoll));
	clientPoll.fd = clientSocket;
	clientPoll.events = POLLIN | POLLOUT;
	clientPoll.revents = 0;
	_fds.push_back( clientPoll );

	return;
}

int    Server::ft_recv( int fd ) {

	_message.clear();
	_message.resize(BUFFER_SIZE);
	int bytesRecv = recv(fd, &_message[0], BUFFER_SIZE, 0);
	if (bytesRecv <= 0) {
		return bytesRecv;
	}
	
	_message.resize(bytesRecv);

	return bytesRecv;
}

void Server::handleClientDisconnection(int client_fd, int bytesRecv) {

	if (bytesRecv == 0) {
		std::cout << "Client " << client_fd << " disconnected" << std::endl;
	} else {
		std::cerr << "Error receiving message from client " << client_fd << " (" << strerror(errno) << ")" << std::endl;
	}

	for (std::vector<pollfd>::iterator it = _fds.begin(); it != _fds.end(); ++it) {
		if (it->fd == client_fd) {
			it->fd = -1;
			break;
		}
	}

	return;
}

void Server::handleClientMessage( int client_fd )
{

	int bytesRecv = ft_recv( client_fd );
	
	if (bytesRecv <= 0) {

		handleClientDisconnection(client_fd, bytesRecv);
		return;
	}
	std::vector<std::string> commandList;
	if (_message.empty() || (_message[_message.size() - 1] != '\n' && (_message.size() >= 2 && _message.substr(_message.size() - 2) != "\r\n"))) {
		std::cerr << "Invalid message format from client " << client_fd << std::endl;
		_message.clear();
		return;
	}
	//split on newline using ft_split and return a vector of strings and loop through that and push to 
	std::cout << "Received message from client " << _clients[client_fd]->getNickname() << ": " << _message << std::endl;
	commandList = ft_split(_message, '\n');
	for(std::size_t i = 0; i < commandList.size(); i++)
	{
		ParseMessage parsedMsg(commandList[i]);
		processCommand( _clients[client_fd] , parsedMsg );
	}
	commandList.clear();
	return;
}

void Server::closeClient( int client_fd ) {

	std::map<int, Client*>::iterator it = _clients.find(client_fd);
	if (it != _clients.end()) {

		close(client_fd);
		delete it->second;
		_clients.erase(it);
	}

	// for (std::map<std::string, Channel>::iterator it = Server::_channels.begin(); it != Server::_channels.end(); ++it)
	// 	it->second.removeClient(client_fd);

	for ( std::vector<pollfd>::iterator it = _fds.begin(); it != _fds.end(); ) {

		if ( it->fd == client_fd ) {

			it = _fds.erase(it);
		} else {

			++it;
		}
	}
}

std::string Server::getServerPassword( void )
{
	return _serverPassword;
}
// void Server::authenticateClient(int client_fd, const std::string& password) {

// 	std::cout << "_password: " << Server::_serverPassword << std::endl;
// 	std::cout << "password: " << password << std::endl;
// 	if (password == Server::_serverPassword) {
// 		Server::_clients[client_fd]->setAuthenticated(true);
// 		std::cout << "Client " << client_fd << " authenticated" << std::endl;
// 	} else {
// 		Server::_clients[client_fd]->sendMessage("ERROR: Invalid password");
// 		std::cout << "Please enter the correct password" << std::endl;
// 	}
// }

void Server::cleanupServer(void) {

	std::cout << "Cleaning up server..." << std::endl;
	for (std::vector<pollfd>::iterator it = _fds.begin(); it != _fds.end(); ++it)
		close(it->fd);

	for (std::map<int, Client*>::iterator it = _clients.begin(); it != _clients.end(); ++it)
		delete it->second;

	shutdown(_listeningSocket, SHUT_RDWR);
	close(_listeningSocket);
	_fds.clear();
	_clients.clear();
	// Server::_channels.clear();
	delete Server::_instance;
	exit (0);
}

Server* Server::_instance = NULL;
// int Server::_listeningSocket = -1;
// std::string Server::_serverPassword = "";
// int Server::_serverPort = -1;
// struct sockaddr_in Server::_serverHint;
// int Server::_hintLen = 0;
// std::string Server::_message = "";
// char Server::_host[NI_MAXHOST];
// char Server::_svc[NI_MAXSERV];
// std::map<int, Client*> Server::_clients;
// std::map<std::string, Channel> Server::_channels;
// std::vector<pollfd> Server::_fds;
// std::map<std::string, Channel>	Server::_channels;
// std::vector<std::string>		Server::_nicknames;

// Path: includes/Server.hpp