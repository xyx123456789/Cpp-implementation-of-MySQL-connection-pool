#pragma once
//实现连接池功能模块
#include<string>
#include<queue>
#include<mutex>
#include"Connection.h"
#include<atomic>
#include<thread>
#include<memory>
#include<functional> 
#include<condition_variable>
using namespace std;

class ConnectionPool
{
public:
	//获取连接池对象
	static ConnectionPool* GetConnectionPool();

	//给外部提供接口，从连接池中获取一个可用连接
	//智能指针自动析构释放资源
	shared_ptr<Connection> getConnection();

private:
	ConnectionPool();//单例

	bool loadConfigFile();//加载配置文件

	//运行在独立的线程中，专门负责生产新连接
	void produceConnectionTask();

	//扫描超过maxIdleTime的连接，进行多余的回收
	void scannerConnectionTask();

	string _ip;
	unsigned short _port;
	string _user;
	string _pwd;
	string _dbname;

	int _initSize;//连接池的初始连接数
	int _maxSize;//连接池的最大连接数
	int _maxIdleTime;//连接池的最大空闲时间
	int _connectionTimeout;//连接池的获取连接超时时间

	queue<Connection*> _connectionQue;//存储mysql连接的队列
	mutex _queueMtx;//保护连接队列的互斥锁
	atomic_int _connectionCnt;//连接池中当前连接的数量
	condition_variable cv;//条件变量，用于通知生产者线程和消费者线程
};