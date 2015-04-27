#include <kstardbconn.h>

MySQLKSTARDB::MySQLKSTARDB()
{
	 m_new_db = false;
}
MySQLKSTARDB::~MySQLKSTARDB()
{
}
bool 
MySQLKSTARDB::Connect_to_db(const string &dbname, const string &hostname, const string &dbuser, const string &passwd,
				mysqlpp::Connection &conn)
{
	//cout << "Database connect try **1: " << conn.error() << endl;
	//printf("dbname:%s, hostname:%s, user:%s, passwd:%s: \n", dbname.c_str(), hostname.c_str(), dbuser.c_str(), passwd.c_str() );
	try
	{
		conn.connect(dbname.c_str(), hostname.c_str(), dbuser.c_str(), passwd.c_str());
	}
	catch ( exception & e)
	{
		cout << "Database Connection Error: " << e.what() << endl;
	};

	if (!conn) 
	{
		cout << "Database connection failed: " << conn.error() << endl;
		return false;
	};

	return true;
}


void 
MySQLKSTARDB::Print_row( mysqlpp::sql_bigint& index,
			mysqlpp::sql_char& pvname, mysqlpp::sql_double& pvval, mysqlpp::sql_date& date)
{
	cout << setw(10) << index << ' ' << setw(9) << pvname << ' ' << setw(9) << pvval << ' ' <<
			setw(9) << date << endl;
}
