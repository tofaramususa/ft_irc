#include <Channel.hpp>
#include <Server.hpp>


void Server::privateMessage(Client *client, const ParseMessage &parsedMsg)
{
    const std::vector<std::string>& params = parsedMsg.getParams();
    const std::string& trailing = parsedMsg.getTrailing();
	std::string receiver; 

    if (params.empty() && trailing.empty())
    {
        if (params.empty())
            client->serverReplies.push_back(ERR_NORECIPIENT(client->getNickname()));
        else
            client->serverReplies.push_back(ERR_NOTEXTTOSEND(client->getNickname()));
        return;
    }

	if(params.empty() == false)
	{
		receiver = params[0];
	}

    if(receiver[0] == '#' || receiver[0] == '&') //potential segfault here for receiver
    {
        if (!isChannelInServer(receiver))
        {
            client->serverReplies.push_back(ERR_CANNOTSENDTOCHAN(client->getNickname(), receiver));
            return;
        }

        Channel &channel = getChannel(receiver);
        if (!channel.isClientInChannel(client->getNickname()))
        {
            client->serverReplies.push_back(ERR_CANNOTSENDTOCHAN(client->getNickname(), receiver));
            return;
        }
        channel.sendToOthers(client, RPL_PRIVMSG(client->getNickname(), client->getNickname(), receiver, trailing));
    }
    else
    {
        if (!isUserInServer(receiver))
        {
            client->serverReplies.push_back(ERR_NOSUCHNICK(client->getNickname(), receiver));
            return;
        }
        Client *recipientClient = getClient(receiver);
        recipientClient->serverReplies.push_back(RPL_PRIVMSG(client->getNickname(), client->getUsername(), receiver, trailing));
    }
}