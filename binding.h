#define V8STR String::AsciiValue
#define MAXCLI 10

#include <node.h>
#include <stdio.h>
#include <node.h> 
#include <node_events.h> 
#include <pthread.h>
#include <libircclient/libircclient.h>

using namespace v8; 

//JS equivalent for irc_create_session command
//parameters:
//CreateSession(Object session) 
//session.sessionId - integer indentifier of the session, should be [0-MAXLEN], watch it to be unique
//session.RecCB - a callback function for recieving a message from some user to channel/personally
//session.ConCB - a callback for "connected" event on IRC server
static Handle<Value> CreateSession(const Arguments &args);

//JS equivalent for irc_connect command
//parameters:
//Connect(String server, Number port, String password, String nick, String username, String realname)
static Handle<Value> Connect(const Arguments &args);

//JS equivalent for irc_run command
//parameters:
//Run(Number session)
static Handle<Value> Run(const Arguments &args);

//JS equivalent for irc_cmd_join command
//parameters:
//Join(Number session, String channel, String key)
static Handle<Value> Join(const Arguments &args);

//JS equivalent for irc_cmd_msg command
//parameters:
//SendMsg(Number session, String dest, String text)
static Handle<Value> SendMsg(const Arguments &args);

//Internal structures and functions:

typedef struct message
{
    int session;
    const char *origin;
    const char **params;
} mess;

void rec_cb(irc_session_t *session, const char *event, const char *origin, const char **params, unsigned int count);
void con_cb(irc_session_t *session, const char *event, const char *origin, const char **params, unsigned int count);
void *run_thr(void *vptr_args);
void con_cb_ev(EV_P_ ev_async *watcher, int revents);
void rec_cb_ev(EV_P_ ev_async *watcher, int revents);

Persistent<Function> RecCB; 
Persistent<Function> ConCB; 
irc_callbacks_t _callbacks;
irc_session_t *_session[MAXCLI];
struct ev_async eio_nt;
struct ev_async eio_rc;
