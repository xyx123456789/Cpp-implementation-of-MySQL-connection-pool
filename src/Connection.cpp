#include"public.h"
#include"Connection.h"

// 初始化数据库连接
Connection::Connection()
{
	_conn = mysql_init(nullptr);
}

// 释放数据库连接资源
Connection::~Connection()
{
	if (_conn != nullptr)
		mysql_close(_conn);
}

// 连接数据库
bool Connection::connect(string ip, unsigned short port, string user, string pwd,
	string dbname)
{
	MYSQL* sql = mysql_real_connect(_conn, ip.c_str(), user.c_str(),
		pwd.c_str(), dbname.c_str(), port, nullptr, 0);
	return sql != nullptr;
}

// 更新操作 insert、delete、update
bool Connection::update(string sql)
{
	if (mysql_query(_conn, sql.c_str())) {
		LOG("更新失败:" + sql);
		return false;
	}
	else {
		LOG("更新成功:" + sql);
	}
	return true;
}

// 查询操作 select
MYSQL_RES* Connection::query(string sql)
{
	if (mysql_query(_conn, sql.c_str()))
	{
		LOG("查询失败" + sql);
		return nullptr;
	}
	else
	{
		LOG("查询成功" + sql);
	}

	return mysql_use_result(_conn);
}