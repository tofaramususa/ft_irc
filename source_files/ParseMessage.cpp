/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ParseMessage.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tofaramususa <tofaramususa@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/15 18:55:07 by alsaeed           #+#    #+#             */
/*   Updated: 2024/07/12 14:33:52 by tofaramusus      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <Server.hpp>

std::string	ParseMessage::ft_trim( const std::string &str ) const {

	std::size_t start = str.find_first_not_of( " \n\r\t" );
	std::size_t end = str.find_last_not_of( " \n\r\t" );

	if ( start == std::string::npos || end == std::string::npos ) {

		return "";
	}

	return str.substr( start, end - start + 1 );
}

std::vector<std::string>  ft_split(std::string str, char delimiter)
{
	std::vector<std::string> result;
	std::string word;
	
	for (std::size_t i = 0; i < str.length(); ++i)
	{
		if(str[i] != delimiter)
		{
			word += str[i];
		}
		else
		{
			if(!word.empty())
			{
				result.push_back(word);
				word.clear();
			}
		}
	}
	if(!word.empty())
	{
		result.push_back(word);
	}
	return result;
}


//function to remove and split and remove all the spaces
std::vector<std::string> remove_spaces(std::string &str)
{
	std::vector<std::string> result;
	std::string word;

	for (std::size_t i = 0; i < str.length(); ++i)
	{
		if(str[i] != ' ')
		{
			word += str[i];
		}
		else
		{
			if(!word.empty())
			{
				result.push_back(word);
				word.clear();
			}
		}
	}
	if(!word.empty())
	{
		result.push_back(word);
	}
	return result;
}

ParseMessage::ParseMessage( const std::string& message ) {

	if ( message.empty() ) {

		return;
	}

	_msg = message;
	_msgLen = static_cast<int>(message.length());
	_cmd = "";
	_params.clear();
	_trailing = "";
	_notValidParam = false;
	_errorMsg = "";
	std::string	trimmedMsg = ft_trim( message );
	std::string token;
	std::istringstream iss( trimmedMsg );
	bool tagFlag = false;
	bool tagCmd = true;

	if ( trimmedMsg[0] == '@' ) {
		tagFlag = true;
	}
	
	while ( iss >> token ) {

		if ( tagFlag ) {

			if ( token[0] == ':' )
			{
				_cmd = token.substr( 1 );
				tagFlag = false;
				tagCmd = false;
				continue;
			}

			continue;
		}
		
		if ( tagCmd ) {
			
			_cmd = token;
			tagCmd = false;
			continue;
		}

		if ( token[0] == ':' ) {
			
			_trailing = ft_trim( message.substr( message.find( token ) + 1 ) ); // maybe add + 1
			break;
		} else {
	
			if ( isValid( token ) ) {
				
				_params.push_back( token );
			} else {
				
				_notValidParam = true;
				_errorMsg = "Invalid character in parameter: " + token;
				break;
			}	
		}
	}

	return;
}

bool	ParseMessage::isValid( const std::string &param ) const {

	/* if find_first_of() find any invalid character in param
	returns the position of it, o/w returns std::string::npos */
	std::string invalidChars = "\n\r\t:";
	std::size_t valid = param.find_first_of( invalidChars ); 

	return valid == std::string::npos;
}

bool Server::isAlphanumeric(const std::string &str) {
    for (std::string::const_iterator it = str.begin(); it != str.end(); ++it) {
        if (!std::isalnum(static_cast<unsigned char>(*it))) {
            return false;
        }
    }
    return true;
}