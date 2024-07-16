#include <Server.hpp>

void Server::noticeCommand(Client *client, const ParseMessage &parsedMsg)
{
    const std::vector<std::string>& params = parsedMsg.getParams();
    const std::string& trailing = parsedMsg.getTrailing().empty() ? "" : parsedMsg.getTrailing();

    if (params.empty() || trailing.empty()) { return; }

    std::vector<std::string> receivers = ft_split(params[0], ',');
    std::vector<std::string>::const_iterator it;

    for (it = receivers.begin(); it != receivers.end(); ++it)
    {
        std::string receiver = *it;

        if(receiver[0] == '#' || receiver[0] == '&')
        {
            if (!isChannelInServer(receiver))
            {
                continue;
            }

            Channel &channel = getChannel(receiver);
            if (!channel.isClientInChannel(client->getNickname()))
            {
                continue;
            }
            channel.sendToOthers(client, RPL_NOTICE(client->getNickname(), client->getUsername(), receiver, trailing));
        }
        else
        {
            if (!isUserInServer(receiver))
            {
                continue;
            }
            getClient(receiver)->serverReplies.push_back(RPL_NOTICE(client->getNickname(), client->getUsername(), receiver, trailing));
        }
    }
}