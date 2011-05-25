#include <libircclient/libircclient.h>
#include <node.h>
#include <node_events.h>

using namespace v8;
using namespace node;

class IRC;
	public:
		
		irc_session_t* session;
		irc_callbacks_t* irc-cb;
		char* channel = "#nodejs";
		char* text;
		char* serv = "irc.freenode.net";
		char* pass = ""
		char* nick = "shooshpanchick";
		char* username = "shoo";
		char* realname = "shpanchik";
		unsigned short port = 6667;
		//this are the default values for connecting, for some of them created accessors

		static void initcon(Handle<Object> target)
		//initialize connection with default server
		{
			Handle<Context> context = Context::New();
			//creating a new context for collecting objects
			Context::Scope context_scope(context)
			HandleScope scope;
			Handle<ObjectTemplate> nodeirc_templ = ObjectTemplate::New();
			//an object template for irc processing
			nodeirc_templ->SetInheritanceFieldCount(1);
			session = irc_create_session(irc-cb);
			//create a new session automatically
			nodeirc_templ->SetAccessor(String::New("text"), TSet, TGet);
			nodeirc_templ->SetAccessor(String::New("chan"), CSet, CGet);
			node_irc_templ->Set(String::New("irc_connect"), FunctionTemplate::New(\
			irc_connect(session)));
			node_irc_templ->Set(String::New("irc_run"), FunctionTemplate::New(\
			irc_run(session)));
			node_irc_templ->Set(String::New("irc_msg"), FunctionTemplate::New(\
			irc_cmd_msg(session, channel, text)));
		}
		Handle<Value> TSet(Local<String> property, const AccessorInfo& info)
		{
			return String::New(text);
		}
		Handle<Value> TGet(Local<String> property, Local<Value> value, const AcessorInfo& info)
		{
			text = value;
		}
		Handle<Value> CSet(Local<String> property, const AccessorInfo& info)
		{
			return String::New(channel);
		}
		Handle<Value> CGet(Local<String> property, Local<Value> value, const AcessorInfo& info)
		{
			channel = value;
		}

		irc_event_callback_t cb::event_channel()
		{
		//a callback function for recieving a message from anyone on a channel - one of the libircclient irc_callback_t fields
		//yet nothing to do, have to work with libev or so on	
		}

extern "C" void init(Handle<Object> target)
{
    HandleScope handle_scope;
	IRC:initcon(target);
}
