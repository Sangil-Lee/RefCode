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
MySQLKSTARDB::Create_kstardb(const string &dbname, mysqlpp::Connection &conn)
{
	mysqlpp::NoExceptions ne(conn);
	mysqlpp::Query query = conn.query();

	if (conn.select_db(dbname.c_str())) {
		// Toss old table, if it exists.  If it doesn't, we don't
		// really care, as it'll get created next.
		cout << "Dropping existing sample data tables..." << endl;
		query.execute("drop table kstartbl");
	}
	else 
	{
		// Database doesn't exist yet, so create and select it.
		cout << "Creating kstardb table..." << endl;
		if (conn.create_db(dbname.c_str()) &&
				conn.select_db(dbname.c_str())) {
			m_new_db = true;
		}
		else {
			cerr << "Error creating DB: " << conn.error() << endl;
			return;
		}
	};

	// Create sample data table within sample database.
	try {
		// Send the query to create the stock table and execute it.
		cout << "Creating kstartbl table..." << endl;
		mysqlpp::Query query = conn.query();
		query.reset();
		query << "CREATE TABLE kstartbl (" << "  pvindex BIGINT, " <<
				"  pvname CHAR(28) NOT NULL, " << "  pvval DOUBLE, " <<
				"  pvdate DATE) " << "ENGINE = InnoDB " <<
				"CHARACTER SET utf8 COLLATE utf8_general_ci";
		query.execute();

		// Set up the template query to insert the data.  The parse()
		// call tells the query object that this is a template and
		// not a literal query string.
		query << "insert into %4:table values (%0q, %1q, %2, %3q)";
		query.parse();

		// Set the template query parameter "table" to "stock".
		query.def["table"] = "kstartbl";

		// Notice that we don't give a sixth parameter in these calls,
		// so the default value of "stock" is used.  Also notice that
		// the first row is a UTF-8 encoded Unicode string!  All you
		// have to do to store Unicode data in recent versions of MySQL
		// is use UTF-8 encoding.
		cout << "kstar table intial value..." << endl;
		query.execute(1, "TMS_PVNAME_001", 12.453, "2007-06-20");
		query.execute(2, "TMS_PVNAME_002", 24.341, "2007-06-19");
		query.execute(3, "TMS_PVNAME_003", 53.534, "2007-06-18");
		query.execute(4, "TMS_PVNAME_004", 213.534,"2007-06-17");

		// Report success
		cout << (m_new_db ? "Created" : "Reinitialized") <<
				" kstardb database successfully." << endl;
	}
	catch (const mysqlpp::BadQuery& er) {
		// Handle any query errors
		cerr << "Query error: " << er.what() << endl;
		return;
	}
	catch (const mysqlpp::BadConversion& er) {
		// Handle bad conversions
		cerr << "Conversion error: " << er.what() << endl <<
				"\tretrieved data size: " << er.retrieved <<
				", actual size: " << er.actual_size << endl;
		return;
	}
	catch (const mysqlpp::Exception& er) {
		// Catch-all for any other MySQL++ exceptions
		cerr << "Error: " << er.what() << endl;
		return;
	};
}

void 
MySQLKSTARDB::Print_row( mysqlpp::sql_bigint& index,
			mysqlpp::sql_char& pvname, mysqlpp::sql_double& pvval, mysqlpp::sql_date& date)
{
	cout << setw(10) << index << ' ' << setw(9) << pvname << ' ' << setw(9) << pvval << ' ' <<
			setw(9) << date << endl;
}
