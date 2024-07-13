/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alsaeed <alsaeed@student.42abudhabi.ae>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/08 11:55:22 by alsaeed           #+#    #+#             */
/*   Updated: 2024/07/08 18:57:47 by alsaeed          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <Server.hpp>
#include <iostream>

int main( int ac, char* av[] ) {

	// Ensure correct number of arguments
	if ( ac != 3 ) {

		std::cerr << "Usage: " << av[0] << " <port> <password>" << std::endl;
		return -1;
	}

	// Convert port argument to integer and validate it
	int 		portNum = std::atoi( av[1] );
	std::string	port( av[1] ), password( av[2] );

	if ( portNum < 1024 || portNum > 65535 || port.empty() || port.length() > 5 \
		|| port.find_first_not_of( "0123456789" ) != std::string::npos ){

		std::cerr << "Invalid port number" << std::endl;
		return -2;
	}

	// Validate password is not empty
	if ( password.empty() ) {

		std::cerr << "Server's password is not given" << std::endl;
		return -3;
	}
	
	Server *server = NULL;
		// Initialize and run the server
	try {

		server = Server::getInstance();
		server->setServerPassword( password );
		server->setServerPort( portNum );
		server->initServer();
		server->runServer();
	} catch ( const IrcException &e ) {

		std::cerr << "Server error: " << e.what() << std::endl;
		delete server;
		return -4;
	} catch (const std::exception& e) {
        std::cerr << "Unexpected error: " << e.what() << std::endl;
		delete server;
        return -5;
    } catch (...) {
        std::cerr << "Unknown error occurred." << std::endl;
		delete server;
        return -6;
    }

	delete server;

	return 0;
}