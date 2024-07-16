/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Commands.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alsaeed <alsaeed@student.42abudhabi.ae>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/09 07:48:18 by alsaeed           #+#    #+#             */
/*   Updated: 2024/07/15 16:44:46 by alsaeed          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <Channel.hpp>
#include <Server.hpp>

void Server::addNewUser(Client* client, const ParseMessage &parsedMsg)
{
	std::vector<std::string> params = parsedMsg.getParams();
	
    if (client->getUsername().empty() == true && !params.empty())
    {
        client->setUsername(params[0]);
        std::cout << "User registered: " << client->getUsername() 
                  << " (Real name: " << client->getUsername() << ")" << std::endl;
    }
}

void Server::connectUser(Client *client, const ParseMessage &parsedMsg) 
{
    const std::string &command = parsedMsg.getCmd();
    const std::vector<std::string> &params = parsedMsg.getParams();

    if (command == "CAP")
	{
        handleCapCommand(client, params);
    }
    else if (client->conRegi[0] == true && command == "PASS") {

        handlePassCommand(client, params);
		if (client->getIsCorrectPassword() == true) {
			client->conRegi[1] = true;
		}
    }
	else if (client->conRegi[1] == true && (command == "USER" || command == "NICK"))
	{
		if (command == "USER") {

			addNewUser(client, parsedMsg);
		}
		else if (command == "NICK") {

			nickCommand(client, params);
		}

		if (client->getUsername() != "" && client->getNickname() != "") {

			client->conRegi[2] = true;
		}
	}
	if ( client->isRegistered == true && client->conRegi[2] == true )
	{
		motdCommand(client);
	}
    return ;
}

void Server::handleCapCommand(Client *client, const std::vector<std::string> &params) 
{
    if (params.size() > 0 && params[0] == "LS") {

		client->conRegi[0] = true;
        client->serverReplies.push_back(":irssi CAP * LS :  \r\n");
    } else if ( client->conRegi[0] == true ) {

		if (params.size() == 1 && params[0] == "REQ") {

        	client->serverReplies.push_back(":irssi CAP * REQ:  \r\n");
		} else if (params.size() == 1 && params[0] == "NAK" ) {

			client->serverReplies.push_back(":irssi CAP * NAK:  \r\n");
		} else if (params.size() == 1 && params[0] == "ACK" ) {

			client->serverReplies.push_back(":irssi CAP * ACK:  \r\n");
		} else if (params.size() == 1 && params[0] == "END") {

			client->isRegistered = true;
		}
	}
}

bool Server::handlePassCommand(Client *client, const std::vector<std::string> &params) {
    if (client->getIsCorrectPassword() == false) 
	{
        if (!params.empty() && params[0] == getServerPassword())
		{
            client->setIsCorrectPassword(true);
        } else 
		{
            client->serverReplies.push_back(ERR_PASSWDMISMATCH(std::string("ircserver")));
        }
    } else {
        client->serverReplies.push_back(ERR_ALREADYREGISTERED(std::string("ircserver")));
    }
    return true;
}

bool Server::isValidIRCCommand(const std::string& command) 
{
    static const char* validCommands[] = {
        "JOIN", "MODE", "TOPIC", "NICK", "QUIT", "PRIVMSG", "KICK",
        "INVITE", "PING", "motd", "CAP", "PASS", "USER", "PART", "WHO", "NOTICE", "WHOIS", 0
    };

    for (const char** cmd = validCommands; *cmd; ++cmd) {
        if (command == *cmd) 
			return true;
    }
    return false;
}

void	Server::displayCommand(  const ParseMessage &parsedMessage ) const {
	std::cout << "Command: " << parsedMessage.getCmd() << std::endl;
	std::cout << "Params: ";
	for ( int i = 0; i < static_cast<int>(parsedMessage.getParams().size()); i++ ) {
		
		std::cout << "Parameter " << "[" << i << "]: " << parsedMessage.getParams()[i] << std::endl;
	}
	std::cout << std::endl;
	if(!parsedMessage.getTrailing().empty())
		std::cout << "Trailing: " << parsedMessage.getTrailing() << std::endl;

	return;
}

void Server::processCommand(Client *client, const ParseMessage &parsedMsg)
{
	std::string command;
	
	std::vector<std::string> params;
	if(parsedMsg.getCmd().empty() == true) //check if it is empty 
	{
		return;
	}
	displayCommand(parsedMsg);
	command = parsedMsg.getCmd();
	params = parsedMsg.getParams();
	if(params.size() < 1 && parsedMsg.getTrailing().empty() == true && command != "QUIT" && command != "motd")
	{
		 client->serverReplies.push_back(ERR_NEEDMOREPARAMS(std::string("ircserver") ,command));
		 return;
	}
	if(isValidIRCCommand(parsedMsg.getCmd()) == false)
	{
		client->serverReplies.push_back(ERR_UNKNOWNCOMMAND(std::string("ircserver"), parsedMsg.getCmd()));
		return;
	}
	if (command == "QUIT")
		quitCommand(parsedMsg.getTrailing(), client);
	if( client->isRegistered == false || client->conRegi[2] == false )
	{
		connectUser(client, parsedMsg);	
	}
	else if ( client->isRegistered == true && client->conRegi[2] == true )
	{
		if (command == "USER" || command == "PASS")
		{
            client->serverReplies.push_back(ERR_ALREADYREGISTERED(std::string("ircserver")));
        }
		else if (command == "JOIN")
		{
			joinCommand(client, parsedMsg);
		}
		else if(command == "PRIVMSG")
		{
			privateMessage(client, parsedMsg);	
		}
		else if(command == "PING" && params.size() > 0)
		{
			 client->serverReplies.push_back(RPL_PONG(user_id(client->getNickname(),client->getUsername()),params[0]));
		}
		else if(command == "NICK")
		{
			nickCommand(client, params);
		}
		else if (command == "MODE")
		{
			modeCommand(client, parsedMsg);
		}
		else if(command == "TOPIC")
		{
			topicCommand(client, parsedMsg);
		}
		else if(command == "KICK")
		{
			kickCommand(client, parsedMsg);
		}
		else if(command == "INVITE")
		{
			inviteCommand(client, parsedMsg);
		}
		else if(command == "motd")
		{
			motdCommand(client);
		}
		else if(command == "PART")
		{
			partCommand(client, parsedMsg);
		}
		else if(command == "NOTICE")
		{
			noticeCommand(client, parsedMsg);
		}
	}
	return;
}