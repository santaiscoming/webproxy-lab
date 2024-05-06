#include "csapp.h"
/*
  main의 parameter로 명령줄 argment를 받는다.
  - argc: argument count
  - argv: argument vector (동적배열 like JS array)
    - argv[0]: 프로그램 이름
    - argv[1~n]: 프로그램에 전달된 argument

  ex) ./echoclient host(arg[1]) port(arg[2]) ...
      - argc: 3
*/

int main(int argc, char *argv[]) {
  /*
    - clinefd: Client line file descriptor
    - MAXLINE: Maximum text line length
  */

  int clinefd;
  char *host, *port, buf[MAXLINE];
  rio_t rio;

  if (argc != 3) {
    fprintf(stderr, "usage: %s <host> <port>\n", argv[0]);
    exit(0);
  }
  host = argv[1];
  port = argv[2];

  clinefd = Open_clientfd(host, port); /* Open client connection */
  Rio_readinitb(&rio, clinefd);        /* 버퍼 초기화 */

  while (Fgets(buf, MAXLINE, stdin) != NULL) {
    Rio_writen(clinefd, buf, strlen(buf)); /* Write buf to server */
    Rio_readlineb(&rio, buf, MAXLINE);     /* Read buf from server */
    Fputs(buf, stdout);                    /* Write buf to stdout */
  }

  Close(clinefd); /* Close client connection */
  exit(0);
}

//  inet 172.17.0.2  netmask 255.255.0.0  broadcast 172.17.255.255