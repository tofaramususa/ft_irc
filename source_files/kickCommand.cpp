#include <Server.hpp>

void Server::kickCommand(Client *client, const ParseMessage &ParsedMsg)
{
    std::vector<std::string> params = ParsedMsg.getParams();

    if (params.size() < 2) {
        client->serverReplies.push_back(ERR_NEEDMOREPARAMS(client->getNickname(), "KICK"));
        return;
    }

    std::string trailingMessage = ParsedMsg.getTrailing().empty() ? "" :  ParsedMsg.getTrailing();
    std::string channelName = params[0];


    if (!isChannelInServer(channelName)) {
        client->serverReplies.push_back(ERR_NOSUCHCHANNEL(client->getNickname(), channelName));
        return;
    }
    Channel &channel = getChannel(channelName);

    if (!channel.isClientInChannel(client->getNickname())) {
        client->serverReplies.push_back(ERR_NOTONCHANNEL(client->getNickname(), channelName));
        return;
    }

    if (!channel.isOperator(client->getNickname())) {
        client->serverReplies.push_back(ERR_CHANOPRIVSNEEDED(client->getNickname(), channelName));
        return;
    }

    std::vector<std::string> users = ft_split(params[1] , ',');

    for (std::vector<std::string>::iterator it = users.begin(); it != users.end(); ++it)
    {
        std::string targetNick = *it;
        // Check if the client is trying to kick themselves
        if (targetNick == client->getNickname()) {
            client->serverReplies.push_back(": localhost  482 " + client->getNickname() + " " + channelName + " :You can't kick yourself\r\n");
            continue;
        }
        Client *targetClient = getClient(targetNick);

        if (!targetClient || !channel.isClientInChannel(targetNick)) {
            client->serverReplies.push_back(ERR_USERNOTINCHANNEL(client->getNickname(), targetNick, channelName));
            continue;
        }

        std::string kickMsg = RPL_KICK(user_id(client->getNickname(), client->getUsername()), channelName, targetClient->getNickname(), trailingMessage);
        channel.broadcastMessage(kickMsg);
        channel.removeClient(targetClient);

        if (channel.getUsers().empty()) {
            _channels.erase(channelName);
        }
    }
}