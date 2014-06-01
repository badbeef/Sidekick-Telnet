stelnet
=======

Sidekick for Telnet - allows you to run custom script when inside a telnet session.

It run scripts in the background. When the program starts, it tries to
find a script file under ~/.telnetrc/ e.g. "~/.telnetrc/ts1-2001.tel"
and load it. After that, the scripts will be standing by, "typing" words
on your behave. While at the same time, you have full control over
console like normal telnet.

To build stelnet, simply run "make". I hope Makefile is simple enough.
Tested on Mac OSX and Linux (RedHat with glibc).

Here is a sample script file. I hope the scripts are self-explanatory.
(Any line starting with '#' is ignored, so you can Copy & Paste the
following to your .tel file)

# Commands:
# NAME - Mandatory. Indicates start of a script
# ENDS - indicates the script ended
# MTCH - matches a string (Does not support regex. Also watch out for
#        invisible whitespaces in the end)
# INJT - injects a string (may use \a \b \n \r \t \\)
# INJL - injects a string and add \n in the end
# DELY - delays for number of seconds
# CBUF - clear the buffer so MTCH will start flesh
# RSTA - Restarts the script from beginning
# LABL - string label for branching (JUMP / JPIF)
# GOTO - Goto to specified string LABL within the same script. Script
#        will end if no matching LABL string.
# SKIF - skip next command if matched a string, best used after DELY
#        (combine it with GOTO to form a while loop)
# WAIT - wait for a signal from another script
# GIVE - give a signal to 1 other script that is waiting on the signal
# DIST - distribute a signal to all scripts waiting for the signal
# NEWF - create a 0 byte file /tmp/tin.<userid>.<string>, NOP if file presents
# DELF - delete a file if it exists
# FILE - wait until a file exists


