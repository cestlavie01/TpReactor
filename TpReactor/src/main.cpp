/*
 * main.cpp
 *
 *  Created on: 2017. 1. 12.
 *      Author: cobus
 */

#include <stdio.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <unistd.h>

#define CFG_LISTEN_PORT 15000
#define CFG_SIZE_MAX_FD 1000
#define CFG_SIZE_MAX_RECV_BUFFER 10

void
printPacket(unsigned char *packet,
            unsigned int packetSize);

int
main() {

	/* create socket */
	int serverSocket = -1;
	serverSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (serverSocket < 0) {
		printf("can not create socket\n");
		return -1;
	}

	/* bind socket */
	struct sockaddr_in serverAddr;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serverAddr.sin_port = htons(CFG_LISTEN_PORT);

	int result = 0;
	result = bind(serverSocket,
			(struct sockaddr *) &serverAddr,
			sizeof(serverAddr));
	if (result < 0) {
		printf("bind error\n");
		return -1;
	}

	/* listen */
	result = listen(serverSocket, CFG_SIZE_MAX_FD);
	if (result < 0) {
		printf("listen error\n");
		return -1;
	}

	printf("listen %d port...\n", CFG_LISTEN_PORT);

	/* fd_set에 서버 소켓 설정 */
	fd_set reads;
	fd_set copy_reads;
	FD_ZERO(&reads);
	FD_SET(serverSocket, &reads);
	int maxFd = serverSocket;

	/* select timeout 1초 설정 */
	struct timeval timeout;
	timeout.tv_sec = 1;
	timeout.tv_usec = 0;

	int clientSocket = -1;
	struct sockaddr_in clientAddr;
	unsigned int clientAddrSize = sizeof(clientAddr);

	const int bufferSize = CFG_SIZE_MAX_RECV_BUFFER;
	char buffer[bufferSize];

	while(true) {

		/* timeout 시간 재설정 */
		timeout.tv_sec = 1;
		timeout.tv_usec = 0;

		/* fd_set 복사 */
		copy_reads = reads;

		/* select */
		result = select(maxFd + 1, &copy_reads, NULL, NULL, &timeout);

		/* select 결과 예외처리 */
		if (result < 0) {
			printf("select error\n");
			break;
		}

		if (result == 0) {
			// printf("time out...\n");
			continue;
		}

		/* server listen socket에 input event가 있는지 확인 */
		if (FD_ISSET(serverSocket, &copy_reads)) {
			/* input event가 있으면 accept */
			clientSocket = accept(serverSocket,
					(struct sockaddr *) &clientAddr,
					&clientAddrSize);

			if (clientSocket < 0) {
				printf("invalid client socket. [clientSocket:%d]\n",
						clientSocket);
				continue;
			}

			printf("accept new client. [clientSocket:%d]\n",
					clientSocket);

			do {
				/* client socket에서 데이터 수신 */
				result = recv(clientSocket, buffer, bufferSize, 0);
				if (result < 0) {
					printf("could not recv. client socket error.\n");
					break;
				}

				if (result == 0) {
					printf("receive client socket closed.\n");
					break;
				}

				printf("receive from [clientSocket:%d][read:%d]\n",
						clientSocket, result);
				printPacket((unsigned char *) buffer, result);

				/* 수신한 데이터를 다시 client로 전송 */
				result = send(clientSocket, buffer, result, 0);
				if (result < 0) {
					printf("could not send. socket error\n");
					break;
				}

				printf("send to [clientSocket:%d][write:%d]\n",
						clientSocket, result);
				printPacket((unsigned char *) buffer, result);

			} while (false);

			/* client socket 종료 */
			close(clientSocket);
			printf("close [clientSocket:%d]\n", clientSocket);
		}
	}

	/* 무한 루프가 종료되면 server socket close */
	if (serverSocket != -1) {
		close(serverSocket);
	}

	return 0;
}

void
printPacket(unsigned char *packet,
            unsigned int packetSize) {

	for (unsigned int i = 0; i < packetSize; i++) {
		if ((i % 0x10) == 0x00) {
			fprintf(stdout, "0x%04X: ", i);
			fprintf(stdout, "%02X ", packet[i]);
			continue;
		}

		fprintf(stdout, "%02X ", packet[i]);

		if (i == 0) {
			continue;
		}

		if ((i % 0x10) == 0x0F) {
			fprintf(stdout, "| ");
			for (unsigned int j = i - 0x0F; j < i; j++) {
				if (packet[j] >= 0x20 && packet[j] < 0x7F) {
					fprintf(stdout, "%c", packet[j]);
				} else {
					fprintf(stdout, ".");
				}

				if ((j % 0x08) == 0x07) {
					fprintf(stdout, " ");
				}
			}
			fprintf(stdout, "\n");
			continue;
		}

		if ((i % 0x08) == 0x07) {
			fprintf(stdout, "  ");
			continue;
		}
	}

	int remain = packetSize % 0x10;
	if (remain != 0) {
		unsigned int space = (0x10 - remain) * 3;
		if (remain < 8) {
			space += 2;
		}
		for (; space > 0; space--) {
			fprintf(stdout, " ");
		}

		fprintf(stdout, "| ");
		for (int i = remain; i > 0; i--) {
			int j = packetSize - i;
			if (packet[j] >= 0x20 && packet[j] < 0x7F) {
				fprintf(stdout, "%c", packet[j]);
			} else {
				fprintf(stdout, ".");
			}

			if ((j % 0x08) == 0x07) {
				fprintf(stdout, " ");
			}
		}

		fprintf(stdout, "\n");
	}
	fprintf(stdout, "\n");
}
