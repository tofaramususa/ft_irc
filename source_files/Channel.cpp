/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tofaramususa <tofaramususa@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/08 11:50:49 by alsaeed           #+#    #+#             */
/*   Updated: 2024/07/11 19:38:14 by tofaramusus      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <Server.hpp>
#include <Channel.hpp>

//CONSTRUCTOR
Channel::Channel(std::string &channelName, Client *client) : channelName(channelName), UserLimit(0)
{
	operators[client->getNickname()] = client;
	users[client->getNickname()] = client;
	modes['i'] = false;
	modes['t'] = false;
	modes['k'] = false;
	modes['o'] = false;
	modes['l'] = false;
	setMode('t', true);
}

Channel::~Channel(){}

//GETTER
Channel &Server::getChannel(std::string channelName)
{
	std::map<std::string, Channel>::iterator chan = _channels.find(channelName);
	return chan->second;
}

bool	Server::isChannelInServer(std::string &channelName)
{
	std::map<std::string, Channel>::iterator chan = _channels.find(channelName);
	if(chan != _channels.end())
	{
		return (true);
	}
	return (false);
}
void Channel::addClient(Client *client)
{
	std::string nick = client->getNickname();
	users[nick] = client;
	if(isInInvite(nick))
	{
		inviteList.erase(nick);
	}
	if(operators.size() == 0)
	{
		operators[nick] = client;
	}
}

void Channel::setTopic(std::string &topic)
{
		_topic = topic;
		this->setMode('t', true);
}


bool Channel::isClientInChannel(std::string nickname)
{
	std::map<std::string, Client *>::iterator user_itr = this->users.find(nickname);
	if(user_itr != this->users.end())
	{
		return true;
	}
	return false;
}

bool Channel::isInInvite(std::string nickname)
{
	if (this->inviteList.find(nickname) != inviteList.end())
	{
		return true;
	}
	return false;
}


std::map<std::string, Client *> Channel::getUsers( void ) const
{
	return this->users;
}

bool Channel::checkMode(char c)
{
	if(this->modes.find(c) != this->modes.end())
	{
		return this->modes.find(c)->second;
	}
	return false;
}

bool Channel::isOperator(std::string &nickname)
{
	
	if(this->operators.find(nickname) != operators.end())
	{
		return true;
	}
	return false;
}

void Channel::inviteClient(Client *client)
{
	std::string nick = client->getNickname();
	this->inviteList[nick] = client;
}

void Channel::setKey(std::string &password)
{
	this->_key = password;
	this->setMode('k', true);
}

std::string Channel::getKey() const
{
	return this->_key;
}

int Channel::getUserLimit() const
{
	return UserLimit;
}

std::string Channel::getModes() const
{
    std::string result = "";

    for (std::map<char, bool>::const_iterator it = modes.begin(); it != modes.end(); ++it) {
        if (it->second) {
            result += it->first;
        }
    }
    return result.empty() ? "" : "+" + result;
}

std::map<char, bool> Channel::getModesMap( void ) const {

	return modes;
}


void Channel::broadcastMessage(const std::string message)
{
    std::map<std::string, Client *>::iterator it;
    for (it = users.begin(); it != users.end(); ++it)
    {
        if (it->second->getFd() != -1)
        {
			it->second->serverReplies.push_back(message);
        }
    }
}

void Channel::sendToOthers(Client *client, std::string message)
{
    std::map<std::string, Client *>::iterator it;
    for (it = users.begin(); it != users.end(); ++it)
    {
        if (it->second->getFd() != -1 && it->second != client)
        {
            it->second->serverReplies.push_back(message);
        }
    }
}

bool Channel::setMode(char c, bool setting)
{
	// in case the setting is different from the previous one 
	std::map<char, bool>::iterator itr = modes.find(c);
	
	if ( setting == itr->second ) {

		return (false);
	}
		
	if(itr != modes.end())
	{
		itr->second = setting;
	}
	
	return (true);
}

std::string Channel::getChannelName() const
{
	return channelName;
}

void Channel::removeKey()
{
	_key.clear();
	this->setMode('k', false);
}

void Channel::setUserLimit(int limit)
{
	if(limit > 0)
	{
		UserLimit = limit;
		this->setMode('l', true);
	}
	else
		std::cout << "Invalid user limit inputted by the user" << std::endl;
}

void Channel::removeUserLimit()
{
	UserLimit = -1;
	this->setMode('l', false);
}

void Channel::addOperator(std::string nickname)
{
	std::map<std::string, Client *>::iterator user_itr = this->users.find(nickname);
	
	if (user_itr != this->users.end())
	{
		this->operators[user_itr->second->getNickname()] = user_itr->second;
		this->setMode('o', true);
	}
}

void Channel::removeOperator(std::string nickname)
{
	std::map<std::string, Client*>::iterator operator_itr = this->operators.find(nickname);
    if (operator_itr != this->operators.end())
	{
        this->operators.erase(operator_itr);
		this->setMode('o', false);
    }
	if(this->operators.empty() && !users.empty())
	{
		operators[users.begin()->first] = users.begin()->second;
	}
}

std::string Channel::getTopic() const
{
		return _topic;
}

void Channel::removeInvite(std::string &invite)
{
	std::map<std::string, Client*>::iterator invite_itr = this->inviteList.find(invite);
    if (invite_itr != this->inviteList.end())
	{
        this->inviteList.erase(invite_itr);
    }
}

void Channel::removeClient(Client *client)
{
	this->removeOperator(client->getNickname());
	std::map<std::string, Client*>::iterator users_itr = this->users.find(client->getNickname());
    if (users_itr != this->users.end())
	{
        this->users.erase(users_itr);
    }
}

std::string ft_trim(std::string text)
{
    size_t first = text.find_first_not_of(" \n\r\t");
    size_t last = text.find_last_not_of(" \n\r\t");

    if (first == std::string::npos || last == std::string::npos) {
        return "";
    }
    return text.substr(first, (last - first + 1));
}

void	Server::addChannel(Channel &channel)
{
	// _channels[channel.getChannelName()] = channel;
	_channels.insert(std::make_pair(channel.getChannelName(), channel));
}

std::string Server::greetJoinedUser(Client &client, Channel &channel)
{
    std::string reply;

    reply = RPL_JOIN(user_id(client.getNickname(), client.getUsername()), channel.getChannelName());
    if (channel.getUsers().size() == 1)
        reply += MODE_CHANNELMSG(channel.getChannelName(), channel.getModes());
    if (channel.getTopic().empty() == false) // if has a topic append it to the message
        reply += RPL_TOPIC(client.getNickname(), channel.getChannelName(), channel.getTopic());
    reply += RPL_NAMREPLY(client.getNickname(), '@', channel.getChannelName(), channel.getUsersList());
    reply += RPL_ENDOFNAMES(client.getUsername(), channel.getChannelName());
    return reply;
}

std::string Channel::getUsersList()
{
    std::string memberList;
	std::map<std::string, Client *> users = this->getUsers();
    for (std::map<std::string, Client*>::const_iterator iter = users.begin(); iter != users.end(); ++iter) {
        const Client* currentMember = iter->second;
        if (isOperator(currentMember->getNickname())) {
            memberList += "@";
        }
        memberList += currentMember->getNickname() + " ";
    }
    return ft_trim(memberList);
}

std::map<std::string, Client *> operators;
std::map<std::string, Client *> users;
std::map<std::string, Client *> inviteList;
std::map<char, bool> modes;
std::string _topic;
std::string _key;