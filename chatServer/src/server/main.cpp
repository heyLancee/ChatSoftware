#include <iostream>
#include "chatServer.hpp"
#include "chatService.hpp"
#include "signal.h"

using namespace std;

void resetHandler(int sig) {
  if (sig == SIGINT) {
    ChatService::getInstance().reset();
    exit(0);
  }
}

int main(int argc, char** argv) {
  if (argc != 3)
  {
	cout << "default format:./exe ip:port" << endl;
	return -1;	
  }

  signal(SIGINT, resetHandler);

  muduo::net::EventLoop loop;
  InetAddress addr(argv[1], atoi(argv[2]));

  ChatServer server(&loop, addr, "ChatServer");
  server._start();

  cout << "loop start" << endl;
  loop.loop();

  cout << "loop" << endl;
}
