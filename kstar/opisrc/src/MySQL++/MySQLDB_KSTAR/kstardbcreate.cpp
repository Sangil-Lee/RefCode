#include <mysql++.h>
#include <sys/time.h>
#include <iostream>

#include <kstardb.h>
#include <kstardbconn.h>

using namespace std;
using namespace mysqlpp;

sql_create_2(kstartbl_subset, 1, 0, 
		mysqlpp::sql_char, pvname,
		mysqlpp::sql_double, pvval
		)

void MicroTimePrint ( long *sec, double *msec )
{
	struct timeval tp;

	if( gettimeofday((struct timeval *)&tp,NULL) == 0 )
	{
		(*msec)=(double)(tp.tv_usec/1000000.00);
		(*sec)=tp.tv_sec;
		if((*msec)>=1.0) (*msec)-=(long)(*msec);
		printf("sec: %d, msec:%f\n", *sec, *msec);
	};
};


int main(int argc, char *argv[])
{
	if ( mysqlpp::get_library_version() != MYSQLPP_HEADER_VERSION )
	{
		cerr<< "Library/header version number mismatch" << endl;
		return -1;
	}

	MySQLKSTARDB mysqlkstardb;
	string dbname   = "kstardb";
	string hostname = "172.17.100.116";
	string user     = "root";
	string passwd   = "1234";
	mysqlpp::Connection con(mysqlpp::use_exceptions);
#if 0
//Test 1. DB Create.
	try 
	{
		cout << "Connection to MySQL Server..." << endl;
		if( !mysqlkstardb.Connect_to_db(dbname, hostname, user, passwd, con) )
		{
			return -1;
		}
		else 
		{
			mysqlkstardb.Create_kstardb(dbname, con);
		};
	}
	catch (exception &er)
	{
		cerr << "Connection Failed to MySQL Server..." << endl;
		return -1;
	};
#endif
#if 0
//Test 2. SQL Search.
	long sec1 = 0, sec2 = 0;
	double msec1 = 0., msec2 = 0.;
	try
	{
		if( !mysqlkstardb.Connect_to_db(dbname, hostname, user, passwd, con) )
		{
			return -1;
		}

		mysqlpp::Query query = con.query();
		query << "select * from kstartbl";

		vector<kstartbl> res;
		query.storein(res);

		MicroTimePrint (&sec1, &msec1 );
		// Display the result set
		vector<kstartbl>::iterator it;
		for (it = res.begin(); it != res.end(); ++it) {
			mysqlkstardb.Print_row(it->pvindex, it->pvname, it->pvval, it->pvdate);
		};
		MicroTimePrint (&sec2, &msec2 );
		printf("sec diff: %d, msec diff: %d\n", (sec2 - sec1), (int)((msec2-msec1)*1000));
	}
	catch (const mysqlpp::BadQuery& er) {
		cerr << "Query error: " << er.what() << endl;
		return -1;
	}
	catch (const mysqlpp::BadConversion& er) {
		cerr << "Conversion error: " << er.what() << endl <<
				"\tretrieved data size: " << er.retrieved <<
				", actual size: " << er.actual_size << endl;
		return -1;
	}
	catch (const mysqlpp::Exception& er) {
		cerr << "Error: " << er.what() << endl;
		return -1;
	}
#endif
#if 0
//Test 3. SQL Insert
	try
	{
		if( !mysqlkstardb.Connect_to_db(dbname, hostname, user, passwd, con) )
		{
			return -1;
		}

		mysqlpp::Query query = con.query();
		char pvbuf[28];
		for ( int i = 5; i < 10000; i++ )
		{
			double drandval = (double)(rand()/100000.0);
			sprintf(pvbuf, "VMS_PVNAME_%04d", i);
			kstartbl ktbladd(i, pvbuf, drandval, "2007-06-22");
			// Form the query to insert the row into the stock table.
			query.insert(ktbladd);
			query.execute();
		};


		// Show the query about to be executed.
		//cout << "Query: " << query.preview() << endl;

		// Execute the query.  We use execute() because INSERT doesn't
		// return a result set.
		//query.execute();
	}
	catch (const mysqlpp::BadQuery& er) {
		// Handle any query errors
		cerr << "Query error: " << er.what() << endl;
		return -1;
	}
	catch (const mysqlpp::BadConversion& er) {	
		// Handle bad conversions
		cerr << "Conversion error: " << er.what() << endl <<
				"\tretrieved data size: " << er.retrieved <<
				", actual size: " << er.actual_size << endl;
		return -1;
	}
	catch (const mysqlpp::Exception& er) {
		// Catch-all for any other MySQL++ exceptions
		cerr << "Error: " << er.what() << endl;
		return -1;
	};
#endif
#if 0
//Test 4. SQL Insert
	try
	{
		if( !mysqlkstardb.Connect_to_db(dbname, hostname, user, passwd, con) )
		{
			return -1;
		};

		// Build a query to retrieve the stock item that has Unicode
		// characters encoded in UTF-8 form.
		mysqlpp::Query query = con.query();
		query << "select * from kstartbl where pvname = \"TMS_PVNAME_003\"";

		// Retrieve the row, throwing an exception if it fails.
		mysqlpp::Result res = query.store();

		if (res.empty()) {
			throw mysqlpp::BadQuery("UTF-8 bratwurst item not found in "
					"table, run resetdb");
		}

		// Because there should only be one row in the result set,
		// there's no point in storing the result in an STL container.
		// We can store the first row directly into a stock structure
		// because one of an SSQLS's constructors takes a Row object.
		kstartbl row = res.at(0);

		// Create a copy so that the replace query knows what the
		// original values are.
		kstartbl orig_row = row;

		// Change the stock object's item to use only 7-bit ASCII, and
		// to deliberately be wider than normal column widths printed
		// by print_stock_table().
		row.pvval = 0.195;

		// Form the query to replace the row in the stock table.
		query.update(orig_row, row);

		// Show the query about to be executed.
		cout << "Query: " << query.preview() << endl;

		// Run the query with execute(), since UPDATE doesn't return a
		// result set.
		query.execute();
	}
	catch (const mysqlpp::BadQuery& er) {
		cerr << "Query error: " << er.what() << endl;
		return -1;
	}
	catch (const mysqlpp::BadConversion& er) {
		cerr << "Conversion error: " << er.what() << endl <<
				"\tretrieved data size: " << er.retrieved <<
				", actual size: " << er.actual_size << endl;
		return -1;
	}
	catch (const mysqlpp::Exception& er) {
		cerr << "Error: " << er.what() << endl;
		return -1;
	};
#endif
#if 0
	try
	{
		if( !mysqlkstardb.Connect_to_db(dbname, hostname, user, passwd, con) )
		{
			return -1;
		};

		// Retrieve all rows from the stock table and put them in an
		// STL set.  Notice that this works just as well as storing them
		// in a vector, which we did in custom1.cpp.  It works because
		// SSQLS objects are less-than comparable.
		mysqlpp::Query query = con.query();
		query << "select * from kstartbl";

		set<kstartbl> res;
		query.storein(res);

		// Display the result set.  Since it is an STL set and we set up
		// the SSQLS to compare based on the item column, the rows will
		// be sorted by item.
		set<kstartbl>::iterator it;
		cout.precision(3);
		for (it = res.begin(); it != res.end(); ++it) {
			//mysqlkstardb.Print_row(it->pvindex, it->pvname, it->pvval, it->pvdate);
			cout <<"index: " << it->pvindex <<" pvname: " << it->pvname << " pvval: " <<
				it->pvval <<" date: " << it->pvdate << endl;
		}

		// Use set's find method to look up a stock item by item name.
		// This also uses the SSQLS comparison setup.
		//kstartbl() -> Primary key 
		it = res.find(kstartbl(4));
		if (it != res.end()) {
			cout << endl << "PvIndex: " << it->pvindex << " " << it->pvname <<
					"  in kstar table." << endl;
		}
		else {
			cout << endl << "Sorry, no TMS_PVNAME_002 buns in kstar table." << endl;
		}
	}
	catch (const mysqlpp::BadQuery& er) {
		cerr << "Query error: " << er.what() << endl;
		return -1;
	}
	catch (const mysqlpp::BadConversion& er) {
		cerr << "Conversion error: " << er.what() << endl <<
				"\tretrieved data size: " << er.retrieved <<
				", actual size: " << er.actual_size << endl;
		return -1;
	}
	catch (const mysqlpp::Exception& er) {
		cerr << "Error: " << er.what() << endl;
		return -1;
	}
#endif
#if 0
//Test 6. 

// To store a subset of a row, we define an SSQLS containing just the
// fields that we will store.  There are complications here that are
// covered in the user manual.
	try
	{
		if( !mysqlkstardb.Connect_to_db(dbname, hostname, user, passwd, con) )
		{
			return -1;
		};

		// Retrieve a subset of the stock table, and store the data in
		// a vector of 'stock_subset' SSQLS structures.
		mysqlpp::Query query = con.query();
		query << "select pvname, pvval from kstartbl";
		vector<kstartbl_subset> res;
		query.storein(res);

		// Display the result set
		cout << "We have:" << endl;
		vector<kstartbl_subset>::iterator it;
		for (it = res.begin(); it != res.end(); ++it) {
			printf("pvname: %s, pvval: %f\n", it->pvname.c_str(), it->pvval );
			//cout << '\t' <<"PVName: " << it->pvname << '\t'
			//<<"Value: " << it->pvval << endl;
		}
	}
	catch (const mysqlpp::BadQuery& er) {
		// Handle any query errors
		cerr << "Query error: " << er.what() << endl;
		return -1;
	}
	catch (const mysqlpp::BadConversion& er) {
		// Handle bad conversions; e.g. type mismatch populating 'stock'
		cerr << "Conversion error: " << er.what() << endl <<
				"\tretrieved data size: " << er.retrieved <<
				", actual size: " << er.actual_size << endl;
		return -1;
	}
	catch (const mysqlpp::Exception& er) {
		// Catch-all for any other MySQL++ exceptions
		cerr << "Error: " << er.what() << endl;
		return -1;
	}
#endif
#if 0 
//Test 7. SQL Update for table all
	try
	{
		if( !mysqlkstardb.Connect_to_db(dbname, hostname, user, passwd, con) )
		{
			return -1;
		};

		mysqlpp::Query query = con.query();
		long sec1 = 0, sec2 = 0;
		double msec1 = 0., msec2 = 0.;
		MicroTimePrint (&sec1, &msec1 );
		for ( int i = 0; i < 10000;i++)
		{
			//query << "update kstartbl set pvval=50348342.2942";
			//query <<"update kstartbl set pvval=" << i+12.43 <<" where pvindex=" <<i;
			query.reset();
			query <<"update kstartbl set pvval = %0q where pvindex = %1q";
			query.parse();
			query.execute(i+1.245, i);
		}
		MicroTimePrint (&sec2, &msec2 );

		printf("sec diff: %d, msec diff: %d\n", (sec2 - sec1), (int)((msec2-msec1)*1000));
	}
	catch (const mysqlpp::BadQuery& er) {
		cerr << "Query error: " << er.what() << endl;
		return -1;
	}
	catch (const mysqlpp::BadConversion& er) {
		cerr << "Conversion error: " << er.what() << endl <<
				"\tretrieved data size: " << er.retrieved <<
				", actual size: " << er.actual_size << endl;
		return -1;
	}
	catch (const mysqlpp::Exception& er) {
		cerr << "Error: " << er.what() << endl;
		return -1;
	};
#endif
#if 0
//Test 8. 
	long sec1 = 0, sec2 = 0;
	double msec1 = 0., msec2 = 0.;
	try
	{
		if( !mysqlkstardb.Connect_to_db(dbname, hostname, user, passwd, con) )
		{
			return -1;
		}

		mysqlpp::Query query = con.query();
		//query << "select pvindex, pvname, pvval from kstartbl where pvindex > 1 and pvindex <= 1000 order by pvname desc";
		//query << "select pvindex, pvname, pvval from kstartbl where pvname like \"TMS_%\"";
		//query << "select pvindex, pvname, pvval from kstartbl where pvname like \"VMS_%\"";
		query << "select pvindex, pvname, pvval from kstartbl where pvname like \"%101%\"";
		mysqlpp::Result res = query.store();
		MicroTimePrint (&sec1, &msec1 );
		//mysqlpp::Result::iterator it;
		mysqlpp::Row row;
		// Display the result set
		//for (it = res.begin(); it != res.end(); ++it) {
		for (size_t i = 0; i < res.size(); ++i) {
			row = res.at(i);
			//printf("pvindex:%d, pvname:%s, pvval:%f\n", row.at(0), row.at(1), row.at(2));
			cout << "pvindex: "<<row.at(0)<<" pvname: " << row.at(1) << " pvval: "<<row.at(2)<<endl;
		};
		MicroTimePrint (&sec2, &msec2 );
		printf("sec diff: %d, msec diff: %d\n", (sec2 - sec1), (int)((msec2-msec1)*1000));
	}
	catch (const mysqlpp::BadQuery& er) {
		cerr << "Query error: " << er.what() << endl;
		return -1;
	}
	catch (const mysqlpp::BadConversion& er) {
		cerr << "Conversion error: " << er.what() << endl <<
				"\tretrieved data size: " << er.retrieved <<
				", actual size: " << er.actual_size << endl;
		return -1;
	}
	catch (const mysqlpp::Exception& er) {
		cerr << "Error: " << er.what() << endl;
		return -1;
	}
#endif
#if 0
//Test 9. 
	long sec1 = 0, sec2 = 0;
	double msec1 = 0., msec2 = 0.;
	try
	{
		if( !mysqlkstardb.Connect_to_db(dbname, hostname, user, passwd, con) )
		{
			return -1;
		}

		mysqlpp::Query query = con.query();
		query << "select pvindex, pvname, pvval from kstartbl where pvname like \"%101%\"";
		mysqlpp::ResUse res = query.use();
		MicroTimePrint (&sec1, &msec1 );
		mysqlpp::Row row;
		// Display the result set
		while (row = res.fetch_row() )
		{
			printf("row_size:%d\n", row.size());
			cout << "pvindex: " << row["pvindex"]<<" pvname: " << row["pvname"] << " pvval: "<<row["pvval"]<<endl;
		};
		MicroTimePrint (&sec2, &msec2 );
		printf("sec diff: %d, msec diff: %d\n", (sec2 - sec1), (int)((msec2-msec1)*1000));
	}
	catch (const mysqlpp::Exception& er) {
		cerr << "Error: " << er.what() << endl;
		return -1;
	}
#endif
#if	1 
//Test 10. 
	long sec1 = 0, sec2 = 0;
	double msec1 = 0., msec2 = 0.;
	try
	{
		if( !mysqlkstardb.Connect_to_db(dbname, hostname, user, passwd, con) )
		{
			return -1;
		}

		mysqlpp::Query query = con.query();
		query << "select pvindex, pvname, pvval from kstartbl where pvname like \"%101%\"";
		mysqlpp::ResUse res = query.use();
		MicroTimePrint (&sec1, &msec1 );
		mysqlpp::Row row;
		// Display the result set
		while (row = res.fetch_row())
		{
			cout << "pvindex: " << row["pvindex"]<<" pvname: " << row["pvname"] << " pvval: "<<row["pvval"]<<endl;
		};
		MicroTimePrint (&sec2, &msec2 );
		printf("sec diff: %d, msec diff: %d\n", (sec2 - sec1), (int)((msec2-msec1)*1000));
	}
	catch (const mysqlpp::BadQuery& er) {
		cerr << "Query error: " << er.what() << endl;
		return -1;
	}
	catch (const mysqlpp::Exception& er) {
		MicroTimePrint (&sec2, &msec2 );
		printf("sec diff: %d, msec diff: %d\n", (sec2 - sec1), (int)((msec2-msec1)*1000));
		cerr << "Error: " << er.what() << endl;
		return -1;
	}
#endif
#if 0
//Test 11. Meta infomation
	long sec1 = 0, sec2 = 0;
	double msec1 = 0., msec2 = 0.;
	try
	{
		if( !mysqlkstardb.Connect_to_db(dbname, hostname, user, passwd, con) )
		{
			return -1;
		}

		mysqlpp::Query query = con.query();
		query << "select pvindex, pvname, pvval from kstartbl where pvname like \"%101%\"";
		mysqlpp::Result res = query.store();
		MicroTimePrint (&sec1, &msec1 );
		mysqlpp::Row row;
		// Display the result set

		for (unsigned int i = 0; i < res.names().size(); i++) {
			cout << setw(2) << i
				// this is the name of the field
				<< setw(15) << res.names(i).c_str()
				// this is the SQL identifier name
				// Result::types(unsigned int) returns a mysql_type_info whic
				// ways is like type_info except that it has additional sql t
				// information in it. (with one of the methods being sql_name
				<< setw(15) << res.types(i).sql_name()
				// this is the C++ identifier name which most closely resembl
				// the sql name (its is implementation defined and often not 
				//<< setw(20) << res.types(i).name()
				<< endl;
		}
		cout << endl;
		if (res.types(0) == typeid(string)) {
			cout << "Field 'item' is of an SQL type which most "
				"closely resembles\nthe C++ string type\n";
		}
		if (res.types(1) == typeid(longlong)) {
			cout << "Field 'num' is of an SQL type which most "
				"closely resembles\nC++ long long int type\n";
		}
		else if (res.types(1).base_type() == typeid(longlong)) {
			cout << "Field 'num' base type is of an SQL type which "
				"most closely\nresembles the C++ long long int type\n";
		};

		MicroTimePrint (&sec2, &msec2 );
		printf("sec diff: %d, msec diff: %d\n", (sec2 - sec1), (int)((msec2-msec1)*1000));
	}
	catch (const mysqlpp::Exception& er) {
		cerr << "Error: " << er.what() << endl;
		return -1;
	}
#endif

	return 0;
}
