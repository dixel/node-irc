#include<libircclient/libircclient.h>
#include<stdio.h>
#include<string.h>

struct IRC
{
	char* serv_;
	char* channel_;
	char* nick_;
	irc_session_t* session_;
	char** mb_;
	irc_callbacks_t callbacks_;
};

int initIRC(char* serv, char* channel, char* nick);
int join_channel();
int send_message(char* message);
void recieved_message(irc_session_t *session, const char* event, const char *origin, const char** params, unsigned int count);
void connected(irc_session_t *session, const char* event, const char *origin, const char** params, unsigned int count);
