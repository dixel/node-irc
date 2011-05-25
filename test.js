#!/usr/bin/env node
var http = require('http');
var util = require('util');
var fs = require('fs');
var ind;
var con;
var msgq = "";
var header = '<html><meta http-equiv="refresh" content="10;url = ./"><meta http-equiv="Content-Type" content="text/html; charset=utf-8"><head><body><h1>simple IRC chat on irc.freenode.net #newchan</h1>'
var heade = '<html><meta http-equiv="Content-Type" content="text/html; charset=utf-8"><head><body><h1>simple IRC chat on irc.freenode.net #newchan</h1>'
var nodeirc = require('./build/default/binding');
irc = nodeirc.func.prototype;
var connected = 4;

console.log('starting nodejs');

fs.readFile('./index.html','utf-8',function (err, data) {
            if (err) throw err;
            ind = data; });
fs.readFile('./con.html','utf-8',function (err, data) {
            if (err) throw err;
            con = data; });

function ConnectCallback()
{
    console.log('Connected');
    irc.SendMsg("dixel", "Hello dixel, I successfully connected!");
    irc.Join("#newchan", "");
    irc.SendMsg("#newchan", "Hello everybody, I successfully joined!");
    irc.Join("#pieceofp", "");
    irc.SendMsg("#pieceofp", "Hello, pieces!");
    connected = 2;
}

function RecieveCallback(origin, message)
{
    console.log('%s: %s', origin, message);
    msgq = msgq + ": " + message + "<br>";
}


http.createServer(function(request, response){
    if(request.method == 'GET')
    {
        query = require('url').parse(request.url, true);

        if(query.query.submit == 'Send')
        {
            console.log('attempting to send');
            irc.SendMsg("#newchan", query.query.msg);
            response.writeHead(200, {"Content-Type": "text/html"});
            response.write(header, encoding = 'utf-8');
            response.write('<table noshade border=1 cellspacing=0 cellpadding=0><tr><td>', encoding = 'utf-8');
            msgq = msgq + query.query.msg + "<br>";
            response.write(msgq, encoding = 'utf-8');
            response.end(con);
        }

        else if (query.query.submit == 'Connect' && connected == 4)
        {
            irc.CreateSession(ConnectCallback, RecieveCallback);
            irc.Connect("irc.freenode.net", 6667, null, "test_node_irc", null, null);
            connected = 1;
            response.writeHead(200, {"Content-Type": "text/html"});
            response.write(header, encoding = 'utf-8');
            response.end("<p>Connecting...</p>", encoding='utf-8');
        }

        else if (connected == 1 || connected == 3)
        {
            response.writeHead(200, {"Content-Type": "text/html"});
            response.write(header, encoding = 'utf-8');
            response.end("<p>Connecting...</p>", encoding='utf-8');
        }
        
        else if (connected == 2)
        {
            response.writeHead(200, {"Content-Type": "text/html"});
            response.write(header, encoding = 'utf-8');
            response.write('<table noshade border=1 cellspacing=0 cellpadding=0><tr><td>', encoding = 'utf-8');
            response.write(msgq, encoding = 'utf-8');
            response.end(con);
        }

        else
        {
            response.writeHead(200, {"Content-Type": "text/html"});
            response.write(heade, encoding = 'utf-8');
            response.end(ind);
        }

    }
    }).listen(8100);
console.log('web server running on 127.0.0.1:8100');
setInterval(function () {
if (connected == 1)
{
    connected = 3;
    irc.Run();
}
}, 5000);
