/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alsaeed <alsaeed@student.42abudhabi.ae>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/08 11:48:38 by alsaeed           #+#    #+#             */
/*   Updated: 2024/07/15 16:26:48 by alsaeed          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <IrcLibs.hpp>
#include <IrcException.hpp>
#include <sys/socket.h>

class Channel 
{
	private:
		std::string channelName;
		std::string _topic;
		std::string _key;
		std::map<std::string, Client *> operators;
		std::map<std::string, Client *> users;
		std::map<std::string, Client *> inviteList;
		std::map<char, bool> modes;
		
		int UserLimit; 

	public:
		Channel(std::string &channelName, Client *client);
		~Channel();

		//SEND TO OTHERS
		void	broadcastMessage(const std::string message);
		void	sendToOthers(Client *client, std::string message);
		//ADD FUNCTIONS		
		void addClient(Client *client);
		void inviteClient(Client *client);
		void addOperator(std::string nickname);

		//REMOVE FUNCTIONS
		void removeClient(Client *client);
		void removeInvite(std::string &invite);
		void removeOperator(std::string nickname);
		void removeKey();
		void removeUserLimit();

		void	updateNickname(std::string oldNick, std::string newNick);
		
		//GETTERS
		std::string getKey( void ) const;
		std::map<std::string, Client *> getUsers() const;
		std::string  getUsersList();
		int		getUserLimit() const;
		std::string getModes() const;
		std::string getChannelName() const;
		std::string getTopic() const;
		std::map<char, bool> getModesMap() const;

		//SETTERS
		void setTopic(std::string &topic);
		// void setTopicRestricted(bool topicRestricted, Client* client);
		void setUserLimit(int limit);
		void setKey(std::string &password);
		bool setMode(char c, bool setting);

		//CHECK FUNCTIONS
		bool isClientInChannel(std::string nickname);
		bool isOperator(std::string &nickname);
		bool isInvited(std::string nickname);
		bool checkMode(char c);

};

#endif /* CHANNEL_HPP */