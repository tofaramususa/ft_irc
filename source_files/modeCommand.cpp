#include <Server.hpp>

bool Server::handleKeyMode(Client *client, Channel &channel, bool isAdding,
	  std::vector<std::string> &params, std::size_t &paramIndex)
{
	std::map<char, bool> modesMap = channel.getModesMap();
	std::map<char, bool>::iterator itr = modesMap.find('k');
	
	if ( isAdding == itr->second ) {

		return (false);
	}
	if (isAdding && paramIndex < params.size()) {
		if(isAlphanumeric(params[paramIndex])) {
			channel.setKey(params[paramIndex++]);
			std::string key(channel.getKey().size(), '*');
			client->serverReplies.push_back(RPL_CHANNELMODEISWITHKEY(client->getNickname(), channel.getChannelName(), channel.getModes(), key));
			return (true);
		}
		else {
			client->serverReplies.push_back(ERR_INVALIDMODEPARAM(client->getNickname(),channel.getChannelName(),'k', params[paramIndex++]));
			return(false);
		}
		return (true);
	} else if (isAdding) {

		client->serverReplies.push_back(ERR_NEEDMOREPARAMS(client->getNickname(), "MODE +k"));
	} else {

		channel.removeKey();
		return(true);
	}

	return (false);
}

bool Server::handleLimitMode(Client *client, Channel &channel, bool isAdding,
	  std::vector<std::string> &params, std::size_t &paramIndex)
{
	int	UserLimit;

	std::map<char, bool> modesMap = channel.getModesMap();
	std::map<char, bool>::iterator itr = modesMap.find('l');
	if ( isAdding == itr->second ) {

		return (false);
	}

	if (isAdding && paramIndex < params.size())
	{
		UserLimit = std::atoi(params[paramIndex++].c_str());
		if(UserLimit > 0)
		{
			channel.setUserLimit(UserLimit);
			return (true);
		}
		else
		{
			client->serverReplies.push_back(ERR_WRONGMODEPARAMS(client->getNickname(), channel.getChannelName(), "MODE +l"));
			return (false);
		}
	}
	else if (isAdding)
	{
		client->serverReplies.push_back(ERR_NEEDMOREPARAMS(client->getNickname(), "MODE +l"));
	}
	else
	{
		channel.removeUserLimit();
		return(true);
	}
	return (false);
}

bool Server::handleOperatorMode(Client *client, Channel &channel, bool isAdding,
	  std::vector<std::string> &params, std::size_t &paramIndex)
{
	std::map<char, bool> modesMap = channel.getModesMap();
	std::map<char, bool>::iterator itr = modesMap.find('o');
	
	if ( isAdding == itr->second ) {

		return (false);
	}

	if (paramIndex < params.size())
	{
		std::string targetNick = params[paramIndex++];
		if(!channel.isClientInChannel(targetNick))
		{
			client->serverReplies.push_back(ERR_USERNOTINCHANNEL(client->getNickname(), targetNick, channel.getChannelName()));
			return (false);
		}
		if (isAdding)
			channel.addOperator(targetNick);
		else
			channel.removeOperator(targetNick);
		return (true);
	}
	else
	{
		client->serverReplies.push_back(ERR_NEEDMOREPARAMS(client->getNickname(), "MODE o"));
	}
	return (false);
}

bool Server::processSingleChannelMode(Client *client, Channel &channel,
	char mode, bool isAdding,   std::vector<std::string> &params,
	std::size_t &paramIndex)
{
	switch (mode)
	{
	case 'i':
		return (channel.setMode('i', isAdding));
	case 'k':
		return (handleKeyMode(client, channel, isAdding, params, paramIndex));
	case 'l':
		return (handleLimitMode(client, channel, isAdding, params, paramIndex));
	case 't':
		return (channel.setMode('t', isAdding));
	case 'o':
		return (handleOperatorMode(client, channel, isAdding, params,
				paramIndex));
	case 'b':
		return (false);
	default:
		client->serverReplies.push_back(ERR_UNKNOWNMODE(client->getNickname(), std::string(1,
					mode)));
		return (false);
	}
}

void Server::processChannelModes(Client *client, Channel &channel,
	  std::vector<std::string> &params)
{
	bool	isAdding;
	std::size_t	paramIndex;
	char	mode;

	std::string modeString = params[1];
	isAdding = true;
	paramIndex = 2;
	// std::string modeChanges = ":" + client->getNickname() + " MODE "
	// 	+ channel.getChannelName() + " " + modeString;

	std::string modeStr;
	for (std::size_t i = 0; i < modeString.length(); ++i)
	{
		mode = modeString[i];
		if (mode == '+' || mode == '-')
		{
			isAdding = (mode == '+');
			continue;
		}
		if (processSingleChannelMode(client, channel, mode, isAdding, params, paramIndex)) {

			modeStr += isAdding == false ? std::string(1,'-') + std::string(1, mode) : std::string(1,'+') + std::string(1, mode);
		}
	}
	
	if ( modeStr.empty() == false ) {
		std::string modeChanges = MODE_CHANNELCHANGEMODE(user_id(client->getNickname(), client->getUsername()), channel.getChannelName(), modeStr);
		channel.broadcastMessage(modeChanges);
	}
}

void Server::handleChannelMode(Client *client, std::string &channelName,
	  std::vector<std::string> &params)
{
	if (!Server::isChannelInServer(channelName))
	{
		client->serverReplies.push_back(ERR_NOSUCHCHANNEL(client->getNickname(),
				channelName));
		return ;
	}
	Channel &channel = Server::getChannel(channelName);
	const std::string nick = client->getNickname();
	if (params.size() == 1)
	{
		client->serverReplies.push_back(RPL_CHANNELMODEIS(client->getNickname(), channelName,
				channel.getModes()));
	}
	else
	{
		if (!channel.isOperator(const_cast<std::string &>(client->getNickname())))
		{
			client->serverReplies.push_back(ERR_CHANOPRIVSNEEDED(client->getNickname(),
					channelName));
			return ;
		}
		processChannelModes(client, channel, params);
	}
}

void Server::modeCommand(Client *client, const ParseMessage &parsedMsg)
{
	  std::vector<std::string> params = parsedMsg.getParams();

	if(parsedMsg.getTrailing().empty() == false)
	{
		std::vector<std::string> splitTrailing = ft_split(parsedMsg.getTrailing(), ' ');
		params.insert(params.end(), splitTrailing.begin(), splitTrailing.end());
	}

	if (params.size() < 1)
	{
		client->serverReplies.push_back(ERR_NEEDMOREPARAMS(client->getNickname(), "MODE"));
		return ;
	}
	std::string target = params[0];
	// params.erase(params.begin());
	if (target[0] == '#' || target[0] == '&')
	{
		handleChannelMode(client, target, params);
	}
	else
	{
		if ( !isUserInServer( target ) ) {

			client->serverReplies.push_back(ERR_NOSUCHCHANNEL(client->getNickname(), target));
		}
	}
}