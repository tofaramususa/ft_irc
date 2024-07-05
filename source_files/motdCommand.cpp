#include <Server.hpp>

void	Server::motdCommand(Client *client)
{
    std::ostringstream oss;
    std::string line;
    std::ifstream infile;

	if(!client->isRegistered())
		return;
    infile.open("./source_files/motd.txt", std::ios::in);
	
	client->serverReplies.push_back( RPL_WELCOME(user_id(client->getNickname(),client->getUsername()),client->getNickname()));
	client->serverReplies.push_back( RPL_YOURHOST(client->getUsername(),"irssi", "1"));
	client->serverReplies.push_back( RPL_CREATED(client->getUsername(), std::string("creation_date")));
	client->serverReplies.push_back( RPL_MYINFO(client->getUsername(),"irssi", "1", "","",""));
	client->serverReplies.push_back( RPL_ISUPPORT(client->getUsername(),"CHANMODES=ikolt"));
	client->serverReplies.push_back( RPL_MOTDSTART(client->getUsername(),client->getUsername()));
	client->serverReplies.push_back( RPL_MOTDSTART(client->getUsername(),std::string("ft_irc_server")));
    if (infile.is_open())
    {
        while (std::getline(infile,line))
        {
            client->serverReplies.push_back( RPL_MOTD(std::string("ft_irc_server"),line));
        }
        infile.close();
    }
    else
        client->serverReplies.push_back( ERR_NOMOTD(std::string("ft_irc_server")));
		
    client->serverReplies.push_back( RPL_ENDOFMOTD(std::string("ft_irc_server")));
    return ;
}
