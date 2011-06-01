#define V8STR String::AsciiValue
#define MAXCLI 100000
#define EVENTLEN 10
#define ORIGINLEN 30

#include <node.h>
#include <stdio.h>
#include <node.h> 
#include <string.h>
#include <node_events.h> 
#include <pthread.h>
#include <libircclient/libircclient.h>

using namespace v8; 

//JS equivalent for irc_create_session command
//parameters:
//CreateSession(Object session) 
//return:
//Object: Session - internal unique indentifier of this session
static Handle<Value> CreateSession(const Arguments &args); 
//JS equivalent for irc_connect command
//parameters:
//Connect(String server, Number port, String password, String nick, String username, String realname)
static Handle<Value> Connect(const Arguments &args);

//JS equivalent for irc_disconnect command
//parameters:
//Disconnect(Session)
static Handle<Value> Disconnect(const Arguments &args);

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


static Handle<Value> Part(const Arguments &args);
static Handle<Value> Topic(const Arguments &args);
static Handle<Value> Kick(const Arguments &args);
static Handle<Value> Mode(const Arguments &args);
static Handle<Value> Invite(const Arguments &args);
static Handle<Value> Me(const Arguments &args);
static Handle<Value> Notice(const Arguments &args);
static Handle<Value> Quit(const Arguments &args);
static Handle<Value> UserMode(const Arguments &args);
static Handle<Value> Whois(const Arguments &args);
static Handle<Value> SendRaw(const Arguments &args);
static Handle<Value> SetCtx(const Arguments &args);
static Handle<Value> GetCtx(const Arguments &args);

//Internal structures and functions:

typedef struct message
{
    int session;
    const char *origin;
    const char **params;
} mess;

typedef struct cb_type
{
    int session;
    char event[EVENTLEN];
    char origin[ORIGINLEN];
    const char **params;
} cbvar;

int strip_sess(Handle<Value> sess);
int call_func(cbvar *msg, const char *name);
void cmn_cb(irc_session_t *session, const char *event, const char *origin, const char **params, unsigned int count);
void *run_thr(void *vptr_args);
void cmn_cb_ev(EV_P_ ev_async *watcher, int revents);

Persistent<Object> UserCB[MAXCLI];
irc_callbacks_t _callbacks;
irc_session_t *_session[MAXCLI];
int sess_cnt = 0;
struct ev_async eio_cm;
