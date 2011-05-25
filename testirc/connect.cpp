#include "connect.h"

IRC irc_st;

int initIRC(char* serv, char* channel, char* nick)
{
	irc_st.serv_ = serv;
	irc_st.channel_ = channel;
	irc_st.nick_ = nick;
	irc_st.callbacks_.event_channel = recieved_message;
	irc_st.callbacks_.event_connect = connected;
	irc_st.session_ = irc_create_session(&irc_st.callbacks_);
	printf("connection: %d\n", irc_connect(irc_st.session_, serv, 6667, NULL, irc_st.nick_, NULL, NULL));
	printf("run: %d\n", irc_run(irc_st.session_));
}

int send_message(char* message)
{
	irc_cmd_msg(irc_st.session_, irc_st.channel_, message);
	return(0);
}

int join_channel()
{
	printf("channel: %d\n", irc_cmd_join(irc_st.session_, irc_st.channel_, NULL));
}


void recieved_message(irc_session_t *session, const char* event, const char *origin, const char** params, unsigned int count)
{
	printf("%s: %s\n", origin, params[1]);
}

void connected(irc_session_t *session, const char* event, const char *origin, const char** params, unsigned int count)
{
	printf("connected\n");
	join_channel();
	send_message("Hello world\n");
}

int main()
{
	initIRC("irc.freenode.net", "#newchan", "avasitronen");
	return(0);
}
