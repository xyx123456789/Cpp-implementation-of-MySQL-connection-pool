#define  _CRT_SECURE_NO_WARNINGS
#include"CommonConnectionPool.h"
#include"public.h"


//懒汉单例函数接口
ConnectionPool* ConnectionPool::GetConnectionPool()
{
	static ConnectionPool pool;
	return &pool;
}

//加载配置文件
bool ConnectionPool::loadConfigFile()
{
	FILE* pf = fopen("my.ini", "r");
	if (pf == nullptr)
	{
		LOG("my.ini is not exist!");
		return false;
	}
	
	bool success = true; // 用于标记是否加载成功
	while (!feof(pf))
	{
		char line[1024] = { 0 };
        fgets(line, 1024, pf);
		string str(line);
		int idx = str.find('=', 0);
		if (idx == -1) continue;
		int edx = str.find('\n', idx);

		string key = str.substr(0, idx);
		string val = str.substr(idx + 1, edx - idx - 1);

        if (key == "ip")
        {
            _ip = val;
        }
        else if (key == "port")
        {
            _port = atoi(val.c_str());
        }
        else if (key == "user")
        {
            _user = val;
        }
        else if (key == "password")
        {
            _pwd = val;
        }
        else if (key == "initSize")
        {
            _initSize = atoi(val.c_str());
        }
        else if (key == "maxSize")
        {
            _maxSize = atoi(val.c_str());
        }
        else if (key == "maxIdleTime")
        {
            _maxIdleTime = atoi(val.c_str());
        }
        else if (key == "connectionTimeout")
        {
            _connectionTimeout = atoi(val.c_str());
        }
        else if (key == "dbname")
        {
            _dbname = val;
        }
        else
        {
            LOG("invalid key: " << key);
            success = false; // 标记无效的键
        }
        LOG(key << "=" << val);
	}

    fclose(pf);

    if(!success)
        LOG("load config failed");
    else
        LOG("load config success");
    return success;
}

//连接池的构造
ConnectionPool::ConnectionPool()
{
    if (!loadConfigFile())
    {
        LOG("load config failed, pool process exit");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < _initSize; i++)
    {
        Connection* p = new Connection();
        p->connect(_ip,_port,_user,_pwd,_dbname);
        p->refreshAliveTime();//刷新一下开始空闲的起始时间
        _connectionQue.push(p);
        _connectionCnt++;
    }

    //启动一个新的线程，作为连接的生产者
    thread produce(&ConnectionPool::produceConnectionTask,this);
    produce.detach();

    //启动一个新的线程，扫描超过maxIdleTime的连接，进行多余的回收
    thread scanner(&ConnectionPool::scannerConnectionTask, this);
    scanner.detach();
}

//运行在独立的线程中，专门负责生产新连接
void ConnectionPool::produceConnectionTask()
{
    for (;;)
    {
        unique_lock<mutex> lock(_queueMtx);
        while (!_connectionQue.empty())
        {
            cv.wait(lock);//队列不空，生产者线程进入等待状态
        }

        //如果连接池中的连接数小于最大连接数，则创建新的连接
        if (_connectionCnt < _maxSize)
        {
            Connection* p = new Connection();
            if (!p->connect(_ip, _port, _user, _pwd, _dbname))
            {
                LOG("create Conn connection to mysql failed");
                delete p;
                continue;
            }
            p->refreshAliveTime();//刷新一下开始空闲的起始时间
            _connectionQue.push(p);
            _connectionCnt++;
        }

        cv.notify_all();
    }
}

//给外部提供接口，从连接池中获取一个可用连接
//智能指针自动析构释放资源
shared_ptr<Connection> ConnectionPool::getConnection()
{
    unique_lock<mutex> lock(_queueMtx);
 
        if (!cv.wait_for(lock, std::chrono::milliseconds(_connectionTimeout),
            [this] { return !_connectionQue.empty(); })) {
            LOG("get connection timeout");
            return nullptr;
        }

        /*shared_ptr智能指针析构时，会把connection资源直接delete掉，相当于
        调用connection的析构函数，connection就被close了
        这里需要重定义shared_ptr的释放资源方式，把connection直接归还到_connectionQue中*/

        shared_ptr<Connection> sp(_connectionQue.front(), 
            [&](Connection* pcon) {
            //这里是在服务器线程中调用的，一定要注意线程安全

            unique_lock<mutex> lock(_queueMtx);
            _connectionQue.push(pcon);
            pcon->refreshAliveTime();//刷新一下开始空闲的起始时间
            });
;
        _connectionQue.pop();
        if (_connectionQue.empty())  cv.notify_all();
        return sp;
}

//扫描超过maxIdleTime的连接，进行多余的回收
void ConnectionPool::scannerConnectionTask()
{
    for (;;)
    {
        //通过sleep模拟定时效果
        this_thread::sleep_for(chrono::seconds(_maxIdleTime));

        //扫描整个队列，释放多余连接
        unique_lock<mutex> lock(_queueMtx);
        while (!_connectionQue.empty())
        {
            Connection* p = _connectionQue.front();
            if (p->getAliveTime() >= _maxIdleTime)
            {
                _connectionQue.pop();
                _connectionCnt--;
                delete p;
            }
            else
            {
                break; //如果队头还没到最大空闲时间，则说明后面的连接也没有到最大空闲时间，直接退出循环
            }
        }
    }
}