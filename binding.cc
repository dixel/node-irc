#include <node.h>
#include <stdio.h>
#include <node.h> 
#include <node_events.h> 
#include <pthread.h>
#include <libircclient/libircclient.h>

#define V8STR String::AsciiValue
#define MAXCLI 10

using namespace v8; 

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

//JS equivalent for irc_create_session command
//parameters:
//CreateSession(Integer SessionID, Function RecCB, Function ConCB) //RecCB - a callback function for recieving a message from some user to channel/personally
//ConCB - a callback for "connected" event on IRC server
static Handle<Value> CreateSession(const Arguments &args)
{
    printf("DEBUG:: createsession\n");
    unsigned int sess_id = 0;
    _callbacks.event_channel = rec_cb;
    _callbacks.event_connect = con_cb;
    bool wecandoit = false;
    if (args[0]->IsObject())
    {
        Local<Object> sess = Object::Cast(*args[0]);
        if(sess->Has(String::New("sessionId")))
        {
            sess_id = sess->Get(String::New("sessionId"))->Int32Value();
            wecandoit = true;
        }
        if(sess->Has(String::New("connectCallback")))
        {
            Local<Function> concb = Function::Cast(*(sess->Get(String::New("connectCallback"))));
            ConCB = Persistent<Function>::New(concb);
        }
        if(sess->Has(String::New("recieveCallback")))
        {
            Local<Function> reccb = Function::Cast(*(sess->Get(String::New("recieveCallback"))));
            RecCB = Persistent<Function>::New(reccb);
        }
    }
    if(wecandoit)
    {
        _session[sess_id] = irc_create_session(&_callbacks);
        irc_set_ctx(_session[sess_id], (void *) sess_id);
        return(Integer::New(0));
    }
    else
    {
        return(Integer::New(1));
    }
}

//JS equivalent for irc_connect command
//parameters:
//Connect(String server, Number port, String password, String nick, String username, String realname)
static Handle<Value> Connect(const Arguments &args)
{
    printf("DEBUG:: connect\n");
    unsigned int sess_id = args[0]->Int32Value();
    V8STR server(args[1]);
    int port = args[2]->Int32Value();
    V8STR password(args[3]);
    V8STR nick(args[4]);
    V8STR username(args[5]);
    V8STR realname(args[6]);
    irc_connect(_session[sess_id], *server, port, *password, *nick, *username, *realname);
}

//JS equivalent for irc_run command
//parameters:
//No
static Handle<Value> Run(const Arguments &args)
{
    printf("DEBUG:: run\n");
    unsigned int sess_id = args[0]->Int32Value();
    pthread_t thread;
    ev_async_init(&eio_nt, con_cb_ev);
    ev_async_start(EV_DEFAULT_UC_ &eio_nt);
    ev_async_init(&eio_rc, rec_cb_ev);
    ev_async_start(EV_DEFAULT_UC_ &eio_rc);
    pthread_create(&thread, NULL, run_thr, (void *) sess_id);
    ev_loop(EV_DEFAULT_ EVLOOP_NONBLOCK);
}

//JS equivalent for irc_cmd_join command
//parameters:
//Join(String channel, String key)
static Handle<Value> Join(const Arguments &args)
{
    printf("DEBUG:: join\n");
    unsigned int sess_id = args[0]->Int32Value();
    V8STR chan(args[1]);
    V8STR key(args[2]);
    irc_cmd_join(_session[sess_id], *chan, *key);
}

//JS equivalent for irc_cmd_msg command
//parameters:
//SendMsg(String dest, String text)
static Handle<Value> SendMsg(const Arguments &args)
{
    printf("DEBUG:: sendmsg\n");
    unsigned int sess_id = args[0]->Int32Value();
    V8STR chan(args[1]);
    V8STR message(args[2]);
    irc_cmd_msg(_session[sess_id], *chan, *message);
}

void *run_thr(void *vptr_args)
{
    printf("DEBUG:: run_thr\n");
    irc_run(_session[(int)vptr_args]);
}

void rec_cb(irc_session_t *session, const char *event, const char *origin, const char **params, unsigned int count)
{
    printf("DEBUG:: rec_cb\n");
    mess *newm = new mess;
    newm->session = (int) irc_get_ctx(session);
    newm->origin = origin;
    newm->params = params;
    ev_set_userdata(newm);
    ev_async_send(EV_DEFAULT_UC_ &eio_rc);
}

void con_cb(irc_session_t *session, const char *event, const char *origin, const char **params, unsigned int count)
{
    printf("DEBUG:: con_cb\n");
    ev_set_userdata(irc_get_ctx(session));
    ev_async_send(EV_DEFAULT_UC_ &eio_nt);
}

void rec_cb_ev(EV_P_ ev_async *watcher, int revents)
{
    printf("DEBUG:: rec_cb_ev\n");
    mess *parser;
    parser = (mess *)ev_userdata();
    Handle<Value> args[2];
    args[0] = Integer::New(parser->session);
    args[1] = String::New(parser->origin);
    args[2] = String::New(parser->params[1]);
    if (RecCB->IsFunction())
    {
        Handle<Object> tmp = Object::New();
        RecCB->Call(tmp, 3, args);
    }
}

void con_cb_ev(EV_P_ ev_async *watcher, int revents)
{
    printf("DEBUG:: con_cb_ev, session = %d\n", (int) ev_userdata());
    if (ConCB->IsFunction())
    {
        Handle<Object> tmp = Object::New();
        ConCB->Call(tmp, 0, NULL);
    }
}


extern "C" void
init (Handle<Object> target)
{
    HandleScope scope;
    Local<FunctionTemplate> t = FunctionTemplate::New();
    Local<Template> proto_t = t->PrototypeTemplate();
    proto_t->Set("Connect", FunctionTemplate::New(Connect));
    proto_t->Set("Run", FunctionTemplate::New(Run));
    proto_t->Set("CreateSession", FunctionTemplate::New(CreateSession));
    proto_t->Set("Join", FunctionTemplate::New(Join));
    proto_t->Set("SendMsg", FunctionTemplate::New(SendMsg));
    target->Set(String::NewSymbol("func"), t->GetFunction());
}
