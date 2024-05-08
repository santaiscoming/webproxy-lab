/*
 * adder.c - a minimal CGI program that adds two numbers together
 */
/* $begin adder */
#include "csapp.h"

int main(void) {
  char *buf, *p;
  char arg1[MAXLINE], arg2[MAXLINE], content[MAXLINE];
  int num1 = 0, num2 = 0;

  /* Extract the two arguments */
  if ((buf = getenv("QUERY_STRING")) != NULL) {
    p = strchr(buf, '&');
    *p = '\0';

    sscanf(buf, "first=%s", arg1);
    sscanf(p + 1, "second=%s", arg2);

    num1 = atoi(arg1);
    num2 = atoi(arg2);
  }

  /* Make the response body */
  sprintf(content, "QUERY_STRING=%s", buf);
  sprintf(content, "Welcome to add.com: ");
  sprintf(content, "%sTHE Internet addition portal.\r\n<p>", content);
  sprintf(content, "%sThe answer is: %d + %d = %d\r\n<p>", content, num1, num2,
          num1 + num2);
  sprintf(content, "%sThanks for visiting!\r\n", content);

  /* Generate the HTTP response */
  printf("Connection: close\r\n");
  printf("Content-length: %d\r\n", (int)strlen(content));
  printf("Content-type: text/html\r\n\r\n"); /* \r\n\r\n로 헤더와 바디를 구분 */

  /*
    -------------- after 11.11 --------------
    HEAD 메서드일 경우 바디를 출력하지 않는다.
  */
  if (!!strcasecmp(getenv("REQUEST_METHOD"), "HEAD")) {
    printf("%s", content); /* 바디 출력 */
  }

  fflush(stdout);
  exit(0);
}
/* $end adder */

/*
  atoi(str) : 문자열을 정수로 변환 ex) atoi("123") -> 123
*/
