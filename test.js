#!/usr/bin/env node
var http = require('http');
var nodeirc = require('./build/default/binding');
var util = require('util');
util.debug("starting nodejs");
irc = nodeirc.func.prototype;
irc.Connect("irc.freenode.net", 6667,null,"vasitronen",null,null);
irc.Run();

