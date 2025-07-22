#pragma once
#include<string>
#include<mysql.h>
#include<ctime>
using namespace std;
//ʵ�����ݿ���ɾ�Ĳ�

// ���ݿ������
class Connection
{
public:
	// ��ʼ�����ݿ�����
	Connection();
	// �ͷ����ݿ�������Դ
	~Connection();
	// �������ݿ�
	bool connect(string ip, unsigned short port, string user, string password,
		string dbname);
	// ���²��� insert��delete��update
	bool update(string sql);
	// ��ѯ���� select
	MYSQL_RES* query(string sql);
	//ˢ��һ�����ӵ���ʼ���ʱ���
	void refreshAliveTime() { _aliveTime = clock(); }
	//���ش��ʱ��
	clock_t getAliveTime()const { return clock()-_aliveTime; }
private:
	MYSQL* _conn; // ��ʾ��MySQL Server��һ������
	clock_t _aliveTime;//�������״̬�����ʼ���ʱ��
};