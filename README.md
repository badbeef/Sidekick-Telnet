stelnet
=======

Sidekick for Telnet - allows you to run custom script when inside a telnet session.

It run scripts in the background. When the program starts, it tries to
find a script file under ~/.telnetrc/ e.g. "~/.telnetrc/ts1-2001.tel"
and load it. After that, the scripts will be standing by, "typing" words
on your behave. While at the same time, you have full control over
console like normal telnet.

Support commands include - match, inject, delay, label, goto, etc. See README file
inside for details.
