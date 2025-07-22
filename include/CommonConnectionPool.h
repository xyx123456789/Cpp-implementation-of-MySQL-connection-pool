#pragma once
//ʵ�����ӳع���ģ��
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
	//��ȡ���ӳض���
	static ConnectionPool* GetConnectionPool();

	//���ⲿ�ṩ�ӿڣ������ӳ��л�ȡһ����������
	//����ָ���Զ������ͷ���Դ
	shared_ptr<Connection> getConnection();

private:
	ConnectionPool();//����

	bool loadConfigFile();//���������ļ�

	//�����ڶ������߳��У�ר�Ÿ�������������
	void produceConnectionTask();

	//ɨ�賬��maxIdleTime�����ӣ����ж���Ļ���
	void scannerConnectionTask();

	string _ip;
	unsigned short _port;
	string _user;
	string _pwd;
	string _dbname;

	int _initSize;//���ӳصĳ�ʼ������
	int _maxSize;//���ӳص����������
	int _maxIdleTime;//���ӳص�������ʱ��
	int _connectionTimeout;//���ӳصĻ�ȡ���ӳ�ʱʱ��

	queue<Connection*> _connectionQue;//�洢mysql���ӵĶ���
	mutex _queueMtx;//�������Ӷ��еĻ�����
	atomic_int _connectionCnt;//���ӳ��е�ǰ���ӵ�����
	condition_variable cv;//��������������֪ͨ�������̺߳��������߳�
};