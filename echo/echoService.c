#include "csapp.h"
#include "echo.h"

void echo(int connfd);

int main(int argc, char *argv[]) {
  /*
    - linstenfd : 듣기 식별자
  */

  if (argc != 2) {
    fprintf(stderr, "usage: %s <port>\n", argv[0]);
    exit(0);
  }

  int listenfd, connfd;
  socklen_t clientlen;
  struct sockaddr_storage clientaddr;
  char client_hostname[MAXLINE], client_port[MAXLINE];
  char *port = NULL;

  port = argv[1];

  listenfd = Open_listenfd(port);
  while (1) {
    clientlen = sizeof(struct sockaddr_storage);
    /*
      [await function]
      Accept : 클라이언트로의 연결 요청을 기다린다
      @ listenfd: 서버 소켓 디스크립터
      @ clientaddr: 클라이언트 주소 정보를 저장할 구조체
      @ clientlen: 클라이언트 주소 정보 구조체의 크기

      @ return: 클라이언트와 통신하기 위해 사용하는 **연결 식별자**
    */
    connfd = Accept(listenfd, (SA *)&clientaddr, &clientlen);
    Getnameinfo((SA *)&clientaddr, clientlen, client_hostname, MAXLINE,
                client_port, MAXLINE, 0);
    printf("Connected to (%s, %s)\n", client_hostname, client_port);
    echo(connfd);
    Close(connfd);
  }

  return 0;
}
