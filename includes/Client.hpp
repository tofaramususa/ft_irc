/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alsaeed <alsaeed@student.42abudhabi.ae>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/08 11:37:35 by alsaeed           #+#    #+#             */
/*   Updated: 2024/07/15 16:43:47 by alsaeed          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#ifndef CLIENT_HPP
# define CLIENT_HPP

#include <string>
#include <vector>
#include <IrcException.hpp>
#include <unistd.h>
#include <cstring>
#include <sys/socket.h>

class Client {

	private:

		int							_fd;
		bool						_isCorrectPassword;
		std::string					_nickname;
		std::string					_username;
		std::string					_channel;
		

	public:
		
		bool						isRegistered;
		std::vector<std::string>	serverReplies;
		bool	conRegi[3];
		Client( void );
		Client( int fd );

		bool		sendMessage( const std::string &message );
		
		//SETTERS
		void		setIsCorrectPassword( bool isCorrectPassword );
		void		setNickname( const std::string &nickname );
		void		setUsername( const std::string &username );
		void		setFd(int value);
		
		//GETTERS
		std::string getFullIdentity( void ) const;
		std::string &getNickname( void ) const;
		std::string getUsername( void ) const;
		bool		getIsCorrectPassword( void ) const;
		int			getFd( void ) const;
};

#endif /* CLIENT_HPP */