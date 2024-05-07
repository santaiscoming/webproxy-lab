#ifndef TINY_H
#define TINY_H

#include "csapp.h"

void doit(int fd);

void serve_static(int fd, char *filename, int filesize);
void serve_dynamic(int fd, char *filename, char *cgiargs);

void read_requesthdrs(rio_t *rp);

void get_filetype(char *filename, char *filetype);
int parse_uri(char *uri, char *filename, char *cgiargs);
void clienterror(int fd, char *cause, char *errnum, char *shortmsg,
                 char *longmsg);

void handle_SIGCHLD(int sig);

#endif