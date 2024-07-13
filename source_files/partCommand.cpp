#include <Server.hpp>

void Server::partCommand(Client *client, const ParseMessage &ParsedMsg)
{
    std::vector<std::string> params = ParsedMsg.getParams();
    std::string response = "";

    if (params.empty()) {
        client->serverReplies.push_back(ERR_NEEDMOREPARAMS(client->getNickname(), "PART"));
        return;
    }

    std::vector<std::string> chan_list = ft_split(params[0], ',');
    std::string reason = ParsedMsg.getTrailing().size() ? ParsedMsg.getTrailing() : "";

    for (std::vector<std::string>::iterator itr_chan = chan_list.begin(); itr_chan != chan_list.end(); ++itr_chan)
    {
        std::string chanName = *itr_chan;
        if (chanName[0] != '#' && chanName[0] != '&')
            continue;
        if (!isChannelInServer(chanName)) {
            response = ERR_NOSUCHCHANNEL(client->getNickname(), chanName);
        }
        else {
            Channel &tempChannel = getChannel(chanName);

            if (!tempChannel.isClientInChannel(client->getNickname())) {
                response = ERR_NOTONCHANNEL(client->getNickname(), chanName);
            }
            else {
                std::string partMsg = RPL_PART(user_id(client->getNickname(), client->getUsername()), tempChannel.getChannelName(), reason);
                tempChannel.broadcastMessage(partMsg);
                tempChannel.removeClient(client);
                if (tempChannel.getUsers().empty())
				{
                    _channels.erase(chanName);
                }
                client->serverReplies.push_back(partMsg);
                continue;
            }
        }
        client->serverReplies.push_back(response);
    }
}