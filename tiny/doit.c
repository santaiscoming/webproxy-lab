#include "tiny.h"

/**
 * @brief 클라이언트의 요청을 읽고 파싱하여 동적, 정적컨텐츠를 제공
 *
 * @param fd : 클라이언트와 연결된 파일 디스크립터
 */
void doit(int fd) {
  /*
    - is_static : 정적 컨텐츠인지 동적 컨텐츠인지 확인하는 변수
    - sbuf : 파일의 상태정보를 저장하는 구조체
  */

  int is_static;
  struct stat sbuf;
  char buf[MAXLINE], method[MAXLINE], uri[MAXLINE], version[MAXLINE];
  char filename[MAXLINE], cgiargs[MAXLINE];
  rio_t rio;

  /* 1) client의 request를 읽음 */
  Rio_readinitb(&rio, fd); /* init buffer & connect fd(already opend) */
  if (!Rio_readlineb(&rio, buf, MAXLINE))  // line:netp:doit:readrequest
    return;
  printf("Request headers:\n");
  printf("%s", buf);
  sscanf(buf, "%s %s %s", method, uri, version);

  /* GET 메서드가 아니면 return (strcasecmp 는 두 문자열이 같은면 return 0)*/
  if (strcasecmp(method, "GET")) {
    clienterror(fd, method, "501", "Not Implemented",
                "Tiny does not implement this method");
    return;
  }

  read_requesthdrs(&rio);

  /* 2) 데이터를 parsing */
  is_static = parse_uri(uri, filename, cgiargs);

  /* 2-1) 파일 존재 확인 */
  if (stat(filename, &sbuf) < 0) { /* 파일의 상태정보를 sbuf에 저장한다. */
    clienterror(fd, filename, "404", "Not found",
                "Tiny couldn't find this file");
    return;
  }

  /* 3) 컨텐츠 제공 */
  if (is_static) { /* 정적 컨텐츠 */
    if (!(S_ISREG(sbuf.st_mode)) || !(S_IRUSR & sbuf.st_mode)) {
      clienterror(fd, filename, "403", "Forbidden",
                  "Tiny couldn't read the file");
      return;
    }

    serve_static(fd, filename, sbuf.st_size);
  } else { /* 동적 컨텐츠 */
    if (!(S_ISREG(sbuf.st_mode)) ||
        !(S_IXUSR & sbuf.st_mode)) { /* 실행 가능한 파일인지 확인 */
      clienterror(fd, filename, "403", "Forbidden",
                  "Tiny couldn't run the CGI program");
      return;
    }

    serve_dynamic(fd, filename, cgiargs);
  }
}
