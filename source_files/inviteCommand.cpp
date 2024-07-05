#include <Server.hpp>

void Server::inviteCommand(Client *client, const ParseMessage &ParsedMsg)
{
    std::vector<std::string> params = ParsedMsg.getParams();
    std::string response = "";
	std::string targetNickname;
	 std::string channelName;

    if (params.size() < 2) {
        client->serverReplies.push_back(ERR_NEEDMOREPARAMS(client->getNickname(), "INVITE"));
        return;
    }
    targetNickname = params[0];
    channelName = params[1];
    if (channelName.at(0) != '#' && channelName.at(0) != '&') {
        return;
    }
    if (!isChannelInServer(channelName)) {
        response = ERR_NOSUCHCHANNEL(client->getNickname(), channelName);
        client->serverReplies.push_back(response);
        return;
    }

    Channel &channel = getChannel(channelName);

    if (!channel.isClientInChannel(client->getNickname())) {
        response = ERR_NOTONCHANNEL(client->getNickname(), channelName);
        client->serverReplies.push_back(response);
        return;
    }

    if (channel.checkMode('i') && !channel.isOperator(client->getNickname())) {
        response = ERR_CHANOPRIVSNEEDED(client->getNickname(), channelName);
        client->serverReplies.push_back(response);
        return;
    }

    Client *targetClient = getClient(targetNickname);
    if (!targetClient) {
        response = ERR_NOSUCHNICK(client->getNickname(), targetNickname);
        client->serverReplies.push_back(response);
        return;
    }
    if (channel.isClientInChannel(targetClient->getNickname())) {
        response = ERR_USERONCHANNEL(client->getNickname(), targetNickname, channelName);
        client->serverReplies.push_back(response);
        return;
    }
    channel.inviteClient(targetClient);
    response = RPL_INVITING(std::string("localhost"), client->getNickname(), targetNickname, channelName);
    client->serverReplies.push_back(response);
    std::string inviteMessage = ":" + client->getNickname() + "!" + client->getUsername() + "@" + " INVITE " + targetNickname + " " + channelName;
    targetClient->serverReplies.push_back(inviteMessage);
}
