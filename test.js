#!/usr/bin/env node
var http = require('http');
var util = require('util');
var irc = require('./build/default/binding').func.prototype;

var hr_ref = '<html><meta http-equiv="refresh" content="10;url = ./"><meta http-equiv="Content-Type" content="text/html; charset=utf-8"><head><body><h1>simple IRC chat on irc.freenode.net #test_node</h1>'
var hr_noref = '<html><meta http-equiv="Content-Type" content="text/html; charset=utf-8"><head><body><h1>simple IRC chat on irc.freenode.net #test_node</h1>'
var ind = '<form method="get"> <input type="submit" name="submit" value="Connect"> </form> </body> </head> </html>'
var con = '</td></tr></table> <form method="get"> <input type="text" name="msg" size=20> Message <br> <input type="submit" name="submit" value="Send"> <input type = "submit" name="submit" value = "Disconnect"> </form> </body> </head> </html>'
var tab = '<table noshade border=1 cellspacing=0 cellpadding=0 width=100%><tr><td><p align=center><b>irc.freenode.net #test_node</b></p>'
var nick = 'crazy_nodder_pic';
var serv = 'irc.freenode.net';
var chan = '#test_node';
var sess_1 = new Object();
var sess_2 = new Object();

var msgq = "";
var connected = 4;
var nickrx = /.+(?=!)/;

console.log('||||||||starting nodejs http server|||||||||||||');

function ConnectCallback(session)
{
    console.log('session #' + session.sess_id + 'Connected to server, attempting to join...');
    irc.SendMsg(session, "dixel", "<here I am>");
    irc.Join(session, chan, "");
    irc.SendMsg(sess_2, chan, "Hello, I am just a bot here!");
    connected = 2;
}

function RecieveCallback(session, origin, message)
{
    console.log('>%d %s: %s', session.sess_id, origin, message);
    if(session.sess_id == 1)
    {
        msgq = msgq + "<b>"+ origin.match(nickrx)+"</b>: " + message + "<br>";
    }
    if(session.sess_id == 2)
    {
        if (message.match(/.+test.+/))
        {
            irc.SendMsg(session, chan, origin.match(nickrx)+", you're so cute!");
        }
    }
}

http.createServer(function(request, response){
    if(request.method == 'GET')
    {
        query = require('url').parse(request.url, true);

        if(query.query.submit == 'Send')
        {
            console.log('attempting to send a message');
            irc.SendMsg(sess_1, chan, query.query.msg);
            response.writeHead(200, {"Content-Type": "text/html"});
            response.write(hr_ref, encoding = 'utf-8');
            response.write(tab, encoding = 'utf-8');
            msgq = msgq + "<b>" + nick + "</b>: " + query.query.msg + "<br>";
            response.write(msgq, encoding = 'utf-8');
            response.end(con);
        }

        if(query.query.submit == 'Disconnect')
        {
            console.log('disconnecting');
            irc.Disconnect(sess_1);
            irc.Disconnect(sess_2);
            response.writeHead(200, {"Content-Type": "text/html"});
            response.write(hr_noref, encoding = 'utf-8');
            response.end(ind);
            connected = 4;
        }

        else if (query.query.submit == 'Connect' && connected == 4)
        {
            var x = new Object();
            var y = new Object();
            x.recieveCallback = RecieveCallback;
            x.connectCallback = ConnectCallback;
            y.recieveCallback = RecieveCallback;
            y.connectCallback = ConnectCallback;
            sess_1 = irc.CreateSession(x);
            sess_2 = irc.CreateSession(y);
            irc.Connect(sess_1, serv, 6667, null, nick, null, null);
            irc.Connect(sess_2, serv, 6667, null, "testearnodeirc", null, null);
            console.log('connected');
            irc.Run(sess_1);
            irc.Run(sess_2);
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
