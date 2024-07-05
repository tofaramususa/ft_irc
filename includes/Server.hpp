/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tofaramususa <tofaramususa@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/05 23:42:39 by alsaeed           #+#    #+#             */
/*   Updated: 2024/07/03 16:22:01 by tofaramusus      ###   ########.fr       */
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

		static const int						MAX_CLIENTS = FD_SETSIZE;
		static const int						BUFFER_SIZE = 1024;

		static int								_listeningSocket;
		static std::string						_serverPassword;
		static int								_serverPort;
		static std::string						_message;
		static struct sockaddr_in				_serverHint;
		static int								_hintLen;
		static char								_host[NI_MAXHOST];
		static char								_svc[NI_MAXSERV];
		static std::map<int, Client*>			_clients;
		static std::map<std::string, Channel>	_channels;
		static std::vector<std::string>			_nicknames;
		
		static std::vector<pollfd>				_fds;

		Server( void );

		static void             setNonblocking(int fd);
		static void             handleNewConnection(void);
		static int     			ft_recv( int fd );
		static void             cleanupServer(void);

		//CLIENT FUNCTIONS
		static void				handleClientDisconnection(int client_fd, int bytesRecv);
		static void             handleClientMessage(int client_fd);
		static void             closeClient(int client_fd);
		static void				sendToClient( int client_fd );
		static void				connectUser( Client* client, const ParseMessage& parsedMsg );
		static	void			addNewUser(Client* client, const ParseMessage &parsedMsg);
		static Client					*getClient(std::string nickname);

		// Commands
		static void				quitCommand(std::string reason, Client *client);
		static void				nickCommand(Client *client, const std::vector<std::string> &params);
		static void				processCommand( Client *client, const ParseMessage& parsedMsg);
		static void 			joinCommand(Client *client, const ParseMessage& parsedMsg);
		static void 			privateMessage(Client *client, const ParseMessage &ParsedMsg);
		static void 			modeCommand(Client *client, const ParseMessage& parsedMsg);
		static void 			inviteCommand(Client *client, const ParseMessage& parsedMsg);
		static void 			topicCommand(Client *client, const ParseMessage& parsedMsg);
		static void 			partCommand(Client *client, const ParseMessage& parsedMsg);
		static void 			kickCommand(Client *client, const ParseMessage& parsedMsg);
		static void 			motdCommand(Client *client);
		static void 			noticeCommand(Client *client, const ParseMessage& parsedMsg);

		static void				handleCapCommand(Client *client, const std::vector<std::string> &params);
		static bool 			handlePassCommand(Client *client, const std::vector<std::string> &params);
		

	public:

		//Channels
		static void 			addChannel(Channel &channel);
		static Channel 	 		&getChannel(std::string channelName);
		static bool				isChannelInServer(std::string &channelName);

		static void 			initServer(void);
		static void 			runServer(void);
		static void 			signalHandler(int signal);

		static void 			setServerPassword(const std::string& password) { _serverPassword = password; };
		static void 			setServerPort(int port) { _serverPort = port; };
		static std::string		getServerPassword( void );
		static bool				isValidIRCCommand(const std::string& command);
		static void 			printCommand(ParseMessage message);
		static bool					isUserInServer(std::string nickname);
};

std::vector<std::string>  ft_split(std::string str, char delimiter);
std::vector<std::string> remove_spaces(std::string &str);



#endif /* SERVER_HPP */