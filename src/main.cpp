#define  _CRT_SECURE_NO_WARNINGS
#include<iostream>
#include"Connection.h"
#include"CommonConnectionPool.h"
using namespace std;

int main()
{
#if 0
	clock_t begin = clock();
	ConnectionPool* cp = ConnectionPool::GetConnectionPool();
	for (int i = 0; i < 1000; i++)
	{
		/*Connection con;
		char sql[1024] = { 0 };
		sprintf(sql, "insert into user(name,age,sex) values('%s',%d,'%s')",
			"zhang san", 18, "male");
		con.connect("127.0.0.1", 3306, "root", "root", "chat");
		con.update(sql);*/
		shared_ptr<Connection> sp = cp->getConnection();
		char sql[1024] = { 0 };
		sprintf(sql, "insert into user(name,age,sex) values('%s',%d,'%s')",
			"zhang san", 18, "male");
		sp->update(sql);

	}
#endif

	Connection con;
	con.connect("127.0.0.1", 3306, "root", "root", "chat");
	clock_t begin = clock();
	thread t1([]() {
		//ConnectionPool* cp = ConnectionPool::GetConnectionPool();
		for (int i = 0; i < 250; i++)
		{
			/*char sql[1024] = {0};
			sprintf(sql, "insert into user(name,age,sex) values('%s',%d,'%s')",
				"zhang san", 18, "male");
			shared_ptr<Connection> sp = cp->getConnection();
			sp->update(sql);*/
			Connection con;
			char sql[1024] = { 0 };
			sprintf(sql, "insert into user(name,age,sex) values('%s',%d,'%s')",
				"zhang san", 18, "male");
			con.connect("127.0.0.1", 3306, "root", "root", "chat");
			con.update(sql);
		}
	});

	thread t2([]() {
		//ConnectionPool* cp = ConnectionPool::GetConnectionPool();
		for (int i = 0; i < 250; i++)
		{
			/*char sql[1024] = {0};
			sprintf(sql, "insert into user(name,age,sex) values('%s',%d,'%s')",
				"zhang san", 18, "male");
			shared_ptr<Connection> sp = cp->getConnection();
			sp->update(sql);*/
			Connection con;
			char sql[1024] = { 0 };
			sprintf(sql, "insert into user(name,age,sex) values('%s',%d,'%s')",
				"zhang san", 18, "male");
			con.connect("127.0.0.1", 3306, "root", "root", "chat");
			con.update(sql);
		}
		});

	thread t3([]() {
		//ConnectionPool* cp = ConnectionPool::GetConnectionPool();
		for (int i = 0; i < 250; i++)
		{
			/*char sql[1024] = {0};
			sprintf(sql, "insert into user(name,age,sex) values('%s',%d,'%s')",
				"zhang san", 18, "male");
			shared_ptr<Connection> sp = cp->getConnection();
			sp->update(sql);*/
			Connection con;
			char sql[1024] = { 0 };
			sprintf(sql, "insert into user(name,age,sex) values('%s',%d,'%s')",
				"zhang san", 18, "male");
			con.connect("127.0.0.1", 3306, "root", "root", "chat");
			con.update(sql);
		}
		});

	thread t4([]() {
		//ConnectionPool* cp = ConnectionPool::GetConnectionPool();
		for (int i = 0; i < 250; i++)
		{
			/*char sql[1024] = {0};
			sprintf(sql, "insert into user(name,age,sex) values('%s',%d,'%s')",
				"zhang san", 18, "male");
			shared_ptr<Connection> sp = cp->getConnection();
			sp->update(sql);*/
			Connection con;
			char sql[1024] = { 0 };
			sprintf(sql, "insert into user(name,age,sex) values('%s',%d,'%s')",
				"zhang san", 18, "male");
			con.connect("127.0.0.1", 3306, "root", "root", "chat");
			con.update(sql);
		}
		});

	t1.join();
    t2.join();
    t3.join();
    t4.join();


    clock_t end = clock();
    cout << "time:" << end - begin << endl;


	return 0;
}