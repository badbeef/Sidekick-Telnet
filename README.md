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

Sample script (automatically reload a Cisco IOS router and boot from tftp):

```
NAME reload-tftp
MTCH reloadtftp
INJT \b\b\b\b\n
MTCH Proceed with reload? [confirm]
INJT \n
MTCH rommon 1
INJT b tftp:\n
RSTA 

NAME SKIP_CONFIG_SAVE
MTCH System configuration has been modified. Save? [yes/no]:
INJT no\n
RSTA 
```

The reload-tftp script does the reload. And the SKIP_CONFIG_SAVE bypasses saving config (optional). Both scripts will be matching for strings in parallel. In real life, the above script saves the user time because it takes anywhere from 20 to 40 seconds in between the prompts. And the user is freed from babysitting the console.

Usage beyond telnet
===================
Sidekick Telnet supports only telnet protocol, but it is possible to setup a local-access-only telnetd on a trusted machine, e.g. a Virtual Machine. And then the user may use stelnet to "telnet localhost" and start ssh, sftp or other interactive stream based applications. 


