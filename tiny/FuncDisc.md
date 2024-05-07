# tiny.c

  - ref.1
  Accept() : 클라이언트로의 연결 요청을 기다린다
  @ listenfd: 서버 소켓 디스크립터
  @ struct sockaddr *addr : 클라이언트의 소켓주소
  @ socklen_t *addrlen : 클라이언트 소켓주소의 크기

  @ return: 클라이언트와 통신하기 위해 사용하는 **연결 식별자**

  Rio_readinitb() : 빈 버퍼를 설정하고, 해당 버퍼와 오픈한 fd를 연결한다.
  @ rio : rio_t 구조체
    - rio_fd : 내부 버퍼의 디스크립터
    - rio_cnt : 내부 버퍼의 읽지 않은 바이트 수
    - rio_bufptr : 내부 버퍼의 다음 읽을 바이트
    - rio_buf[RIO_BUFSIZE] : 내부 버퍼
  @ fd : 연결 식별자
  ps. 오픈한 식별자마다 한번 호출되고 식별자 fd를 주소 rp에 위치한 rio_t
  타입의 읽기 버퍼와 연결

  Rio_readlineb() : 버퍼가 비워질때마다 버퍼를 다시 채우기위해 read()를
  호출한다.
  @ rio : rio_t 구조체
  @ buf : 읽은 데이터를 저장할 버퍼
  @ maxlen : 버퍼의 최대 크기

  ps. -> read()를 여러번 호출하는것보다 효율적(read는 커널과의 통신이므로
  비용이 크다, 심지어 각 파일 바이트마다 호출)

  strcasecmp(str1, str2) : 두 문자열이 같은지 비교
  @ return : (str1 == str2) ? 0 : 1

  stat(char *path, struct stat *statbuf) : 파일의 상태정보를 가져옴
  @ path : 파일 경로
  @ statbuf : 파일의 상태정보를 저장할 구조체의 주소

  spirnt(buf, format, ...) : format에 따라 문자열을 생성후 buf에 저장
  -> 즉, buf에 출력할 문자열을 누적하며 저장가능하다.
  printf()는 단순히 console에 출력한다

  strstr(target, compare) : target에 compare가 포함되어 있는지 확인
  strcpy(target, compare) : target에 compare를 복사 (덮어씀)
  strcat(target, compare) : target에 compare를 덧붙임
  strchr(target, compare) : target에 compare가 포함되어 있는 위치의 pointer를
  반환 index(target, compare) : (target include compare) ? location : NULL

  Mmap(void *start, size_t length, int prot, int flags, int fd, off_t offset)
  : 파일을 메모리에 매핑 -> 메모리에 직접 매핑하므로 접근하고 로드하는데 system
  call이 적어짐
  @ addr : 메모리 주소
  @ length : 매핑할 파일의 바이트 크기
  @ prot : 메모리 보호 옵션
  @ flags : 프로세스간 매핑 옵션 (MAP_PRIVATE, MAP_SHARED)
  @ fd : 매핑할 파일의 디스크립터
  @ offset : 파일에서 매핑을 시작할 위치

  return : success ? 메모리 주소 : -1

  Munmap(void *start, size_t length) : 메모리 매핑 해제 ("un"map)

  Open(char *filename, int flags, mode_t mode) : 파일을 열고 디스크립터를 반환

  ref 3) waitpid(pid_t pid, int *status, int options) : 자식프로세스(pid)가
  종료될때 기다렸다 수거
  @ pid : 자식 프로세스의 pid
  @ status : 자식 프로세스의 종료 상태를 담을 포인터
  @ options : waitpid() 의 동작제어 flag
    - WNOHANG : 자식 프로세스가 종료되지 않아도 부모를 블록하지 않는다
    - WUNTRACED : 정지된 자식프로세스의 상태 변경도 status에 저장한다
    - WCONTINUED : 자식 프로세스가 SIGCONT로 재개된 상태를 반환한다


# doit.c

  stat(char *path, struct stat *statbuf) : 파일의 상태정보를 가져옴
  @ path : 파일 경로
  @ statbuf : 파일의 상태정보를 저장할 구조체의 주소