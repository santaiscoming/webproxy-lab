# CC : 컴파일러를 지정
# CFLAGS : 컴파일 옵션을 지정
## -O2 : 최적화 옵션,
## -Wall : 모든 경고를 출력
## -I . : 현재 디렉토리를 포함한 디렉토리를 지정
# LIB : 링크할 라이브러리를 지정
## -lpthread : pthread 라이브러리를 링크 

CC = gcc
CFLAGS = -O2 -Wall -I .
LIB = -lpthread

# all : 명령어를 실행하면 echoClient, echoService 실행 파일을 생성

# echoService : echoService.c, csapp.o, echo.o 파일을 링크하여 실행 파일을 생성
## $(CC) : 컴파일러를 지정
## $(CFLAGS) : 컴파일 옵션을 지정
## ${variable} : 위의 변수를 참조해 링킹

# ex) $(CC) $(CFLAGS) -o echoService echoService.c csapp.o echo.o $(LIB)
## CC 변수 컴파일러와 CFLAGS 컴파일 옵션을 사용하여 
## echoService.c, csapp.o, echo.o 파일을 링크하여 echoService 실행 파일을 생성 하겠다는 의미

all: echoClient echoService

csapp.o: csapp.c csapp.h
	$(CC) $(CFLAGS) -c csapp.c

echo: echo.c csapp.o 
	$(CC) $(CFLAGS) -o echo echo.c csapp.o $(LIB)

echoClient: echoClient.c csapp.o
	$(CC) $(CFLAGS) -o echoClient echoClient.c csapp.o $(LIB)

echoService: echoService.c csapp.o echo.o
	$(CC) $(CFLAGS) -o echoService echoService.c csapp.o echo.o $(LIB)


        
clean:
	rm -f *.o echoClient echoService *~