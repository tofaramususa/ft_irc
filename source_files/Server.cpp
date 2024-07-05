/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tofaramususa <tofaramususa@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/05 23:42:42 by alsaeed           #+#    #+#             */
/*   Updated: 2024/06/26 20:11:11 by tofaramusus      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <Server.hpp>

bool signalInterrupt = false;

void Server::initServer(void) {

	Server::_listeningSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (Server::_listeningSocket == -1) {

		throw IrcException("Can't create a socket!");
	}

	if ( fcntl( Server::_listeningSocket, F_SETFL, O_NONBLOCK ) == -1 ) {
	
		throw IrcException("Can't set file descriptor flags");
	}

	Server::_serverHint.sin_family = AF_INET;
	Server::_serverHint.sin_addr.s_addr = INADDR_ANY;
	Server::_serverHint.sin_port = htons(Server::_serverPort);

	int opt = 1;
	if (setsockopt(Server::_listeningSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) {
		
		close(Server::_listeningSocket);
		throw IrcException("Can't set socket options");
	}

	if (bind(Server::_listeningSocket, (struct sockaddr*)&Server::_serverHint, sizeof(Server::_serverHint)) == -1) {
		perror("bind");
		close(Server::_listeningSocket);
		throw IrcException("Can't bind to IP/port");
	}

	if (listen(Server::_listeningSocket, 1) == -1) {
		perror("listen");
		close(Server::_listeningSocket);
		throw IrcException("Can't listen!");
	}

	Server::_hintLen = sizeof(Server::_serverHint);

	gethostname(Server::_host, NI_MAXHOST);
	std::cout << "IRC server Listening on " << Server::_host << " on port " << Server::_serverPort << std::endl;
	std::cout << "Waiting for incoming connections..." << std::endl;
 
	pollfd listeningSocketPoll;
	memset(&listeningSocketPoll, 0, sizeof(listeningSocketPoll));
	listeningSocketPoll.fd = Server::_listeningSocket;
	listeningSocketPoll.events = POLLIN;
	listeningSocketPoll.revents = 0;
	Server::_fds.push_back( listeningSocketPoll );

	return;
}


void Server::setNonblocking(int fd) {

	if (fcntl(fd, F_SETFL, O_NONBLOCK) == -1) {
		Server::cleanupServer();
		throw IrcException("Can't set file descriptor flags");
	}
}

void    Server::runServer(void) {

	signal(SIGTSTP, Server::signalHandler);
	signal(SIGINT, Server::signalHandler);

	while ( signalInterrupt == false ) {

		if ( poll((&Server::_fds[0]), Server::_fds.size(), 1000) == -1 ) {

			Server::cleanupServer();
			perror("poll");
			throw IrcException("Poll error");
		}
		
		if ( Server::_fds[0].revents & POLLIN )
		{
			handleNewConnection();
		}

		std::vector<pollfd>::iterator it = Server::_fds.begin();
		while ( it != Server::_fds.end() ) {

			if (it->fd != Server::_listeningSocket && it->revents & POLLIN ) 
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
			} else if (it->fd != Server::_listeningSocket && it->revents & POLLOUT ) {

				sendToClient( it->fd );
			}

			if ( it->fd == -1 ) {
			
				closeClient(it->fd);
			} else {
				
				++it;
			}
		}
	}

	Server::cleanupServer();
	return;
}

void Server::sendToClient( int client_fd )
{
	Client* client = Server::_clients[client_fd];
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
	int clientSocket = accept(Server::_listeningSocket, (sockaddr*)&clientHint, &clientSize);
	if (clientSocket == -1) {

		perror("accept");
		throw IrcException("Can't accept client connection");
	}

	int result = getnameinfo((sockaddr*)&clientHint, clientSize, Server::_host, NI_MAXHOST, Server::_svc, NI_MAXSERV, 0);
	if (result) {

		std::cout <<  Server::_host << " connected on " << Server::_svc << std::endl;
	} else {

		inet_ntop(AF_INET, &clientHint.sin_addr, Server::_host, NI_MAXHOST);
		std::cout <<  Server::_host << " connected on " << ntohs(clientHint.sin_port) << std::endl;
	}

	Client* tmpClient = new Client( clientSocket );
	Server::_clients[clientSocket] = tmpClient;

	pollfd clientPoll;
	memset(&clientPoll, 0, sizeof(clientPoll));
	clientPoll.fd = clientSocket;
	clientPoll.events = POLLIN | POLLOUT;
	clientPoll.revents = 0;
	Server::_fds.push_back( clientPoll );

	return;
}

int    Server::ft_recv( int fd ) {

	Server::_message.clear();
	Server::_message.resize(Server::BUFFER_SIZE);
	int bytesRecv = recv(fd, &Server::_message[0], Server::BUFFER_SIZE, 0);
	if (bytesRecv <= 0) {
		return bytesRecv;
	}
	
	Server::_message.resize(bytesRecv);

	return bytesRecv;
}

void Server::handleClientDisconnection(int client_fd, int bytesRecv) {

	if (bytesRecv == 0) {
		std::cout << "Client " << client_fd << " disconnected" << std::endl;
	} else {
		std::cerr << "Error receiving message from client " << client_fd << " (" << strerror(errno) << ")" << std::endl;
	}

	for (std::vector<pollfd>::iterator it = Server::_fds.begin(); it != Server::_fds.end(); ++it) {
		if (it->fd == client_fd) {
			it->fd = -1;
			break;
		}
	}

	return;
}

void Server::handleClientMessage( int client_fd )
{

	int bytesRecv = Server::ft_recv( client_fd );
	
	if (bytesRecv <= 0) {

		handleClientDisconnection(client_fd, bytesRecv);
		return;
	}
	std::vector<std::string> commandList;
	if (Server::_message.empty() || (Server::_message[Server::_message.size() - 1] != '\n' && (Server::_message.size() >= 2 && Server::_message.substr(Server::_message.size() - 2) != "\r\n"))) {
		std::cerr << "Invalid message format from client " << client_fd << std::endl;
		Server::_message.clear();
		return;
	}
	//split on newline using ft_split and return a vector of strings and loop through that and push to 
	std::cout << "Received message from client " << client_fd << ": " << Server::_message << std::endl;
	commandList = ft_split(Server::_message, '\n');
	for(std::size_t i = 0; i < commandList.size(); i++)
	{
		ParseMessage parsedMsg(commandList[i]);
		processCommand( _clients[client_fd] , parsedMsg );
		commandList[i].clear();
	}
	commandList.clear();
	return;
}

void Server::closeClient( int client_fd ) {

	std::map<int, Client*>::iterator it = Server::_clients.find(client_fd);
	if (it != Server::_clients.end()) {

		close(client_fd);
		delete it->second;
		Server::_clients.erase(it);
	}

	// for (std::map<std::string, Channel>::iterator it = Server::_channels.begin(); it != Server::_channels.end(); ++it)
	// 	it->second.removeClient(client_fd);

	for ( std::vector<pollfd>::iterator it = Server::_fds.begin(); it != Server::_fds.end(); ) {

		if ( it->fd == client_fd ) {

			it = Server::_fds.erase(it);
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

void Server::signalHandler(int signal) {

	std::cerr << "Interrupt Signal (" << signal << ") received, Shutting down the Server..." << std::endl;
	signalInterrupt = true;

	return;
}

void Server::cleanupServer(void) {

	std::cout << "Cleaning up server..." << std::endl;
	for (std::vector<pollfd>::iterator it = Server::_fds.begin(); it != Server::_fds.end(); ++it)
		close(it->fd);

	for (std::map<int, Client*>::iterator it = Server::_clients.begin(); it != Server::_clients.end(); ++it)
		delete it->second;

	shutdown(Server::_listeningSocket, SHUT_RDWR);
	close(Server::_listeningSocket);
	Server::_fds.clear();
	Server::_clients.clear();
	// Server::_channels.clear();
	
	exit (0);
}

int Server::_listeningSocket = -1;
std::string Server::_serverPassword = "";
int Server::_serverPort = -1;
struct sockaddr_in Server::_serverHint;
int Server::_hintLen = 0;
std::string Server::_message = "";
char Server::_host[NI_MAXHOST];
char Server::_svc[NI_MAXSERV];
std::map<int, Client*> Server::_clients;
// std::map<std::string, Channel> Server::_channels;
std::vector<pollfd> Server::_fds;
std::map<std::string, Channel>	Server::_channels;
std::vector<std::string>			Server::_nicknames;

// Path: includes/Server.hpp