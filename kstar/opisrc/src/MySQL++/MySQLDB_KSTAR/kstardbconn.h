//#ifndef MYSQL_KSTARDBCONN_H
#if !defined(MYSQL_KSTARDBCONN_H)
#define MYSQL_KSTARDBCONN_H
#include <mysql++.h>

using namespace std;
class MySQLKSTARDB
{
	//interface method
public:
	MySQLKSTARDB();
	virtual ~MySQLKSTARDB();
	bool Connect_to_db(const string &dbname, const string &hostname, const string &dbuser, const string &passwd,
				mysqlpp::Connection &conn);
	void Create_kstardb(const string &dbname, mysqlpp::Connection &conn);
	void Print_row(mysqlpp::sql_bigint& index, mysqlpp::sql_char& pvname, mysqlpp::sql_double& pvval, 
			mysqlpp::sql_date& date);


	//class method
private:


	//member variable
private:
	bool m_new_db;
};
#endif
