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

#ifndef _SIDEKICK_H_
#define _SIDEKICK_H_

#include <stdlib.h>
#include <string.h>
#include <pwd.h>
#include <time.h>

class myring {
public:
    int init();
    void write(const char *buffer, int ct);
    int match(const char *str, unsigned int *last);
    void clear(unsigned int *last);
protected:
    static const unsigned int size = 4096;
    char *buf;
    unsigned int head;
    unsigned int count;
};

typedef enum {
    CMD_MATCH,
    CMD_INJECT,
    CMD_DELAY,
    CMD_CLEAR,
    CMD_RESTART,
    CMD_END,
    CMD_LABEL,
    CMD_GOTO,
    CMD_SKIPIF,
    CMD_WAIT,
    CMD_GIVE,
    CMD_DISTRIB,
    CMD_NEWFILE,
    CMD_DELETE,
    CMD_FILE,
} opcode;
            
typedef struct _token {
    opcode cmd;
    char *string;
    struct _token *next;
} token;

class Script {
public:
    Script();
    int add(opcode cmd, char *string);
    int add_line(char *line);
    int load(char *file);
    void run();
protected:
    void give(opcode oc, char *signal);
    char *name;
    time_t timestamp;
    unsigned int offset;
    token *curr_token;
    token *first_token;
    Script *next_script;
};

#endif /* _SIDEKICK_H_ */
