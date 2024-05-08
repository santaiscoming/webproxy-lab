/* $begin tinymain */
/*
 * tiny.c - A simple, iterative HTTP/1.0 Web server that uses the
 *     GET method to serve static and dynamic content.
 *
 * Updated 11/2019 droh
 *   - Fixed sprintf() aliasing issue in serve_static(), and clienterror().
 */
#include "tiny.h"
#include "csapp.h"

int main(int argc, char **argv) {
  /**
   * @var clientlen : 클라이언트 소켓의 크기를 저장하는 변수
   * - accept할때 client주소 정보를 저장하려면 크기를 알아야 하기에
   *
   * @var clientaddr : 모든 소켓을 받을 수 있는 구조체
   * - 접속할 클라이언트가 어떤 프로토콜을 준수하는 소켓이든 받을 수 있는 구조체
   * - ipv4, ipv6 / tcp, udp 모두 받을 수 있음
   */

  int listenfd, connfd;
  char hostname[MAXLINE], client_port[MAXLINE];
  char *service_port = NULL;
  socklen_t clientlen;
  struct sockaddr_storage clientaddr;

  /* Check command line args */
  if (argc != 2) {
    fprintf(stderr, "usage: %s <port>\n", argv[0]);
    exit(1);
  }

  /* after 11.8 */
  if (Signal(SIGCHLD, handle_SIGCHLD) == SIG_ERR) {
    fprintf(stderr, "Signal error\n");
    exit(1);
  }

  service_port = argv[1];
  listenfd = Open_listenfd(service_port); /* listen 소켓 open */
  /*
    이 루프에 대해서 한번 생각해보자 클라이언트가 connect를 보낼때가지 기다리고
    있다가 doit 을 호출한 이후 connfd를 close한다. 다시말해서 요청을 처리한
    뒤에는 소켓을 닫는다. 한번더 다시 말하면 클라이언트의 정보를 담고있는
    connfd는 일회성이다. 이는 statelss 프로토콜 (http)의 특징이다. 실제로 우리는
    url로 특정한 정보를 요청하고 그에 대한 응답을 받는다. 그리고 다시 리소스를
    원한다면 한번더 요청(url 접속)을 보내야한다.
  */
  while (1) { /* 전형적인 서버 무한 루프 */
    clientlen = sizeof(clientaddr);
    /* 반복적으로 연결 요청 -> ref.1 */
    connfd = Accept(listenfd, (SA *)&clientaddr, &clientlen);
    Getnameinfo((SA *)&clientaddr, clientlen, hostname, MAXLINE, client_port,
                MAXLINE, 0);
    printf("Accepted connection from (%s, %s)\n", hostname, client_port);
    doit(connfd); /* 트랜잭션 수행 */
    Close(connfd);
  }
}

/**
 * @brief 클라이언트에게 에러 메시지를 보내는 함수
 *
 * @param fd 클라이언트와 연결된 파일 디스크립터
 * @param cause 에러의 원인
 * @param errnum 에러 번호
 * @param shortmsg 에러 메시지
 * @param longmsg 에러 메시지(긴 버전)
 */
void clienterror(int fd, char *cause, char *errnum, char *shortmsg,
                 char *longmsg) {
  char buf[MAXLINE], body[MAXBUF];

  /* HTTP의 BODY 템플릿 생성 */
  sprintf(body, "<html><title>Tiny Error</title>");
  sprintf(body,
          "%s<body bgcolor="
          "ffffff"
          ">\r\n",
          body);
  sprintf(body, "%s%s: %s\r\n", body, errnum, shortmsg);
  sprintf(body, "%s<p>%s: %s\r\n", body, longmsg, cause);
  sprintf(body, "%s<hr><em>The Tiny Web server</em>\r\n", body);

  /* save buf the HTTP response */
  sprintf(buf, "HTTP/1.1 %s %s\r\n", errnum, shortmsg);
  Rio_writen(fd, buf, strlen(buf));
  sprintf(buf, "Content-type: text/html\r\n");
  Rio_writen(fd, buf, strlen(buf));
  sprintf(buf, "Content-length: %d\r\n\r\n", (int)strlen(body));
  Rio_writen(fd, buf, strlen(buf));
  Rio_writen(fd, body, strlen(body));
}

/**
 * @brief 요청 헤더를 읽고 출력하는 함수
 *
 * @param rp rio_t 구조체 포인터
 */
void read_requesthdrs(rio_t *rp) {
  char buf[MAXLINE];
  printf("\n");
  Rio_readlineb(rp, buf, MAXLINE);

  while (strcmp(buf, "\r\n")) {
    Rio_readlineb(rp, buf, MAXLINE);
    printf("%s", buf);
  }
}

/**
 * @brief uri를 분석해 filename과 cgi_args를 추출하는 함수
 *
 * @param uri uri
 * @param filename filename을 담아줄 포인터
 * @param cgi_args cgi 인자를 담아줄 포인터
 *
 * @return int (정적컨텐츠) ? 1 : 0
 *
 * @details
  cgi : 웹서버가 동적 컨텐츠를 제공하기 위해 사용하는 기술 (common gateway
  interface) (like 서버사이드 렌더링)
   - 웹서버에 폴더를 지정하고, 해당 폴더에 있는 파일을 CGI 프로그램에게 전달
   - CGI 프로그램은 DB와 통신해 웹서버에게 결과를 전달
   - 웹서버는 결과를 클라이언트에게 전달

   웹서버가 return하는 모든 내용은 서버과 관리하는 연관된다.
   즉, 해당 파일든은 모두 URI를 갖고있다
   다시말해서 URI를 분석해 정적인지 동적인지 알 수 있다.
   (단, 정적과 동적이 따로 관리되어야한다는 가정이 있긴하다)
 */
int parse_uri(char *uri, char *filename, char *cgi_args) {
  char *cgi_arg_p;

  /* uri에 표현되는 폴더가 cgi-bin인가? 즉, 동적컨텐츠인가 ? */
  // http://localhost:35123/cgi-bin/adder?213&23
  // uri = /cgi-bin/adder?213&23

  if (!strstr(uri, "cgi-bin")) { /* 정적 컨텐츠 */
    /* ./{content}.html 로 변환 */
    strcpy(cgi_args, "");
    strcpy(filename, ".");
    strcat(filename, uri);
    /* uri가 '/'로 끝나면 home.html(기본파일)로 변환 */
    if (uri[strlen(uri) - 1] == '/') strcat(filename, "home.html");

    return 1;
  } else {                       /* 동적 컨텐츠 */
    cgi_arg_p = index(uri, '?'); /* cgi argument 추출*/
    if (cgi_arg_p) {
      strcpy(cgi_args, cgi_arg_p + 1);
      *cgi_arg_p = '\0';
    } else {
      strcpy(cgi_args, "");
    }

    strcpy(filename, ".");
    strcat(filename, uri);
    return 0;
  }
}

/**
 * @brief 자식 프로세스를 생성후 CGI를 통해 동적 컨텐츠를 제공하는 함수
 * @details
   1. 성공(200)했다는 res(header)를 클라이언트에게 보낸다
   2. CGI 프로그램을 실행한다
 * @param fd 클라이언트와 연결된 파일 디스크립터
 * @param filename 파일 이름
 * @param cgiargs CGI 인자
*/
void serve_dynamic(int fd, char *filename, char *cgiargs) {
  char buf[MAXLINE], *emptylist[] = {NULL};

  /* 1. 성공(200)했다는 res(header)를 클라이언트에게 보낸다 */
  sprintf(buf, "HTTP/1.1 200 0K\r\n");
  Rio_writen(fd, buf, strlen(buf));
  sprintf(buf, "Server: Tiny Web Server\r\n");
  Rio_writen(fd, buf, strlen(buf));
  if (Fork() == 0) {
    /*
      QUERY_STRING은 CGI 스펙에 정의된 환경 변수 이름
      웹서버가 브라우저로 부터 받은 정보를 CGI에게 넘겨줄때
      CGI가 실행하기 위한 정보(cgiargs)를 환경변수(QUERY_STRING)으로
      넘겨준다
    */
    setenv("QUERY_STRING", cgiargs, 1);
    Dup2(fd, STDOUT_FILENO); /* 자식의 표준 출력을 연결 파일 식별자로 재지정
    // */
    Execve(filename, emptylist, environ); /* CGI prog 실행 */
  }
  /*
    Wait()는 자식 프로세스가 종료될때까지 기다리고 회수한다.
    즉, Wait를 없애면 system call로 회수하지 않았을때 zombie process가 된다.
  */
  /* before 11.8 */
  // Wait(NULL); /* 부모는 자식이 끝날때까지 기다린다. */
  /* after 11.8 */
  // remove Wait(NULL) -> handle_SIGCHLD()로 대체
}

/**
 * @brief 클라이언트에게 정적 컨텐츠를 제공하는 함수
 * @details
   1. res(header)를 클라이언트에게 보낸다
   2. res(body)를 클라이언트에게 보낸다
 * @param fd 클라이언트와 연결된 파일 디스크립터
 * @param filename 파일 이름
 * @param filesize 파일 크기
 */
void serve_static(int fd, char *filename, int filesize) {
  int srcfd;
  char *srcp, filetype[MAXLINE], buf[MAXBUF];

  /* 1. res(header)를 클라이언트에게 보낸다. */
  get_filetype(filename, filetype);
  sprintf(buf, "HTTP/1.1 200 OK\r\n");
  sprintf(buf, "%sServer: Tiny Web Server\r\n", buf);
  sprintf(buf, "%sConnection: close\r\n", buf);
  sprintf(buf, "%sContent-length: %d\r\n", buf, filesize);
  sprintf(buf, "%sContent-type: %s\r\n\r\n", buf, filetype);
  Rio_writen(fd, buf, strlen(buf)); /* buf에 저장된 헤더를 fd에 작성 */
  printf("Response headers:\n");
  printf("%s", buf);

  /* 2. res(body)를 클라이언트에게 보낸다. */
  srcfd = Open(filename, O_RDONLY, 0); /* 파일 오픈 */

  /* -------------- before : 11.9 --------------
    Mmap을 사용시에 파일의 데이터가 복사되는게 아니라 가상 메모리 주소가 파일의
    데이터를 가르키도록 설정한다.
    실제 데이터를 접근할때 메모리에 로드(레이지로딩)하기 떄문에 효율적이다.
  */
  // srcp = Mmap(0, filesize, PROT_READ, MAP_PRIVATE, srcfd, 0); /* 파일 매핑 */
  // Close(srcfd);
  // Rio_writen(fd, srcp, filesize);
  // Munmap(srcp, filesize); /* 매핑한 메모리 해제 */

  /* -------------- after : 11.9 --------------
    before와의 차이점 : Rio_readn()
    -> malloc으로 할당한 메모리에 파일을 담기위해서는 파일을 읽어야한다.
    즉, malloc에 써줄 데이터를 읽어야한다.(malloc에 넣는다.)
   */
  srcp = (char *)Malloc(filesize);  /* 파일크기만큼 할당해야한다. */
  Rio_readn(srcfd, srcp, filesize); /* 파일을 읽고 srcp에 저장 */
  Close(srcfd);
  Rio_writen(fd, srcp, filesize); /* 클라이언트에게 파일을 보낸다 */
  Free(srcp);
}

/**
 * @brief 파일의 타입을 가져오는 함수
 *
 * @param filename 파일 이름
 * @param filetype 파일 타입을 저장할 buffer
 */
void get_filetype(char *filename, char *filetype) {
  if (strstr(filename, ".html")) {
    strcpy(filetype, "text/html");
  }

  else if (strstr(filename, ".gif")) {
    strcpy(filetype, "image/gif");
  }

  else if (strstr(filename, ".mpg")) {
    strcpy(filetype, "video/mpeg");
  }

  else if (strstr(filename, ".png")) {
    strcpy(filetype, "image/png");
  }

  else if (strstr(filename, ".jpg")) {
    strcpy(filetype, "image/jpeg");
  }

  else {
    strcpy(filetype, "text/plain");
  }
}

/**
 * @brief os에 등록하는 자식 프로세스가 종료되면 호출되는 시그널 핸들러
 * (observer pattern)
 *
 * @param sig SIGCHLD signal
 * @details
  Q. waitpid는 해당 pid의 자식 프로세스가 종료될때까지 기다린다.
  그렇다는 의미는 기존의 부모를 Wait(Null)로 블록킹하는거랑 차이가 없다.
  그렇다면 왜 waitpid를 사용하는가?

  A. waitpid는 WNOHANG 옵션을 사용할 수 있다.
  이 옵션을 사용시에 자식프로세스가 종료되지 않더라도 부모프로세스를 블록하지
  않는다.
 */
void handle_SIGCHLD(int sig) {
  int saved_errno = errno;

  while (waitpid((pid_t)-1, 0, WNOHANG) > 0) {
  };

  errno = saved_errno;
}
