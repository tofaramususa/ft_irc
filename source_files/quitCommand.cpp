#include <Server.hpp>

void				Server::quitCommand(std::string reason, Client *client)
{
	std::map<std::string, Channel>::iterator	itr;
	std::string nickname = client->getNickname();
	std::string message = "has quit";
	for(itr = _channels.begin(); itr != _channels.end(); itr++)
	{
		if(itr->second.isClientInChannel(nickname))
		{
			//remove user from all channels
			if(reason.empty())
			{
				// itr->second.broadcastMessage(RPL_QUIT(user_id(client->getNickname(), client->getUsername()), message));
			}
			else
			{
				// itr->second.broadcastMessage(RPL_ERROR(user_id(client->getNickname(), client->getUsername()), reason));
			}
		}
	}
	// closeClient(client->getFd());
	// close(client->getFd());
	throw(std::exception());
	client->setFd(-1);
}