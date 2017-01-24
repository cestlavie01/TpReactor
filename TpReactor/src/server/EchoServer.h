/*
 * EchoServer.h
 *
 *  Created on: 2017. 1. 24.
 *      Author: cobus (blog.naver.com/cestlavie_01)
 */

#ifndef SRC_SERVER_ECHOSERVER_H_
#define SRC_SERVER_ECHOSERVER_H_

#include <sys/types.h>
#include <sys/time.h>
#include <netinet/in.h>

#define CFG_SIZE_MAX_RECV_BUFFER 10

namespace cobus {

class EchoServer {
public:
	EchoServer();
	virtual ~EchoServer();

	/**
	 * echo server를 시작한다.
	 */
	void run();

private:

	/**
	 * select를 수행한다.
	 *
	 * @return
	 * 		0: 정상종료
	 * 		-1: select 동작에 오류 발생
	 */
	int doSelect();

private:
	int serverSocket_;
	int maxFd_;

	fd_set reads_;
	fd_set copy_reads_;

	struct timeval timeout_;

	char buffer_[CFG_SIZE_MAX_RECV_BUFFER];

	int clientSocket_;
	struct sockaddr_in clientAddr_;
	unsigned int clientAddrSize_;
};

} /* namespace cobus */

#endif /* SRC_SERVER_ECHOSERVER_H_ */
