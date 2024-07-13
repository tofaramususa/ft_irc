/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ProfanityPatrol.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alsaeed <alsaeed@student.42abudhabi.ae>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/12 16:04:31 by shamzaou@st       #+#    #+#             */
/*   Updated: 2024/07/12 20:43:59 by alsaeed          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ProfanityPatrol.hpp"

ProfanityPatrol::ProfanityPatrol(const std::string& address, int port, const std::string& password)
    : _address(address), _port(port), _password(password), _socket(-1), _isConnected(false) {
    _nickname = "ProfanityPatrol";
    _username = "profanitybot";
    // Initialize profanity list
    _profanityList.push_back("badword1");
    _profanityList.push_back("badword2");
    _profanityList.push_back("badword3");
}

ProfanityPatrol::~ProfanityPatrol() {
    if (_socket != -1) {
        close(_socket);
    }
}

bool ProfanityPatrol::connectToServer() {
    _socket = socket(AF_INET, SOCK_STREAM, 0);
    if (_socket == -1) {
        std::cerr << "Failed to create socket" << std::endl;
        return false;
    }

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(_port);
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");  // localhost IP

    if (connect(_socket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        std::cerr << "Connection failed: " << strerror(errno) << std::endl;
        close(_socket);
        return false;
    }

    std::cout << "Connected to server successfully" << std::endl;
    return true;
}

void ProfanityPatrol::authenticate() {
    sendMessage("CAP LS");
    sendMessage("CAP END");
    sendMessage("PASS " + _password);
    sendMessage("NICK " + _nickname);
    sendMessage("USER " + _username + " 0 * :Profanity Patrol Bot");
}

void ProfanityPatrol::scanForProfanity(const std::string& message, const std::string& channel) {
    for (std::vector<std::string>::const_iterator it = _profanityList.begin(); it != _profanityList.end(); ++it) {
        if (message.find(*it) != std::string::npos) {
            std::string response = "PRIVMSG " + channel + " :Warning: Profanity detected!";
            sendMessage(response);
            break;
        }
    }
}

bool ProfanityPatrol::processMessages() {
    char buffer[4096];
    ssize_t bytesRead = recv(_socket, buffer, sizeof(buffer) - 1, 0);
    if (bytesRead > 0) {
        buffer[bytesRead] = '\0';
        std::istringstream iss(buffer);
        std::string line;
        while (std::getline(iss, line)) {
            std::cout << "Received: " << line << std::endl; // Debug output

            if (line.find("PING") == 0) {
                sendMessage("PONG" + line.substr(4));
            } else if (line.find("INVITE") != std::string::npos) {
                size_t channelPos = line.rfind(':');
                if (channelPos != std::string::npos) {
                    std::string channel = line.substr(channelPos + 1);
                    handleInvite(channel);
                }
            } else {
                size_t channelPos = line.find('#');
                if (channelPos != std::string::npos) {
                    size_t spacePos = line.find(' ', channelPos);
                    if (spacePos != std::string::npos) {
                        std::string channel = line.substr(channelPos, spacePos - channelPos);
                        // Trim any trailing characters that aren't part of the channel name
                        size_t endPos = channel.find_first_of(" \t\r\n");
                        if (endPos != std::string::npos) {
                            channel = channel.substr(0, endPos);
                        }
                        scanForProfanity(line, channel);
                    }
                }
            }
        }
        return true;
    } else if (bytesRead == 0) {
        std::cout << "Server closed the connection" << std::endl;
        return false;
    } else {
        std::cerr << "Error receiving data: " << strerror(errno) << std::endl;
        return false;
    }
}

void ProfanityPatrol::handleInvite(const std::string& channel) {
    sendMessage("JOIN " + channel);
    std::cout << "Joined channel: " << channel << std::endl;
}


void ProfanityPatrol::sendMessage(const std::string& message) {
    std::string fullMessage = message + "\r\n";
    send(_socket, fullMessage.c_str(), fullMessage.length(), 0);
}

void ProfanityPatrol::run() {
    if (!connectToServer()) {
        return;
    }

    _isConnected = true;
    authenticate();

    struct pollfd pfd;
    pfd.fd = _socket;
    pfd.events = POLLIN;

    while (_isConnected) {
        int ret = poll(&pfd, 1, 5000);  // 5 second timeout
        if (ret > 0) {
            if (pfd.revents & POLLIN) {
                if (!processMessages()) {
                    std::cout << "Lost connection to server" << std::endl;
                    break;
                }
            }
        } else if (ret == 0) {
            // Timeout, can be used to implement periodic tasks if needed
            continue;
        } else {
            std::cerr << "Poll error: " << strerror(errno) << std::endl;
            break;
        }
    }

    disconnect();
    std::cout << "Bot disconnected from server" << std::endl;
}

void ProfanityPatrol::disconnect() {
    if (_socket != -1) {
        close(_socket);
        _socket = -1;
    }
    _isConnected = false;
}

int main(int argc, char* argv[]) {
    if (argc != 4) {
        std::cerr << "Usage: " << argv[0] << " <address> <port> <password>" << std::endl;
        return 1;
    }

    std::string address = argv[1];
    int port = atoi(argv[2]);
    std::string password = argv[3];

    ProfanityPatrol bot(address, port, password);
    bot.run();

    return 0;
}
