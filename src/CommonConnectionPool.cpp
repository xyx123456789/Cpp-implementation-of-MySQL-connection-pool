#define  _CRT_SECURE_NO_WARNINGS
#include"CommonConnectionPool.h"
#include"public.h"


//�������������ӿ�
ConnectionPool* ConnectionPool::GetConnectionPool()
{
	static ConnectionPool pool;
	return &pool;
}

//���������ļ�
bool ConnectionPool::loadConfigFile()
{
	FILE* pf = fopen("my.ini", "r");
	if (pf == nullptr)
	{
		LOG("my.ini is not exist!");
		return false;
	}
	
	bool success = true; // ���ڱ���Ƿ���سɹ�
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
            success = false; // �����Ч�ļ�
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

//���ӳصĹ���
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
        p->refreshAliveTime();//ˢ��һ�¿�ʼ���е���ʼʱ��
        _connectionQue.push(p);
        _connectionCnt++;
    }

    //����һ���µ��̣߳���Ϊ���ӵ�������
    thread produce(&ConnectionPool::produceConnectionTask,this);
    produce.detach();

    //����һ���µ��̣߳�ɨ�賬��maxIdleTime�����ӣ����ж���Ļ���
    thread scanner(&ConnectionPool::scannerConnectionTask, this);
    scanner.detach();
}

//�����ڶ������߳��У�ר�Ÿ�������������
void ConnectionPool::produceConnectionTask()
{
    for (;;)
    {
        unique_lock<mutex> lock(_queueMtx);
        while (!_connectionQue.empty())
        {
            cv.wait(lock);//���в��գ��������߳̽���ȴ�״̬
        }

        //������ӳ��е�������С��������������򴴽��µ�����
        if (_connectionCnt < _maxSize)
        {
            Connection* p = new Connection();
            if (!p->connect(_ip, _port, _user, _pwd, _dbname))
            {
                LOG("create Conn connection to mysql failed");
                delete p;
                continue;
            }
            p->refreshAliveTime();//ˢ��һ�¿�ʼ���е���ʼʱ��
            _connectionQue.push(p);
            _connectionCnt++;
        }

        cv.notify_all();
    }
}

//���ⲿ�ṩ�ӿڣ������ӳ��л�ȡһ����������
//����ָ���Զ������ͷ���Դ
shared_ptr<Connection> ConnectionPool::getConnection()
{
    unique_lock<mutex> lock(_queueMtx);
 
        if (!cv.wait_for(lock, std::chrono::milliseconds(_connectionTimeout),
            [this] { return !_connectionQue.empty(); })) {
            LOG("get connection timeout");
            return nullptr;
        }

        /*shared_ptr����ָ������ʱ�����connection��Դֱ��delete�����൱��
        ����connection������������connection�ͱ�close��
        ������Ҫ�ض���shared_ptr���ͷ���Դ��ʽ����connectionֱ�ӹ黹��_connectionQue��*/

        shared_ptr<Connection> sp(_connectionQue.front(), 
            [&](Connection* pcon) {
            //�������ڷ������߳��е��õģ�һ��Ҫע���̰߳�ȫ

            unique_lock<mutex> lock(_queueMtx);
            _connectionQue.push(pcon);
            pcon->refreshAliveTime();//ˢ��һ�¿�ʼ���е���ʼʱ��
            });
;
        _connectionQue.pop();
        if (_connectionQue.empty())  cv.notify_all();
        return sp;
}

//ɨ�賬��maxIdleTime�����ӣ����ж���Ļ���
void ConnectionPool::scannerConnectionTask()
{
    for (;;)
    {
        //ͨ��sleepģ�ⶨʱЧ��
        this_thread::sleep_for(chrono::seconds(_maxIdleTime));

        //ɨ���������У��ͷŶ�������
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
                break; //�����ͷ��û��������ʱ�䣬��˵�����������Ҳû�е�������ʱ�䣬ֱ���˳�ѭ��
            }
        }
    }
}