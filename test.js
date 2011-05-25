#!/usr/bin/env node
var http = require('http');
var nodeirc = require('./build/default/binding');
var util = require('util');
util.debug("starting nodejs");
irc = nodeirc.func.prototype;
function ConnectCallback()
{
	console.log('Connected');
	irc.SendMsg("dixel", "Hello dixel, I successfully connected!");
	irc.Join("#newchan", "");
	irc.SendMsg("#newchan", "Hello everybody, I successfully joined!");
	irc.Join("#pieceofp", "");
	irc.SendMsg("#pieceofp", "Hello, pieces!");
}
function RecieveCallback(origin, message)
{
	console.log('%s: %s', origin, message);
}
irc.CreateSession(ConnectCallback, RecieveCallback);
irc.Connect("irc.freenode.net", 6667,null,"vasitronen",null,null);
irc.Run();
