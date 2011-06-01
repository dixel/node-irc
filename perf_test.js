#!/usr/bin/env node
irc = require("./lib/default/node-irc");
callbacks = new Object();
callbacks.connectCallback = function (session, even, origin, params)
{
    console.log('connected' + session.sess_id);
    irc.Join(session, "#test_node", "");
    irc.SendMsg(session, "dixel", "<here I am>");
}

session = [];
i = 0;

setInterval(function ()
{
    console.log('attempt ' + i);
    i++;
    session[i] = irc.CreateSession(callbacks);
    irc.Connect(session[i], "debian-6.local", 6667, null, "StrongNodeJs" + i, null, null);
    irc.Run(session[i]);
}, 5);
    
