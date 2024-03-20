#include <QApplication>
#include "common.h"
#include "net/chatClient.hpp"
#include "pthread.h"
#include "thread"
#include "widget.h"

void func(int argc, char* argv[]) {
  QApplication a(argc, argv);

  Widget w;
  if (w.getLoginStatus() == false) return;
  w.show();

  a.exec();
  return;
}

// 网络通信全局变量
EventLoop loop;
InetAddress addr(SERVER_ADDR, SERVER_PORT);
ChatClient client(&loop, addr, "ChatClient");

int main(int argc, char* argv[]) {
  client.connect();

  std::thread loopThread(func, argc, argv);
  loopThread.detach();

  loop.loop();

  return 0;
}
