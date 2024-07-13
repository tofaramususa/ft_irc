/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tofaramususa <tofaramususa@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/05 23:42:39 by alsaeed           #+#    #+#             */
/*   Updated: 2024/07/12 14:27:14 by tofaramusus      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#ifndef SERVER_HPP
#define SERVER_HPP

#include <IrcLibs.hpp>
#include <Reply.hpp>
#include <IrcException.hpp>
#include <ParseMessage.hpp>
#include <Client.hpp>
#include <Channel.hpp>

#include <map>
#include <vector>
#include <fstream>


class Server {

	private:

		static const int				MAX_CLIENTS = FD_SETSIZE;
		static const int				BUFFER_SIZE = 1024;

		int								_listeningSocket;
		std::string						_serverPassword;
		int								_serverPort;
		std::string						_message;
		struct sockaddr_in				_serverHint;
		int								_hintLen;
		char							_host[NI_MAXHOST];
		char							_svc[NI_MAXSERV];
		std::map<int, Client*>			_clients;
		std::map<std::string, Channel>	_channels;
		std::vector<std::string>		_nicknames;

		std::vector<pollfd>				_fds;

		static Server*					_instance;

		Server( void ) {}

		void            handleNewConnection(void);
		int     		ft_recv( int fd );
		void            cleanupServer(void);
		void 			displayCommand(  const ParseMessage &parsedMessage ) const;

		//CLIENT FUNCTIONS
		void			handleClientDisconnection(int client_fd, int bytesRecv);
		void            handleClientMessage(int client_fd);
		void            closeClient(int client_fd);
		void			sendToClient( int client_fd );
		void			connectUser( Client* client, const ParseMessage& parsedMsg );
		static	void	addNewUser(Client* client, const ParseMessage &parsedMsg);
		Client			*getClient(std::string nickname);

		// Commands
		void			quitCommand(std::string reason, Client *client);
		void			nickCommand(Client *client, const std::vector<std::string> &params);
		void			processCommand( Client *client, const ParseMessage& parsedMsg);
		void 			joinCommand(Client *client, const ParseMessage& parsedMsg);
		void 			privateMessage(Client *client, const ParseMessage &ParsedMsg);
		void 			modeCommand(Client *client, const ParseMessage& parsedMsg);
		void 			inviteCommand(Client *client, const ParseMessage& parsedMsg);
		void 			topicCommand(Client *client, const ParseMessage& parsedMsg);
		void 			partCommand(Client *client, const ParseMessage& parsedMsg);
		void 			kickCommand(Client *client, const ParseMessage& parsedMsg);
		void 			motdCommand(Client *client);
		void 			noticeCommand(Client *client, const ParseMessage& parsedMsg);

		void			handleCapCommand(Client *client, const std::vector<std::string> &params);
		bool 			handlePassCommand(Client *client, const std::vector<std::string> &params);
		

	public:

		//Channels
		void 			addChannel(Channel &channel);
		Channel&	getChannel(std::string channelName);
		bool		isChannelInServer(std::string &channelName);
		bool handleKeyMode(Client *client, Channel &channel, bool isAdding,
	  std::vector<std::string> &params, std::size_t &paramIndex);
		bool handleLimitMode(Client *client, Channel &channel, bool isAdding,
	  std::vector<std::string> &params, std::size_t &paramIndex);
		bool handleOperatorMode(Client *client, Channel &channel, bool isAdding,
	  std::vector<std::string> &params, std::size_t &paramIndex);
		bool processSingleChannelMode(Client *client, Channel &channel,
	char mode, bool isAdding,   std::vector<std::string> &params,
	std::size_t &paramIndex);
		void processChannelModes(Client *client, Channel &channel,
	  std::vector<std::string> &params);
		void handleChannelMode(Client *client, std::string &channelName,
	  std::vector<std::string> &params);
	  
		std::string		greetJoinedUser(Client &client, Channel &channel);

		//Server
		static Server*	getInstance(void);

		void 			initServer(void);
		void 			runServer(void);
		static void 			signalHandler(int signal);

		void 			setServerPassword(const std::string& password) { _serverPassword = password; };
		void 			setServerPort(int port) { _serverPort = port; };
		std::string		getServerPassword( void );
		bool			isValidIRCCommand(const std::string& command);
		bool			isUserInServer(std::string nickname);
		bool			isAlphanumeric(const std::string &str);
};

std::vector<std::string>  ft_split(std::string str, char delimiter);
std::vector<std::string> remove_spaces(std::string &str);



#endif /* SERVER_HPP */