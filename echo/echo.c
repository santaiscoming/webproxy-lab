#include "csapp.h"
/*
  @ connfd: Connection file descriptor
*/

void echo(int connfd) {
  /*
    - n: read한 byte 수
    - buf: read한 데이터를 저장할 buffer
  */
  size_t n;
  char buf[MAXLINE];
  rio_t rio;

  Rio_readinitb(&rio, connfd); /* 버퍼 초기화 */

  while ((n = Rio_readlineb(&rio, buf, MAXLINE)) != 0) {
    printf("server received %d bytes\n", (int)n);
    Rio_writen(connfd, buf, n);
  }
}