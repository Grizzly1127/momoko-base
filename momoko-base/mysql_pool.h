#ifndef SERVICE_STATISTIC_ANALYSIS_SYSTEM_MAIN_UI_SYSTEM_COMMON_MYSQL_POOL_H
#define SERVICE_STATISTIC_ANALYSIS_SYSTEM_MAIN_UI_SYSTEM_COMMON_MYSQL_POOL_H

#include <memory>
#include "common/database/mysql/mysql_db.h"
#include "noncopyable.h"
#include "blocking_queue.h"

using namespace std;
namespace common {

class MysqlPool : noncopyable {
    string host_;
    uint32_t port_;
    string user_;
    string pwd_;
    string db_;
    BlockingQueue<shared_ptr<MysqlDB>> mysql_queue_;

public:
    MysqlPool(const string& host, uint32_t port, const string& user, const string& pwd, const string& db, uint32_t max_size = 4)
        : host_{host}
        , port_{port}
        , user_{user}
        , pwd_{pwd}
        , db_{db}
    {
        addConn(max_size);
    }

    ~MysqlPool() {
        destoryPool();
    }

    shared_ptr<MysqlDB> getConnection() {
        shared_ptr<MysqlDB> conn;
        mysql_queue_.pop(conn);
        return conn;
    }

    void returnConnection(shared_ptr<MysqlDB>& conn) {
        mysql_queue_.push(std::move(conn));
    }

private:
    void addConn(uint32_t size) {
        for (uint32_t i = 0; i < size; ++i) {
            shared_ptr<MysqlDB> conn = make_shared<MysqlDB>();
            if (!conn->Init(host_, user_, pwd_, db_, port_, "UTF8") || !conn->Connect()) {
                continue;
            }
            mysql_queue_.push(std::move(conn));
        }
    }

    void destoryPool() {
        while (mysql_queue_.size() > 0) {
            shared_ptr<MysqlDB> conn = getConnection();
            conn->Close();
        }
    }
};

} // namespace common

#endif // SERVICE_STATISTIC_ANALYSIS_SYSTEM_MAIN_UI_SYSTEM_COMMON_MYSQL_POOL_H
