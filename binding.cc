#include <node.h>
#include <stdio.h>
#include <node.h>
#include <node_events.h> 
#include <libircclient/libircclient.h>
#define V8STR String::AsciiValue

using namespace v8; 
void rec_cb(irc_session_t *session, const char *event, const char *origin, const char **params, unsigned int count);
void con_cb(irc_session_t *session, const char *event, const char *origin, const char **params, unsigned int count);

Persistent<Function> RecCB;
Persistent<Function> ConCB;

irc_callbacks_t _callbacks;
irc_session_t *_session;
bool _connected = false;


//JS equivalent for irc_create_session command on libircclient lib
//parameters:
//CreateSession(Function RecCB, Function ConCB)
//RecCB - a callback function for recieving a message from some user to channel/personally
//ConCB - a callback for "connected" event on IRC server
static Handle<Value> CreateSession(const Arguments &args)
{
	printf("Create session\n");
	_callbacks.event_channel = rec_cb;
	_callbacks.event_connect = con_cb;
	_session = irc_create_session(&_callbacks);
	if (args[0]->IsFunction()) 
	{ 
		Local<Function> concb = Function::Cast(*args[0]);
		if (concb->IsFunction())
		{
			ConCB = Persistent<Function>::New(concb);
		}
	}
	if (args[1]->IsFunction()) 
	{ 
		Local<Function> reccb = Function::Cast(*args[1]);
		if (reccb->IsFunction())
		{
			RecCB = Persistent<Function>::New(reccb);
		}
	}
}

//JS equivalent for connect command on libircclient lib
//parameters:
//Connect(String server, Number port, String password, String nick, String username, String realname)
static Handle<Value> Connect(const Arguments &args)
{
	printf("Connect action\n");
	V8STR server(args[0]);
	int port = args[1]->Int32Value();
	V8STR password(args[2]);
	V8STR nick(args[3]);
	V8STR username(args[4]);
	V8STR realname(args[5]);
	printf("connect: %d\n", irc_connect(_session, *server, port, *password, *nick, *username, *realname));
}

//JS equivalent for irc_run command on libircclient lib
//parameters:
//No
static Handle<Value> Run(const Arguments &args) {
	printf("Run action\n");
	printf("run: %d", irc_run(_session));
}

//JS equivalent for irc_run command on libircclient lib
//parameters:
//Join(String channel, String key)
static Handle<Value> Join(const Arguments &args)
{
	printf("Join action\n");
	V8STR chan(args[0]);
	V8STR key(args[1]);
	printf("join: %d\n", irc_cmd_join(_session, *chan, *key));
}

void rec_cb(irc_session_t *session, const char *event, const char *origin, const char **params, unsigned int count)
{
	printf("NEW MESSAGE!\n%s: %s\n", origin, params[1]);
	if (RecCB->IsFunction())
	{
		Handle<Object> tmp = Object::New();
		RecCB->Call(tmp, 0, NULL);
	}
}

void con_cb(irc_session_t *session, const char *event, const char *origin, const char **params, unsigned int count)
{
	printf("CONNECTED!\n");
	if (RecCB->IsFunction())
	{
		Handle<Object> tmp = Object::New();
		ConCB->Call(tmp, 0, NULL);
	}

	_connected = true;
	printf("join: %d\n", irc_cmd_join(_session, "#newchan", NULL));
	irc_cmd_msg(_session, "#newchan", "HeLlO WoRlD!");
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
	target->Set(String::NewSymbol("func"), t->GetFunction());
}
