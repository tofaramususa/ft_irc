#include <Server.hpp>

void	Server::quitCommand(std::string reason, Client *client) //there are some changes to take care of 
{
	std::map<std::string, Channel>::iterator	itr;
	std::string nickname = client->getNickname();
	std::string message = "has quit";

	for (itr = _channels.begin(); itr != _channels.end(); itr++) //the channels in the server
	{
		if(itr->second.isClientInChannel(nickname)) //if in channel
		{
			if(reason.empty())
			{
				itr->second.broadcastMessage(RPL_QUIT(user_id(client->getNickname(), client->getUsername()), message));
			}
			else
			{
				itr->second.broadcastMessage(RPL_QUIT(user_id(client->getNickname(), client->getUsername()), reason));
			}
			itr->second.removeClient(client);
		}
	}
	client->setFd(-1);
	throw(std::exception());
}