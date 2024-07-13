/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ProfanityPatrol.hpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alsaeed <alsaeed@student.42abudhabi.ae>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/12 20:42:52 by alsaeed           #+#    #+#             */
/*   Updated: 2024/07/12 20:43:39 by alsaeed          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#ifndef PROFANITY_PATROL_HPP
# define PROFANITY_PATROL_HPP 

# include <iostream>
# include <stdlib.h>
# include <sstream>
# include <string>
# include <vector>
# include <sys/socket.h>
# include <netinet/in.h>
# include <arpa/inet.h>
# include <unistd.h>
# include <poll.h>
# include <cstring>
# include <cerrno>

class ProfanityPatrol {
private:
    std::string _address;
    int _port;
    std::string _password;
    int _socket;
    std::string _nickname;
    std::string _username;
    std::vector<std::string> _profanityList;
    bool _isConnected;

    // Private member functions
    bool connectToServer();
    void authenticate();
    bool processMessages();
    void handleInvite(const std::string& channel);
    void scanForProfanity(const std::string& message, const std::string& channel);
    void sendMessage(const std::string& message);
    void disconnect();

public:
    ProfanityPatrol(const std::string& address, int port, const std::string& password);
    ~ProfanityPatrol();

    void run();
};

#endif