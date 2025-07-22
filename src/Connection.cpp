#include"public.h"
#include"Connection.h"

// ��ʼ�����ݿ�����
Connection::Connection()
{
	_conn = mysql_init(nullptr);
}

// �ͷ����ݿ�������Դ
Connection::~Connection()
{
	if (_conn != nullptr)
		mysql_close(_conn);
}

// �������ݿ�
bool Connection::connect(string ip, unsigned short port, string user, string pwd,
	string dbname)
{
	MYSQL* sql = mysql_real_connect(_conn, ip.c_str(), user.c_str(),
		pwd.c_str(), dbname.c_str(), port, nullptr, 0);
	return sql != nullptr;
}

// ���²��� insert��delete��update
bool Connection::update(string sql)
{
	if (mysql_query(_conn, sql.c_str())) {
		LOG("����ʧ��:" + sql);
		return false;
	}
	else {
		LOG("���³ɹ�:" + sql);
	}
	return true;
}

// ��ѯ���� select
MYSQL_RES* Connection::query(string sql)
{
	if (mysql_query(_conn, sql.c_str()))
	{
		LOG("��ѯʧ��" + sql);
		return nullptr;
	}
	else
	{
		LOG("��ѯ�ɹ�" + sql);
	}

	return mysql_use_result(_conn);
}