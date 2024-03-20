#include "netpublic.hpp"

NetSignal::NetSignal() {}

NetSignal::~NetSignal() {}

NetSignal& NetSignal::getInstance() {
  static NetSignal instance;
  return instance;
}
