/*
 * EchoServer.cpp
 *
 *  Created on: 2017. 1. 24.
 *      Author: cobus (blog.naver.com/cestlavie_01)
 */

#include "EchoServer.h"

#include <stdio.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>

#include "../server/EchoServer.h"
#include "../util/UtilNetwork.h"

using namespace cobus;

#define CFG_LISTEN_PORT 15000
#define CFG_SIZE_MAX_FD 1000

namespace cobus {

EchoServer::EchoServer() {
	serverSocket_ = -1;
	maxFd_ = 0;

	clientSocket_ = -1;
	clientAddrSize_ = sizeof(clientAddr_);
}

EchoServer::~EchoServer() {
	if (serverSocket_ != -1) {
		close(serverSocket_);
	}
}

/**
 * echo server를 시작한다.
 */
void
EchoServer::run() {
	printf("terminate program.\n");
	serverSocket_ = UtilNetwork::createTcpSocket();
	if (serverSocket_ < 0) {
		printf("can not create socket\n");
		return;
	}

	int r = UtilNetwork::listenTcpPort(serverSocket_,
									   CFG_LISTEN_PORT,
									   CFG_SIZE_MAX_FD);
	if (r == 0) {
		printf("listen %d port...\n", CFG_LISTEN_PORT);
	} else {
		if (r == -1) {
			printf("bind error.\n");
		} else if (r == -2) {
			printf("listen error.\n");
		}
		return;
	}

	/* fd_set에 서버 소켓 설정 */
	FD_ZERO(&reads_);
	FD_SET(serverSocket_, &reads_);
	maxFd_ = serverSocket_;

	/* select timeout 1초 설정 */
	timeout_.tv_sec = 1;
	timeout_.tv_usec = 0;

	while(true) {

		if (doSelect() < 0) {
			break;
		}
	}
}

/**
 * select를 수행한다.
 *
 * @return
 * 		0: 정상종료
 * 		-1: select 동작에 오류 발생
 */
int
EchoServer::doSelect() {
	/* timeout 시간 재설정 */
	timeout_.tv_sec = 1;
	timeout_.tv_usec = 0;

	/* fd_set 복사 */
	copy_reads_ = reads_;

	/* select */
	int r = select(maxFd_ + 1, &copy_reads_, NULL, NULL, &timeout_);

	if (r == 0) {
		// printf("time out...\n");
		return 0;
	}

	/* select 결과 예외처리 */
	if (r < 0) {
		printf("select error\n");
		return -1;
	}

	/* server listen socket에 input event가 있는지 확인 */
	if (FD_ISSET(serverSocket_, &copy_reads_)) {
		/* input event가 있으면 accept */
		clientSocket_ = accept(serverSocket_,
							   (struct sockaddr *) &clientAddr_,
							   &clientAddrSize_);

		if (clientSocket_ < 0) {
			printf("invalid client socket. [clientSocket:%d]\n", clientSocket_);
			return 0;
		}

		printf("accept new client. [clientSocket:%d]\n", clientSocket_);

		do {
			/* client socket에서 데이터 수신 */
			r = recv(clientSocket_, buffer_, sizeof(buffer_), 0);
			if (r < 0) {
				printf("could not recv. client socket error.\n");
				break;
			}

			if (r == 0) {
				printf("receive client socket closed.\n");
				break;
			}

			printf("receive from [clientSocket:%d][read:%d]\n", clientSocket_,
					r);
			UtilNetwork::printPacket(buffer_, r);

			/* 수신한 데이터를 다시 client로 전송 */
			r = send(clientSocket_, buffer_, r, 0);
			if (r < 0) {
				printf("could not send. socket error\n");
				break;
			}

			printf("send to [clientSocket:%d][write:%d]\n", clientSocket_, r);
			UtilNetwork::printPacket(buffer_, r);

		} while (false);

		/* client socket 종료 */
		close (clientSocket_);
		printf("close [clientSocket:%d]\n", clientSocket_);
	}

	return 0;
}

} /* namespace cobus */
