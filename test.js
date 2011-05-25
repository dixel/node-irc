#!/usr/bin/env node
var http = require('http');
var nodeirc = require('./build/default/binding');
var util = require('util');

console.log('starting nodejs');
irc = nodeirc.func.prototype;

function ConnectCallback()
{
	console.log('Connected');
	irc.SendMsg("dixel", "Hello dixel, I successfully connected!");
	irc.Join("#newchan", "");
	irc.SendMsg("#newchan", "Hello everybody, I successfully joined!");
	irc.Join("#pieceofp", "");
	irc.SendMsg("#pieceofp", "Hello, pieces!");
	return("Hello");
}

function RecieveCallback(origin, message)
{
	console.log('%s: %s', origin, message);
}

irc.CreateSession(ConnectCallback, RecieveCallback);
irc.Connect("irc.freenode.net", 6667, null, "vasitronen", null, null);
http.createServer(function(request, response){
	response.writeHead(200, {"Content-Type": "text/plain"});
	response.end("hello!\n");
	}).listen(8100);
irc.Run();
console.log("aFtErThrEad!");
