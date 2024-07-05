#include <Server.hpp>


void Server::joinCommand(Client *client, const ParseMessage &ParsedMsg)
{
	//need to make some changes and add some new checks for params
	std::vector<std::string> params = ParsedMsg.getParams();
	std::vector<std::string> key_list;
	std::vector<std::string>::iterator itr_key;
	std::vector<std::string>::iterator itr_chan;
	std::string response = "";

	if(params.size() < 1)
	{
		client->serverReplies.push_back(ERR_NEEDMOREPARAMS(client->getNickname(), "JOIN"));
	}
	std::vector<std::string> chan_list = ft_split(params[0], ',');

	if(params.size() > 2) {return ;}
	if(params.size() > 1)
	{
		 key_list = ft_split(params[1], ',');
	}

	for (itr_chan = chan_list.begin(); itr_chan != chan_list.end(); ++itr_chan)
	{
		std::string chanName = *itr_chan;
		if (chanName.at(0) != '#' && chanName.at(0) != '&')
		{
			continue;
		}
		itr_key = key_list.begin();
		if (isChannelInServer(chanName))
		{
			Channel &tempChannel = getChannel(chanName);
			if(tempChannel.isClientInChannel(client->getNickname()))
			{
				response = ERR_USERONCHANNEL(client->getUsername(), client->getNickname(), chanName);
			}
			else if(!tempChannel.isInInvite(client->getNickname()) && tempChannel.checkMode('l') 
					&&  static_cast<int>(tempChannel.getUsers().size()) >= tempChannel.getUserLimit())
			{
				response = ERR_CHANNELISFULL(client->getNickname(), chanName);
			}
			else if(tempChannel.checkMode('i')
				&& !tempChannel.isInInvite(client->getNickname()))
			{
				response = ERR_INVITEONLYCHAN(client->getNickname(), chanName);
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
				}
			}
			else 
			{
				response = RPL_JOIN(user_id(client->getNickname(), client->getUsername()), chanName); //change this to welcome message
				tempChannel.removeInvite(client->getNickname());
				tempChannel.broadcastMessage(response);
				tempChannel.addClient(client);
			}
			client->serverReplies.push_back(response);
			return;
		}
		else
		{
			response = RPL_JOIN(user_id(client->getNickname(), client->getUsername()), chanName);
			_channels.insert(make_pair(chanName, Channel(chanName,
						client)));
		}
		client->serverReplies.push_back(response);
	}
}