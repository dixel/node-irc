# :node-irc:
_Simple binding of libircclient for node.js._

### 1. USAGE
    require("./build/default/binding")

### 2. MODULE DESCRIPTION
####Functions:
+ `Session CreateSession(Callbacks callbacks)`  
    Creates an unique javascript object `Session` which will identify your IRC session
+ `Number Connect(Session session, Number port, String password, String nick, String username, String realname)`  
    Creates connection to the server. Returns 0 if success.
+ `Run(Session session)`  
    Starts an IRC-session - processing callbacks, sending messages etc. Implemented non-blocking.
+ `Integer Disconnect(Session session)`  
    Disconnects from IRC-server, free memory ocupied by user-callbacks.
+ `Integer Join(Session session, String channel, String key)`  
    Join IRC-channel `channel` with passphrase `key`
+ `Integer SendMsg(Session session, String destination, String text)`  
    Send a message `text` to channel/nick `destination`
+ `Integer Part(Session session, String channel)`  
    Part channel `channel` and stop talking here.
+ `Integer Invite(Session session, String nick, String channel)`  
    Invite user `nick` to channel `channel`.
+ `Integer Mode(Session session, String channel, String mode)`  
    Set mode (ex. +b -t etc) `mode` for channel `channel`.
+ `Integer Kick(Session session, String nick, String channel, String reason)`  
    Kick user `nick` from `channel` for `reason`.
+ `Integer Topic(Session sessiom, String channel, String topic)`  
    Set `topic` for `channel`.

####Callbacks object fields:
All callback arguments are similar to their equivalents in [libircclient library](http://libircclient.sourceforge.net).

+ `connectCallback(Session session, String event, String origin, Array params)`  
+ `recieveCallback(Session session, String event, String origin, Array params)`  
+ `joinCallback(Session session, String event, String origin, Array params)`  
+ `partCallback(Session session, String event, String origin, Array params)`  
+ `nickCallback(Session session, String event, String origin, Array params)`  

****
### 3. COMMIT HISTORY
    10.03.11:   First commit: README, package.json files, TODO list
    17.03.11:   Added accessors for channel connection processing
    12.04.11:   Testing environment for libircclient and libev - testirc
    19.04.11:   Working tests: sending and recieving messages via console works well. Handling connected/recieved message events. New design of main module file (binding.cc)
    20.04.11:   CreateSession function prototype done. Arguments now are two callback functions
    20.04.11:   Persistent callback functions done. Now we can describe callbacks directly from javascript
    25.04.11:   Functions SendMsg, Join done, now everything can be done directly from JS
    27.04.11:   All about IRC events now works in another thread. ConnectCallback is working. Everything works in parallel with nodejs event loop. Web server and irc connection in example test.js
    28.04.11:   RecievedCallback events. Testing Web-page on 127.0.0.1:8100 - wait for connection to be viewed from console. Everything works fine, but we need a new arquitect to hold many irc connections. For showtime - small javascript which populates the page with new irc messages.
    03.05.11:   Testing web-page with chat window. Fixed some errors with memory management.
    11.05.11:   Many fixes, function Run() now works fine and is non-blocking. Time to begin with lots of sessions.
    16.05.11:   All callbacks for irc.SessionCreate(x) now are in a JavaScript object x.
    17.05.11:   Works with multiple sessions!
    17.05.11:   Two users in test.js: one for web-ui and one just bot.
    25.05.11:   Fixed bugs on 64-bit systems, state UML diagram.
    25.05.11:   Sequence UML diagram; header file binding.h with all necesarry information about JS functions.
    26.05.11:   Session now is an object returned by CreateSession(..); Disconnect function, callback fix.
    26.05.11:   Everything without prototypes. Just require("way_to_module"). You can choose nick on your testing page.
    30.05.11:   Common callback function written, base for implementing every one necessarry callback (joinCallback, partCallback for ex.); test.js modified for ex.)
    30.05.11:   Callbacks for: nick, part, join events. Testing script test.js.
    30.05.11:   Modified bot.
    30.05.11:   All callbacks now are dispatched by cmn_cb.
    31.05.11:   Modified documentation - view README.md/README.html.
    31.05.11:   Functions: Part, Invite, Kick, Mode, Topic. Test how bot leaves channel - test.js.
****
