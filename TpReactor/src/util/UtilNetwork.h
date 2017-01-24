/*
 * UtilNetwork.h
 *
 *  Created on: 2017. 1. 24.
 *      Author: cobus (blog.naver.com/cestlavie_01)
 */

#ifndef SRC_UTIL_UTILNETWORK_H_
#define SRC_UTIL_UTILNETWORK_H_

namespace cobus {

class UtilNetwork {
public:
	UtilNetwork();
	virtual ~UtilNetwork();

	/**
	 * TCP socket을 생성한다.
	 *
	 * @return
	 * 		-1: socket 생성 실패
	 * 		n: 생성한 socket
	 */
	static int createTcpSocket();

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
	static int listenTcpPort(const int socket,
	                         const int port,
	                         const int waitQsize = 1000);

	/**
	 * 전달받은 패킷을 화면에 출력한다.
	 *
	 * @param packet
	 * 		출력하려는 packet
	 * @param packetSize
	 * 		출력하려는 packet buffer의 크기
	 */
	static void printPacket(char *packet,
							int packetSize);
};

} /* namespace cobus */

#endif /* SRC_UTIL_UTILNETWORK_H_ */
