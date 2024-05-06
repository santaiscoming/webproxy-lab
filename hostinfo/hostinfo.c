#include "csapp.h"

int main(int argc, char *argv[]) {
  struct addrinfo *p, *listp, hints;
  char buf[MAXLINE];
  int rc, flags;
  int i = 0;

  if (argc != 2) {
    fprintf(stderr, "usage: %s <domain name>\n", argv[0]);
    exit(0);
  }

  /* Get a list of addrinfo records */
  memset(&hints, 0, sizeof(struct addrinfo));
  hints.ai_family = AF_INET;      /* IPv4 only */
  hints.ai_socktype = SOCK_DGRAM; /* Open a connection */
  /*
    socket type
    - SOCK_STREAM : 연결 지향형 소켓 (TCP)
    - SOCK_DGRAM : 비연결 지향형 소켓 (UDP)
    - SOCK_RAW : raw socket (IP)
  */
  /*
    getaddrinfo() : 호스트 이름과 서비스 이름을 이용하여 주소 정보를 얻어온다.
    - hint : 주소 정보를 얻어올 때 참조할 정보를 담고 있는 구조체의 주소
    즉, getaddrinfo로 부터 최대 3개를 가져올 수 있지만  ai_family, ai_socktype
    으로 특정 addrinfo 구조체만 가져올 수 있다.

    naver.com 의 경우 4개의 ip 주소를 가지고 있으므로 TCP로 소켓타입을 한정시에
    4개의 addrinfo 구조체를 반환한다.
    그리고 그 addrinfo 구조체에서 tcp로 소켓타입을 한정시에 한정된 소켓타입과
    대응되는 addrinfo 구조체만 연결되어있는것

    그렇다는것은 socket_type을 Null로 설정시에 12개의 addrinfo 구조체를
    반환한다.

  */

  /*
    hint에 해당하는 addrinfo 구조체를 반환한다.
      - hint의 sock_type이 NULL이면 모든 addrinfo 구조체를 반환한다.
  */
  if ((rc = getaddrinfo(argv[1], NULL, &hints, &listp)) != 0) {
    fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(rc));
    exit(1);
  }

  /* Walk the list and display each IP address */
  flags = NI_NUMERICHOST;
  for (p = listp; p; p = p->ai_next) {
    Getnameinfo(p->ai_addr, p->ai_addrlen, buf, MAXLINE, NULL, 0, flags);
    printf("%d\n", i++);
    printf("%s\n", buf);
  }

  /* Clean up */
  Freeaddrinfo(listp);

  exit(0);
}