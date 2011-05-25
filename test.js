#!/usr/bin/env node
var http = require('http');
var util = require('util');
var fs = require('fs');
var ind;
var nodeirc = require('./build/default/binding');
irc = nodeirc.func.prototype;
var connected = 0;
fs.readFile('./index.html','utf-8',function (err, data) {
			if (err) throw err;
			ind = data; });
console.log('starting nodejs');

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


http.createServer(function(request, response){
	if(request.method == 'GET')
	{
		query = require('url').parse(request.url, true);
		console.log(query.msg);
		if (query.query.submit == 'Connect')
		{
			irc.CreateSession(ConnectCallback, RecieveCallback);
			irc.Connect("irc.freenode.net", 6667, null, "vasitronen", null, null);
			connected = 1;
			response.writeHead(200, {"Content-Type": "text/html"});
			response.write('<html><head><meta http-equiv="Content-Type" content="text/html; charset=utf-8"><body>', encoding = 'utf-8');
			response.write("<p>Connecting...</p>", encoding='utf-8');
			response.end(ind);
		}
		else if(query.query.submit == 'Send')
		{
			console.log('attempting to send');
			irc.SendMsg("#newchan", query.query.msg);
			response.writeHead(200, {"Content-Type": "text/html"});
			response.write('<html><head><meta http-equiv="Content-Type" content="text/html; charset=utf-8"><body>', encoding = 'utf-8');
			response.end(ind);
		}
		else
		{
			response.writeHead(200, {"Content-Type": "text/html"});
			response.write('<html><head><meta http-equiv="Content-Type" content="text/html; charset=utf-8"><body>', encoding = 'utf-8');
			response.end(ind);
		}

	}
	}).listen(8100);
setInterval(function () {
if (connected == 1)
{
	connected = 0;
	irc.Run();
}
}, 5000);
