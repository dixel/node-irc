#include "binding.h"

static Handle<Value> CreateSession(const Arguments &args)
{
    printf("DEBUG:: createsession\n");
    
    _callbacks.event_channel = cmn_cb;
    _callbacks.event_connect = cmn_cb;
    _callbacks.event_join = cmn_cb;
    _callbacks.event_part = cmn_cb;
    _callbacks.event_nick = cmn_cb;

    sess_cnt++;
    Local<Object> Session = Object::New();
    Session->Set(String::New("sess_id"), Integer::New(sess_cnt));

    if (args[0]->IsObject())
    {
        Local<Object> userobj = Object::Cast(*args[0]);
        UserCB[sess_cnt] = Persistent<Object>::New(userobj);
    }

    _session[sess_cnt] = irc_create_session(&_callbacks);
    irc_set_ctx(_session[sess_cnt], new int(sess_cnt));
    return(Session);
}

int strip_sess(Handle<Value> sess)
{
    if(sess->IsObject())
    {
        Local<Object> stress = Object::Cast(*sess);
        if (stress->Has(String::New("sess_id")))
        {
            return stress->Get(String::New("sess_id"))->Int32Value();
        }
        else
            return 0;
    }
    else
        return 0;
}


static Handle<Value> Connect(const Arguments &args)
{
    printf("DEBUG:: connect\n");
    unsigned int sess_id = strip_sess(args[0]);

    V8STR server(args[1]);
    int port = args[2]->Int32Value();
    V8STR password(args[3]);
    V8STR nick(args[4]);
    V8STR username(args[5]);
    V8STR realname(args[6]);
    return Integer::New(irc_connect(_session[sess_id], *server, port, *password, *nick, *username, *realname));
}

static Handle<Value> Disconnect(const Arguments &args)
{
    printf("DEBUG:: disconnect\n");
    unsigned int sess_id = strip_sess(args[0]);
    UserCB[sess_id].Dispose();
    irc_disconnect(_session[sess_id]);
}

static Handle<Value> Run(const Arguments &args)
{
    printf("DEBUG:: run\n");
    unsigned int sess_id = strip_sess(args[0]);
    pthread_t thread;
    ev_async_init(&eio_cm, cmn_cb_ev);
    ev_async_start(EV_DEFAULT_UC_ &eio_cm);
    pthread_create(&thread, NULL, run_thr, new int (sess_id));
    ev_loop(EV_DEFAULT_ EVLOOP_NONBLOCK);
}

static Handle<Value> Join(const Arguments &args)
{
    printf("DEBUG:: join\n");
    unsigned int sess_id = strip_sess(args[0]);
    V8STR chan(args[1]);
    V8STR key(args[2]);
    return Integer::New(irc_cmd_join(_session[sess_id], *chan, *key));
}

static Handle<Value> Part(const Arguments &args)
{
    printf("DEBUG:: part\n");
    unsigned int sess_id = strip_sess(args[0]);
    V8STR chan(args[1]);
    return Integer::New(irc_cmd_part(_session[sess_id], *chan));
}

static Handle<Value> Topic(const Arguments &args)
{
    printf("DEBUG:: topic\n");
    unsigned int sess_id = strip_sess(args[0]);
    const char *chan = *(V8STR(args[1]));
    const char *topic = *(V8STR(args[2]));
    return Integer::New(irc_cmd_topic(_session[sess_id], chan, topic));
}

static Handle<Value> Kick(const Arguments &args)
{
    printf("DEBUG:: kick\n");
    unsigned int sess_id = strip_sess(args[0]);
    const char *nick = *(V8STR(args[1]));
    const char *chan = *(V8STR(args[2]));
    const char *reason = *(V8STR(args[3]));
    return Integer::New(irc_cmd_kick(_session[sess_id], nick, chan, reason));
}

static Handle<Value> Mode(const Arguments &args)
{
    printf("DEBUG:: mode\n");
    unsigned int sess_id = strip_sess(args[0]);
    const char *chan = *(V8STR(args[1]));
    const char *mode = *(V8STR(args[2]));
    return Integer::New(irc_cmd_channel_mode(_session[sess_id], chan, mode));
}

static Handle<Value> Invite(const Arguments &args)
{
    printf("DEBUG:: invite\n");
    unsigned int sess_id = strip_sess(args[0]);
    const char *nick = *(V8STR(args[1]));
    const char *chan = *(V8STR(args[2]));
    return Integer::New(irc_cmd_invite(_session[sess_id], nick, chan));
}

static Handle<Value> SendMsg(const Arguments &args)
{
    printf("DEBUG:: sendmsg\n");
    unsigned int sess_id = strip_sess(args[0]);
    V8STR chan(args[1]);
    V8STR message(args[2]);
    return Integer::New(irc_cmd_msg(_session[sess_id], *chan, *message));
}

void *run_thr(void *vptr_args)
{
    printf("DEBUG:: run_thr\n sess_id = %d\n", *reinterpret_cast<int*>(vptr_args));
    irc_run(_session[*reinterpret_cast<int*>(vptr_args)]);
    delete reinterpret_cast<int *>(vptr_args);
}

void cmn_cb(irc_session_t *session, const char *event, const char *origin, const char **params, unsigned int count)
{
    printf("DEBUG:: cmn_cb\n");
    cbvar *msg = new cbvar;
    msg->session = *reinterpret_cast<int *>(irc_get_ctx(session));
    strncpy(msg->event, event, EVENTLEN);
    strncpy(msg->origin, origin, ORIGINLEN);
    msg->params = params;
    ev_set_userdata(msg);
    ev_async_send(EV_DEFAULT_UC_ &eio_cm);
}
    
void cmn_cb_ev(EV_P_ ev_async *watcher, int revents)
{
    printf("DEBUG:: cmn_cb_ev\n");
    cbvar *msg = new cbvar(*(cbvar *)ev_userdata());
    printf("%s\n", msg->event);
    if (!strcmp(msg->event, "JOIN"))
    {
        call_func(msg, "joinCallback");
    }
    else if (!strcmp(msg->event, "PART"))
    {
        call_func(msg, "partCallback");
    }
    else if (!strcmp(msg->event, "NICK"))
    {
        call_func(msg, "nickCallback");
    }
    else if (!strcmp(msg->event, "CONNECT"))
    {
        call_func(msg, "connectCallback");
    }
    else if (!strcmp(msg->event, "PRIVMSG"))
    {
        call_func(msg, "recieveCallback");
    }
}

int call_func(cbvar *msg, const char *name)
{
    Handle<Value> func;
    if (UserCB[msg->session]->Has(String::New(name)))
    {
        func = UserCB[msg->session]->Get(String::New(name));
        if (func->IsFunction())
        {
            Handle<Object> tmp = Object::New();
            Handle<Value> args[4];
            int i = 0;
            printf("SESSION: %d  EVENT: %s  ORIGIN: %s  PARAMS[0]: %s\n", msg->session, msg->event, msg->origin, msg->params[0]);
            Handle<Object> sess = Object::New();
            sess->Set(String::New("sess_id"), Integer::New(msg->session));
            Handle<Object> params = Object::New();
            while (msg->params[i] != NULL)
            {
                params->Set(i, String::New(msg->params[i]));
                i++;
            }
            args[0] = sess;
            args[1] = String::New(msg->event);
            args[2] = String::New(msg->origin);
            args[3] = params;
            Local<Function> fn = Function::Cast(*(func));
            fn->Call(tmp, 4, args);
        }
    }
}

extern "C" void
init (Handle<Object> target)
{
    HandleScope scope;
    target->Set(String::NewSymbol("CreateSession"), FunctionTemplate::New(CreateSession)->GetFunction());
    target->Set(String::NewSymbol("Connect"), FunctionTemplate::New(Connect)->GetFunction());
    target->Set(String::NewSymbol("Run"), FunctionTemplate::New(Run)->GetFunction());
    target->Set(String::NewSymbol("Join"), FunctionTemplate::New(Join)->GetFunction());
    target->Set(String::NewSymbol("SendMsg"), FunctionTemplate::New(SendMsg)->GetFunction());
    target->Set(String::NewSymbol("Disconnect"), FunctionTemplate::New(Disconnect)->GetFunction());
    target->Set(String::NewSymbol("Part"), FunctionTemplate::New(Part)->GetFunction());
    target->Set(String::NewSymbol("Topic"), FunctionTemplate::New(Topic)->GetFunction());
    target->Set(String::NewSymbol("Mode"), FunctionTemplate::New(Mode)->GetFunction());
    target->Set(String::NewSymbol("Invite"), FunctionTemplate::New(Invite)->GetFunction());
    target->Set(String::NewSymbol("Kick"), FunctionTemplate::New(Kick)->GetFunction());
}
