/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tofaramususa <tofaramususa@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/08 11:50:49 by alsaeed           #+#    #+#             */
/*   Updated: 2024/07/03 14:57:47 by tofaramusus      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <Server.hpp>
#include <Channel.hpp>

//CONSTRUCTOR
Channel::Channel(std::string &channelName, Client *client) : channelName(channelName), _topic(""), _key(""), UserLimit(0)
{
	operators[client->getNickname()] = client;
	users[client->getNickname()] = client;
	modes['i'] = false;
	modes['t'] = false;
	modes['k'] = false;
	modes['o'] = false;
	modes['l'] = false;
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
		return true;
	}
	return false;
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
	
	if(this->inviteList.find(nickname) != inviteList.end())
	{
		return true;
	}
	return false;
}


std::map<std::string, Client *> Channel::getUsers( void )
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

std::string Channel::getKey()
{
	return this->_key;
}

int Channel::getUserLimit()
{
	return UserLimit;
}

std::string Channel::getModes() const
{
    std::string result;

    for (std::map<char, bool>::const_iterator it = modes.begin(); it != modes.end(); ++it) {
        if (it->second) {
            result += it->first;
        }
    }
    return result;
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

void Channel::setMode(char c, bool setting)
{
	std::map<char, bool>::iterator itr = modes.find(c);
	if(itr != modes.end())
	{
		itr->second = setting;
	}
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

void	Server::addChannel(Channel &channel)
{
	// _channels[channel.getChannelName()] = channel;
	_channels.insert(std::make_pair(channel.getChannelName(), channel));
}

std::map<std::string, Client *> operators;
std::map<std::string, Client *> users;
std::map<std::string, Client *> inviteList;
std::map<char, bool> modes;