#include <Server.hpp>



void 	Server::nickCommand(Client *client, const std::vector<std::string> &params)
{
	if(params.size() != 1)
		return ;
	//check its not empty
	//check if nickname not in use already
	//check nickname doest contain these find_first_of("# @:&")
	//if it checks out then add to nicknames vector
	//erase the current nickname from nickname list
	//send message with nickname reply
	//clear then add the new nickname
	std::string newNick = params[0];
	if (newNick.empty())
    {
       client->serverReplies.push_back(ERR_NONICKNAMEGIVEN(client->getNickname()));
	   return ;
    }
    else if (newNick.find_first_of("#@:&") != std::string::npos)
    {
       client->serverReplies.push_back(ERR_ERRONEUSNICKNAME(client->getNickname(), newNick));
	   return ;
    }
    else if (std::find(_nicknames.begin(), _nicknames.end(), newNick) != _nicknames.end())
    {
       client->serverReplies.push_back(ERR_NICKNAMEINUSE(client->getNickname(), newNick));
	   return ;
    }
    else if (!client->getNickname().empty())
    {
        _nicknames.erase(std::remove(_nicknames.begin(), _nicknames.end(),client->getNickname()), _nicknames.end());
       client->serverReplies.push_back(RPL_NICK(client->getNickname(),client->getUsername(), newNick));
    }
	_nicknames.push_back(newNick);
	client->setNickname(newNick);
	return ;
}