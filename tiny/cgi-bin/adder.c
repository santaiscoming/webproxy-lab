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
    p = strchr(buf, '&'); /* "&" 문자 위치의 pointer */
    *p = '\0';
    strcpy(arg1, buf); /* "&" 위치를 null 종단문자로 만들었기에 null 종단
                        이전까지만 유효*/
    strcpy(arg2, p + 1); /* "&(\n)" 한칸 이후의 string ex) 123\0456*/
                         /*                                  ⌃ */
    /* null 종단문자가 들어가서 문자열 처리 함수에서는 null종단까지 유효하지만
     * 실제로 데이터는 남아있다.*/
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
  printf("Content-type: text/html\r\n\r\n");
  printf("%s", content);
  fflush(stdout);
  exit(0);
}
/* $end adder */

/*
  atoi(str) : 문자열을 정수로 변환 ex) atoi("123") -> 123
*/
