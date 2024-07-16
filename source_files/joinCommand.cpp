#include <Server.hpp>


void Server::joinCommand(Client *client, const ParseMessage &ParsedMsg)
{
	std::vector<std::string> params = ParsedMsg.getParams();
	std::vector<std::string> key_list;
	std::vector<std::string>::iterator itr_key;
	std::vector<std::string>::iterator itr_chan;
	std::string response = "";
	bool allowedJoin = true;

	if(params.size() > 2) {return ;}; //may need to remove this 
	if(params.size() < 1)
	{
		client->serverReplies.push_back(ERR_NEEDMOREPARAMS(client->getNickname(), "JOIN"));
		return;
	}
	std::vector<std::string> chan_list = ft_split(params[0], ',');

	if(params.size() > 1)
	{
		 key_list = ft_split(params[1], ',');
	}

	itr_key = key_list.begin();
	for (itr_chan = chan_list.begin(); itr_chan != chan_list.end(); ++itr_chan)
	{
		std::string chanName = *itr_chan;
		if (chanName.at(0) != '#' && chanName.at(0) != '&')
		{
			continue; //may need to print an error;
		}
		if (isChannelInServer(chanName))
		{
			Channel &tempChannel = getChannel(chanName);
			if(tempChannel.isClientInChannel(client->getNickname()))
			{
				response = ERR_USERONCHANNEL(client->getUsername(), client->getNickname(), chanName);
				allowedJoin = false;
			}
			else if(!tempChannel.isInvited(client->getNickname()) && tempChannel.checkMode('l') 
					&&  static_cast<int>(tempChannel.getUsers().size()) >= tempChannel.getUserLimit())
			{
				response = ERR_CHANNELISFULL(client->getNickname(), chanName);
				allowedJoin = false;
			}
			else if(tempChannel.checkMode('i')
				&& !tempChannel.isInvited(client->getNickname()))
			{
				response = ERR_INVITEONLYCHAN(client->getNickname(), chanName);
				allowedJoin = false;
			}
			else if (tempChannel.checkMode('k'))
			{
				if (itr_key != key_list.end() && *itr_key == tempChannel.getKey())
				{
					++itr_key;
				}
				else
				{
					response = ERR_BADCHANNELKEY(client->getNickname(), chanName);
					allowedJoin = false;
				}
			}
			if (allowedJoin) 
			{
				response = RPL_JOIN(user_id(client->getNickname(), client->getUsername()), chanName);
				tempChannel.removeInvite(client->getNickname());
				tempChannel.broadcastMessage(response);
				tempChannel.addClient(client);
				response = greetJoinedUser(*client, tempChannel);
			}
			client->serverReplies.push_back(response);
			break ;
		}
		else
		{
			response = RPL_JOIN(user_id(client->getNickname(), client->getUsername()), chanName);
			_channels.insert(make_pair(chanName, Channel(chanName,
						client)));
			response = greetJoinedUser(*client, getChannel(chanName));
			client->serverReplies.push_back(response);
		}
	}
}
