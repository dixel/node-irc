#!/usr/bin/env node
var http = require('http');
var util = require('util');
var irc = require('./build/default/binding').func.prototype;

var hr_ref = '<html><meta http-equiv="refresh" content="10;url = ./"><meta http-equiv="Content-Type" content="text/html; charset=utf-8"><head><body><h1>simple IRC chat on irc.freenode.net #test_node</h1>'
var hr_noref = '<html><meta http-equiv="Content-Type" content="text/html; charset=utf-8"><head><body><h1>simple IRC chat on irc.freenode.net #test_node</h1>'
var ind = '<form method="get"> <input type="submit" name="submit" value="Connect"> </form> </body> </head> </html>'
var con = '</td></tr></table> <form method="get"> <input type="text" name="msg" size=20> Message <br> <input type="submit" name="submit" value="Send"> </form> </body> </head> </html>'
var tab = '<table noshade border=1 cellspacing=0 cellpadding=0 width=100%><tr><td><p align=center><b>irc.freenode.net #test_node</b></p>'
var nick = 'default_node_irc';
var serv = 'irc.freenode.net';
var chan = '#test_node';

var msgq = "";
var connected = 4;
var nickrx = /.+(?=!)/;

console.log('||||||||starting nodejs http server|||||||||||||');

function ConnectCallback()
{
    console.log('Connected to server, attempting to join...');
    irc.SendMsg("dixel", "<here I am>");
    irc.Join(chan, "");
    connected = 2;
}

function RecieveCallback(origin, message)
{
    console.log('>%s: %s', origin, message);
    msgq = msgq + "<b>"+ origin.match(nickrx)+"</b>: " + message + "<br>";
}

http.createServer(function(request, response){
    if(request.method == 'GET')
    {
        query = require('url').parse(request.url, true);

        if(query.query.submit == 'Send')
        {
            console.log('attempting to send a message');
            irc.SendMsg(chan, query.query.msg);
            response.writeHead(200, {"Content-Type": "text/html"});
            response.write(hr_ref, encoding = 'utf-8');
            response.write(tab, encoding = 'utf-8');
            msgq = msgq + "<b>" + nick + "</b>: " + query.query.msg + "<br>";
            response.write(msgq, encoding = 'utf-8');
            response.end(con);
        }

        else if (query.query.submit == 'Connect' && connected == 4)
        {
            var x = new Object();
            x.sessionId = 1;
            x.recieveCallback = RecieveCallback;
            x.connectCallback = ConnectCallback;
            irc.CreateSession(x);
            //irc.CreateSession(ConnectCallback, RecieveCallback);
            irc.Connect(serv, 6667, null, nick, null, null);
            console.log('connected');
            irc.Run();
            console.log('runned');
            connected = 3;
            response.writeHead(200, {"Content-Type": "text/html"});
            response.write(hr_ref, encoding = 'utf-8');
            response.end("<p>Connecting...</p>", encoding='utf-8');
        }

        else if (connected == 1 || connected == 3)
        {
            response.writeHead(200, {"Content-Type": "text/html"});
            response.write(hr_ref, encoding = 'utf-8');
            response.end("<p>Connecting...</p>", encoding='utf-8');
        }
        
        else if (connected == 2)
        {
            response.writeHead(200, {"Content-Type": "text/html"});
            response.write(hr_ref, encoding = 'utf-8');
            response.write(tab, encoding = 'utf-8');
            response.write(msgq, encoding = 'utf-8');
            response.end(con);
        }

        else
        {
            response.writeHead(200, {"Content-Type": "text/html"});
            response.write(hr_noref, encoding = 'utf-8');
            response.end(ind);
        }

    }
    }).listen(8100);
console.log('web server running on 127.0.0.1:8100');
