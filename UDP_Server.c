#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <unistd.h>

//상수
#define PORT 8080
#define BUFFER_SIZE 1024


//구조체와 변수
int sockfd;
struct sockaddr_in servaddr, cliaddr;
int len;

//송신용 쓰레드
void *send_message(void *arg){
	char buffer[BUFFER_SIZE];

	while(1){
		//stdin 표준 입력 스트림
		fgets(buffer, BUFFER_SIZE, stdin);
		//개행 문자 제거 \0은 끝을 나타냄
		buffer[strcspn(buffer, "\n")] = '\0';

		char message_with_name[BUFFER_SIZE];
		snprintf(message_with_name, BUFFER_SIZE, "서버: %s", buffer);

		//소켓 디스크립터, 전송할 데이터, 전송할 데이터 크기, 옵션, 목적지 주소 정보, 목적지 주소 정보의 크기
		sendto(sockfd,message_with_name, strlen(message_with_name), 0, (struct sockaddr *)&cliaddr, len);
		

		//종료
		if(strncmp(buffer, "exit", 4) == 0){
			printf("\n서버가 퇴장합니다...\n");
			exit(0);
		}
	}
	return 0;
}


int main(){
	char buffer[BUFFER_SIZE];
	//쓰레드 선언
	pthread_t send_thread;
	
	//소켓 생성
	//IPv4, UDP
	if((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) <0) {
		printf("소켓 생성 실패");
		exit(EXIT_FAILURE);

	}

	//서버 주소와 클라이언트 주소 초기화
	memset(&servaddr, 0, sizeof(servaddr));
	memset(&cliaddr, 0, sizeof(cliaddr));
	
	//서버 주소 설정
	//IPv4
	servaddr.sin_family = AF_INET;
	//모든 네트워크 인터페이스에서 들어오는 연결을 수신
	servaddr.sin_addr.s_addr = INADDR_ANY;
	//포트 설정
	servaddr.sin_port = htons(PORT);

	//서버 주소 바인딩
	if(bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr)) < 0){
		printf("바인딩 실패");
		close(sockfd);
		exit(EXIT_FAILURE);
	}

	len = sizeof(cliaddr);

	//쓰레드 실행
	pthread_create(&send_thread, NULL, send_message, NULL);

	//메세지 수신용 반복문
	while(1){

		int n = recvfrom(sockfd, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&cliaddr, &len);
		buffer[n] = '\0';
		printf("\n%s\n", buffer);
	  //printf("%s\n", inet_ntoa(cliaddr.sin_addr));

		char *ptr = strtok(buffer, ":");

		while(ptr != NULL){
		//상대방 종료
			if(strncmp(ptr, "exit", 4) == 0){
				printf("\n클라이언트가 접속 종료합니다...\n");
				break;
			}
			ptr = strtok(NULL, " ");
		}
	}

	pthread_join(send_thread, NULL);

	close(sockfd);
	return 0;
}
	






	













