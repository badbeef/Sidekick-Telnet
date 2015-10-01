/* 

The MIT License (MIT)

Copyright (c) 2014 Chui-Tin Yen

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

*/

#include "ring.h"
#include "defines.h"
#include "externs.h"
#include "types.h"
#include "proto.h"
#include "terminal.h"
#include "sidekick.h"
#include <unistd.h>

myring scroll_buf;

int myring::init() 
{
    buf = new char[size];
    head = 0;
    count = 0;
    return 1;
}   

void myring::write(const char *buffer, int ct) 
{
    for (int i = 0; i < ct; i++) {
        buf[head++] = buffer[i];
        if (head>=size) {
            head -= size;
        }
        count++;    // TODO: handle wrap around
    }
}
    
int myring::match(const char *string, unsigned int *offset) 
{
    unsigned int start, end, str_len, i;
    char c;
    str_len = strlen(string);
    if (count < size) {
        /* no wrap around yet */
        if (*offset < size) {
            start = *offset;
        } else {
            start = 0;
        }
        if ((count - start) < str_len) {
            return 0;
        }
        end = count - str_len;
    } else {
        if (count - *offset < size) {
            if (count - *offset < str_len) {
                /* no match */
                return 0;
            }
            start = *offset % size;
        } else {
            start = head;
        }
        if (head < str_len) {
            end = head + size - str_len;
        } else {
            end = head - str_len;
        }
    }
    while (1) {
        /* do matching */
        for (i = 0; i < str_len; i++) {
            if (start + i < size) {
                c = buf[start + i];
            } else {
                c = buf[start + i - size];
            }
            if (c != string[i]) {
                break;
            }
            if (i == str_len - 1) {
                /* we have a match! return the offset */
                if (start + str_len > size) {
                    *offset = (count / size - 1) * size + start + 1;
                } else {
                    *offset = count / size * size + start + 1;
                }
                return 1;
            }
        }
        if (start == end) {
            /* no match */
            *offset = count - str_len + 1;
            return 0;
        }
        if (++start >= size) {
            start = 0;
        }
    }
}

void myring::clear(unsigned int *offset)
{
    *offset = count;
}

Script myscript;

Script::Script()
{
    this->name = NULL;
    timestamp = 0;
    offset = 0;
    curr_token = NULL;
    first_token = NULL;
    next_script = NULL;
}

int Script::add(opcode cmd, char *string)
{
    token *t = new token;

    if (first_token) {
	t = first_token;
	while (t->next) {
	    t = t->next;
	}
	t->next = new token;
	t = t->next;
    } else {
	/* brand new script */
	t = new token;
	first_token = t;
	curr_token = t;
    }
    t->cmd = cmd;
    t->string = strdup(string);
    t->next = NULL;
    return 1;
}

int Script::add_line(char *line)
{
    int str_len = strlen(line);
    int i, j;

    if (line[0] == ' ') {
	/* check for blank lines */
	for (i = 1; i < str_len - 1; i++) {
	    if (line[i] != ' ') {
		return 0;
	    }
	}
	return 1;
    }
    if (str_len < 4) {
	/* invalid command */
	return 0;
    }

    /* clear the CR */
    line[str_len - 1] = 0;

    /* convert common escape characters */
    for (i = 0, j = 0; j < str_len; i++, j++) {
	if (line[j] == '\\') {
	    if (++j >= str_len) {
		return 0;
	    }
	    switch (line[j]) {
	    case 'a':
		line[i] = '\a';
		break;
	    case 'b':
		line[i] = '\b';
		break;
	    case 't':
		line[i] = '\t';
		break;
	    case 'n':
		line[i] = '\n';
		break;
	    case '\\':
		line[i] = '\\';
		break;
	    case 'r':
		line[i] = '\r';
		break;
	    default:
		return 0;
	    }
	} else {
	    line[i] = line[j];
	}
    }
    line[i] = 0;

    if (strncmp("NAME ", line, 5) == 0) {
	if (name) {
	    free(name);
	}
	name = strdup(&line[5]);
    } else if (strncmp("MTCH ", line, 5) == 0) {
	add(CMD_MATCH, &line[5]);
    } else if (strncmp("INJT ", line, 5) == 0) {
	add(CMD_INJECT, &line[5]);
    } else if (strncmp("INJL ", line, 5) == 0) {
	line[str_len - 1] = '\n';
	add(CMD_INJECT, &line[5]);
    } else if (strncmp("DELY ", line, 5) == 0) {
	add(CMD_DELAY, &line[5]);
    } else if (strncmp("CBUF ", line, 4) == 0) {
	add(CMD_CLEAR, (char *)"");
    } else if (strncmp("END", line, 3) == 0) {
	add(CMD_END, (char *)"");
    } else if (strncmp("ENDS", line, 4) == 0) {
	add(CMD_END, (char *)"");
    } else if (strncmp("RSTA", line, 4) == 0) {
	add(CMD_RESTART, (char *)"");
    } else if (strncmp("LABL", line, 4) == 0) {
	add(CMD_LABEL, &line[5]);
    } else if (strncmp("GOTO", line, 4) == 0) {
	add(CMD_GOTO, &line[5]);
    } else if (strncmp("SKIF", line, 4) == 0) {
	add(CMD_SKIPIF, &line[5]);
    } else if (strncmp("WAIT", line, 4) == 0) {
	add(CMD_WAIT, &line[5]);
    } else if (strncmp("GIVE", line, 4) == 0) {
	add(CMD_GIVE, &line[5]);
    } else if (strncmp("DIST", line, 4) == 0) {
	add(CMD_DISTRIB, &line[5]);
    } else if (strncmp("NEWF", line, 4) == 0) {
	add(CMD_NEWFILE, &line[5]);
    } else if (strncmp("DELF", line, 4) == 0) {
	add(CMD_DELETE, &line[5]);
    } else if (strncmp("FILE", line, 4) == 0) {
	add(CMD_FILE, &line[5]);
    } else {
	return 0;
    }
    return 1;
}

int Script::load(char *file)
{
    char line[128];
    int i = 0;
    FILE *fp;
    Script *s = NULL;
    fp = fopen(file, "r");
    if (!fp) {
	return 2;
    }
    while (fgets(line, sizeof(line), fp)) {
	i++;
	if (line[0] == '#' || line[0] == '\n') {
	    /* comment */
	    continue;
	}
	if (strncmp("NAME ", line, 5) == 0) {
	    if (s) {
		s->next_script = new Script;
		s = s->next_script;
	    } else {
		s = this;
	    }
	}
	if (!s || !s->add_line(line)) {
	    fprintf(stderr, "\nFailed to parse line %d: '%s'\n", i, line);
	    return 0;
	}
    }
    fclose(fp);
    return 1;
}

void Script::give(opcode oc, char *signal) 
{
    if (curr_token->cmd == CMD_WAIT &&
	strcmp(curr_token->string, signal) == 0) {
	/* unblocking the WAIT, assuming WAIT is not the last command */
	curr_token = curr_token->next;
	if (oc == CMD_GIVE) {
	    /* no need to continue */
	    return;
	}
    }
    if (next_script) {
	next_script->give(oc, signal);
    }
}

void Script::run()
{
    char *jump_label;
    struct passwd *pw;
    char file[200];
    FILE *fp;

    if (!curr_token) {
	goto next;
    }
    /* loop through all tokens */
    while (curr_token) {
	switch(curr_token->cmd) {
	case CMD_MATCH:
	    if (!scroll_buf.match(curr_token->string, &offset)) {
		/* next time: restart from here */
		goto next;
	    }
	    break;
	case CMD_SKIPIF:
	    if (scroll_buf.match(curr_token->string, &offset)) {
		/* skip one command */
		if (curr_token->next) {
		    curr_token = curr_token->next;
		}
	    }
	    break;
	case CMD_INJECT:
	    ttyiring.write(curr_token->string, strlen(curr_token->string));
	    break;
	case CMD_DELAY:
	    if (timestamp) {
		/* check if timestamp exceeded */
		if (time(NULL) < timestamp) {
		    goto next;
		}
		/* time exceeded, proceed to next command */
		timestamp = 0;
	    } else {
		/* new wait command */
		timestamp = time(NULL) + atoi(curr_token->string);
		goto next;
	    }
	    break;
	case CMD_CLEAR:
	    scroll_buf.clear(&offset);
	    break;
	case CMD_RESTART:
	    curr_token = first_token;
	    continue;
	case CMD_END:
	    goto next;
	case CMD_LABEL:
	    /* NOP when executing */
	    break;
	case CMD_GOTO:
	    jump_label = curr_token->string;
	    curr_token = first_token;
	    while (curr_token->next) {
		if (curr_token->cmd == CMD_LABEL &&
		    strcmp(curr_token->string, jump_label) == 0) {
		    break;
		}
		curr_token = curr_token->next;
	    }
	    continue;
	case CMD_WAIT:
	    /* wait until someone free me */
	    goto next;
	case CMD_GIVE:
	case CMD_DISTRIB:
	    /* start from first script and repeat */
	    myscript.give(curr_token->cmd, curr_token->string);
	    break;
	case CMD_NEWFILE:
	    pw = getpwuid(getuid());
	    sprintf(file, "/tmp/stelnet.%s.%s", 
		    pw->pw_name, curr_token->string);
	    fp = fopen(file, "w");
	    if (fp) {
		fclose(fp);
	    }
	    break;
	case CMD_DELETE:
	    pw = getpwuid(getuid());
	    sprintf(file, "/tmp/stelnet.%s.%s", 
		    pw->pw_name, curr_token->string);
	    remove(file);
	    break;
	case CMD_FILE:
	    if (timestamp) {
		/* check if timestamp exceeded */
		if (time(NULL) < timestamp) {
		    goto next;
		}
	    } 
	    pw = getpwuid(getuid());
	    sprintf(file, "/tmp/stelnet.%s.%s", 
		    pw->pw_name, curr_token->string);
	    fp = fopen(file, "r");
	    if (fp) {
		/* file found, proceed */
		fclose(fp);
		break;
	    }
	    /* file not found, arm the timer and try again */
	    timestamp = time(NULL) + 1;
	    goto next;
	default:
	    break;
	}
	/* advance to next token */
	curr_token = curr_token->next;
    }
next:
    if (next_script) {
	return this->next_script->run();
    }
}

void loadScript(char *host, char *port)
{
    struct passwd *pw = getpwuid(getuid());
    char script_file[250];
    int rc;
    
    if (port) {
	sprintf(script_file, "%s/.telnetrc/%s-%s.tel", pw->pw_dir, host, port);
    } else {
	sprintf(script_file, "%s/.telnetrc/%s.tel", pw->pw_dir, host);
    }
    fprintf(stderr, "Sidekick-Telnet loading %s ... ", script_file);
    rc = myscript.load(script_file);

    switch(rc) {
    case 0:
	exit(1);
    case 1: 
	fprintf(stderr, "Done\n");
	break;
    case 2:
	fprintf(stderr, "Not Found\n");
	break;
    }
}

void tin_machine_run()
{
    myscript.run();
}       


