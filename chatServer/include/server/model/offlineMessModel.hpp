#ifndef OFFLINEMESSMODEL_H
#define OFFLINEMESSMODEL_H

#include <string>
#include <vector>
#include "json.hpp"

using json = nlohmann::json;

using namespace std;

class OfflineMessModel
{
public:
    // 向offlinemessage数据库插入离线待发送数据
    bool insert(int id, string message, int senderid, string type);

    // 从offlinemessage数据库中删除离线待发送数据
    bool remove(int id);

    // 从offlinemessage数据库中查询离线待发送数据
    vector<string> query(int id);
};

#endif