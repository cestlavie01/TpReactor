/*
 * UtilNetwork.cpp
 *
 *  Created on: 2017. 1. 24.
 *      Author: cobus (blog.naver.com/cestlavie_01)
 */

#include "UtilNetwork.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

namespace cobus {

UtilNetwork::UtilNetwork() {
}

UtilNetwork::~UtilNetwork() {
}

/**
 * TCP socket을 생성한다.
 *
 * @return
 * 		-1: socket 생성 실패
 * 		n: 생성한 socket
 */
int
UtilNetwork::createTcpSocket() {
	return socket(AF_INET, SOCK_STREAM, 0);
}

/**
 * 전달받은 socket으로 listen한다.
 *
 * @param socket
 * 		listen하려는 socket
 * @param port
 * 		listen 하려는 port
 * @param waitQsize
 * 		listen 대기 queue size
 * @return
 * 		0: listen 성공
 * 		-1: bind 실패
 * 		-2: listen 실패
 */
int
UtilNetwork::listenTcpPort(const int socket,
                           const int port,
						   const int waitQsize/* = 1000*/) {
	struct sockaddr_in serverAddr;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serverAddr.sin_port = htons(port);

	int r = 0;
	r = bind(socket, (struct sockaddr *) &serverAddr, sizeof(serverAddr));
	if (r < 0) {
		return -1;
	}

	r = listen(socket, waitQsize);
	if (r < 0) {
		return -2;
	}

	return 0;
}

/**
 * 전달받은 패킷을 화면에 출력한다.
 *
 * @param packet
 * 		출력하려는 packet
 * @param packetSize
 * 		출력하려는 packet buffer의 크기
 */
void UtilNetwork::printPacket(char *packet,
							  int packetSize) {
	for (int i = 0; i < packetSize; i++) {
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

} /* namespace cobus */
