# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: alsaeed <alsaeed@student.42abudhabi.ae>    +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/06/05 23:41:54 by alsaeed           #+#    #+#              #
#    Updated: 2024/07/03 18:36:37 by alsaeed          ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = ircserv

CXX = c++
CXXFLAGS = -Wall -Werror -Wextra -std=c++98 #-g3 -fsanitize=address
INCLUDES = -Iincludes/

SRCS =	Server.cpp \
		Channel.cpp \
		Client.cpp \
		ParseMessage.cpp \
		nickCommand.cpp \
		quitCommand.cpp \
		joinCommand.cpp  \
		privateMessage.cpp \
		Commands.cpp \
		modeCommand.cpp \
		inviteCommand.cpp \
		kickCommand.cpp \
		motdCommand.cpp \
		noticeCommand.cpp \
		partCommand.cpp \
		topicCommand.cpp

OBJS_DIR = object_files
OBJS = $(SRCS:%.cpp=$(OBJS_DIR)/%.o)

# colors #
GREEN 		= \033[0;32m
RED 		= \033[0;31m
YELLOW 		= \033[0;33m
BLUE 		= \033[0;34m
BOLD_GREEN 	= \033[1;32m
BOLD_RED 	= \033[1;31m
BOLD_YELLOW = \033[1;33m
BOLD_BLUE 	= \033[1;34m
RESET 		= \033[0m

all: $(NAME)

$(OBJS_DIR)/%.o: source_files/%.cpp
	@echo "$(BOLD_YELLOW)compiling$(RESET) $(GREEN)$<$(RESET):\r\t\t\t\t\t"
	@mkdir -p $(OBJS_DIR)
	@$(CXX) $(CXXFLAGS) -c $(INCLUDES) $< -o $@
	@echo "\r\t\t\t\t\t$(RED)$(CXX) $(CXXFLAGS)$(RESET)$(BLUE)-c $< -o $@$(RESET) $(BOLD_GREEN)<OK>$(RESET)"

$(NAME): $(OBJS) main.cpp
	@$(CXX) $(CXXFLAGS) $(INCLUDES) $(OBJS) main.cpp -o $@
	@echo "$(BOLD_YELLOW)ircserv Compiled$(RESET): $(BOLD_GREEN)<OK>$(RESET)"

$(OBJS):	Makefile \
			main.cpp \
			includes/Server.hpp \
			includes/Client.hpp \
			includes/Channel.hpp \
			includes/Reply.hpp \
			includes/IrcException.hpp \
			includes/IrcLibs.hpp \
			includes/ParseMessage.hpp \
			source_files/Server.cpp \
			source_files/Channel.cpp \
			source_files/Client.cpp \
			source_files/ParseMessage.cpp \
			source_files/joinCommand.cpp \
			source_files/nickCommand.cpp \
			source_files/quitCommand.cpp \
			source_files/privateMessage.cpp \
			source_files/modeCommand.cpp \
			source_files/inviteCommand.cpp \
			source_files/kickCommand.cpp \
			source_files/motdCommand.cpp \
			source_files/noticeCommand.cpp \
			source_files/partCommand.cpp \
			source_files/topicCommand.cpp \
			source_files/Commands.cpp

clean:
	@if [ -e $(OBJS_DIR) ]; then \
		rm -rf $(OBJS_DIR); \
		echo "$(BOLD_YELLOW)ircserv Clean$(RESET): $(BOLD_GREEN)<OK>$(RESET)"; \
	fi

fclean: clean
	@if [ -e $(NAME) ]; then \
		rm -rf $(NAME); \
		echo "$(BOLD_YELLOW)ircserv Full-Clean$(RESET): $(BOLD_GREEN)<OK>$(RESET)"; \
	fi

re: fclean all

.PHONY: all clean fclean re