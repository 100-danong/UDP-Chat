#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT 8080
#define BUFFER_SIZE 1024
#define NAME_SIZE 50

int sockfd;
struct sockaddr_in servaddr;
int len;
char name[NAME_SIZE];

//송신용 쓰레드
void *send_message(void *arg){
	char buffer[BUFFER_SIZE];

	while(1){
		fgets(buffer, BUFFER_SIZE, stdin);
		buffer[strcspn(buffer, "\n")] = '\0';
		
		char message_with_name[BUFFER_SIZE];
		snprintf(message_with_name, BUFFER_SIZE, "%s: %s", name, buffer);
		sendto(sockfd, message_with_name, BUFFER_SIZE, 0, (const struct sockaddr *)&servaddr, len);


		if(strncmp(buffer, "exit", 4) == 0){
			printf("\n클라이언트가 접속을 종료합니다...\n");
			exit(0);
		}
		
	}
	return NULL;
}

int main(){
	char buffer[BUFFER_SIZE];
	pthread_t send_thread;

	//소켓 생성
	if((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ){
		perror("소켓 생성 실패");
		exit(EXIT_FAILURE);
	}

	//서버 주소 초기화 & 설정
	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(PORT);
	
	if(inet_pton(AF_INET, "10.10.50.87", &servaddr.sin_addr) <= 0){
	perror("서버 ip를 찾을 수 없습니다.");
		exit(EXIT_FAILURE);
	}

	printf("당신의 이름을 입력하세요: ");
	fgets(name, sizeof(name), stdin);
	name[strcspn(name, "\n")] = 0;
	
	if(strlen(name) >= NAME_SIZE){
	fprintf(stderr, "이름은 최대 %d자까지 입력할 수 있습니다.\n", NAME_SIZE - 1);
	close(sockfd);
	exit(EXIT_FAILURE);
	}

	len = sizeof(servaddr);

	//쓰레드 실행
	pthread_create(&send_thread, NULL, send_message, NULL);

	//수신용 반복문
	while(1){
		int n = recvfrom(sockfd, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&servaddr, &len);
		buffer[n] = '\0';
		printf("\n%s\n", buffer);

		char *ptr = strtok(buffer, ":");

		while(ptr != NULL){
			if(strncmp(ptr, "exit", 4) == 0){
				printf("\n서버가 퇴장했습니다...\n");	

				break;
			}
			ptr = strtok(NULL, " ");
		}

	}

	pthread_join(send_thread, NULL);

	close(sockfd);
	return 0;
}





