#include <node.h>
#include <stdio.h>
#include <node.h> 
#include <node_events.h> 
#include <pthread.h>
#include <libircclient/libircclient.h>
#define V8STR String::AsciiValue

using namespace v8; 

typedef struct message
{
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
irc_session_t *_session;
struct ev_async eio_nt;
struct ev_async eio_rc;

//JS equivalent for irc_create_session command
//parameters:
//CreateSession(Integer SessionID, Function RecCB, Function ConCB) //RecCB - a callback function for recieving a message from some user to channel/personally
//ConCB - a callback for "connected" event on IRC server
static Handle<Value> CreateSession(const Arguments &args)
{
    _callbacks.event_channel = rec_cb;
    _callbacks.event_connect = con_cb;
    bool wecandoit = false;
    if (args[0]->IsObject())
    {
        Local<Object> sess = Object::Cast(*args[0]);
        if(sess->Has(String::New("sessionId")))
        {
            _session = irc_create_session(&_callbacks);
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
    V8STR server(args[0]);
    int port = args[1]->Int32Value();
    V8STR password(args[2]);
    V8STR nick(args[3]);
    V8STR username(args[4]);
    V8STR realname(args[5]);
    irc_connect(_session, *server, port, *password, *nick, *username, *realname);
}

//JS equivalent for irc_run command
//parameters:
//No
static Handle<Value> Run(const Arguments &args)
{
    pthread_t thread;
    ev_async_init(&eio_nt, con_cb_ev);
    ev_async_start(EV_DEFAULT_UC_ &eio_nt);
    ev_async_init(&eio_rc, rec_cb_ev);
    ev_async_start(EV_DEFAULT_UC_ &eio_rc);
    pthread_create(&thread, NULL, run_thr, NULL);
    ev_loop(EV_DEFAULT_ EVLOOP_NONBLOCK);
}

//JS equivalent for irc_cmd_join command
//parameters:
//Join(String channel, String key)
static Handle<Value> Join(const Arguments &args)
{
    V8STR chan(args[0]);
    V8STR key(args[1]);
    irc_cmd_join(_session, *chan, *key);
}

//JS equivalent for irc_cmd_msg command
//parameters:
//SendMsg(String dest, String text)
static Handle<Value> SendMsg(const Arguments &args)
{
    V8STR chan(args[0]);
    V8STR message(args[1]);
    irc_cmd_msg(_session, *chan, *message);
}

void *run_thr(void *vptr_args)
{
    HandleScope scope;
    irc_run(_session);
}

void rec_cb(irc_session_t *session, const char *event, const char *origin, const char **params, unsigned int count)
{
    mess *newm = new mess;
    newm->origin = origin;
    newm->params = params;
    ev_set_userdata(newm);
    ev_async_send(EV_DEFAULT_UC_ &eio_rc);
}

void con_cb(irc_session_t *session, const char *event, const char *origin, const char **params, unsigned int count)
{
    ev_async_send(EV_DEFAULT_UC_ &eio_nt);
}

void rec_cb_ev(EV_P_ ev_async *watcher, int revents)
{
    mess *parser;
    parser = (mess *)ev_userdata();
    Handle<Value> args[2];
    args[0] = String::New(parser->origin);
    args[1] = String::New(parser->params[1]);
    if (RecCB->IsFunction())
    {
        Handle<Object> tmp = Object::New();
        RecCB->Call(tmp, 2, args);
    }
}

void con_cb_ev(EV_P_ ev_async *watcher, int revents)
{
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
