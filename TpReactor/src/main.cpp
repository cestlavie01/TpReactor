/*
 * main.cpp
 *
 *  Created on: 2017. 1. 12.
 *      Author: cobus (blog.naver.com/cestlavie_01)
 */

#include "main.h"

#include <stdio.h>

#include "server/EchoServer.h"

int
main() {

	cobus::EchoServer server;
	server.run();

	return 0;
}
