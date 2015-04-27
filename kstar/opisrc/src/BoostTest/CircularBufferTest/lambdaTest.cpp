#include <boost/lambda/lambda.hpp>
#include <boost/lambda/bind.hpp>
#include <boost/lambda/if.hpp>
#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;
using namespace boost::lambda;

class Test
{
public:
	//static void add(int i , int &j);
	void push(int i)
	{
		gv.push_back(i);
	};

	void add(int i , int &j)
	{
		j += i;
	};

	vector<int> gv;
};

void add(int i , int &j)
{
	j += i;
}

int main(int, char **)
{
	Test test;
	vector<int> v(10);
	for(int i = 0; i < 10; i++) v[i]=i;

	//for_each(v.begin(), v.end(), _1=1);
	for_each(v.begin(), v.end(), cout <<_1 <<","); cout << endl;

	//for_each(v.begin(), v.end(), bind(add, 10, _1));
	for_each(v.begin(), v.end(), bind(&Test::add, &test, 10, _1));
	for_each(v.begin(), v.end(), cout <<_1 <<","); cout << endl;

	for_each(v.begin(), v.end(), if_then(_1>15, bind(&Test::push, &test, _1)));
	//for_each(v.begin(), v.end(), if_then(_1>15, cout<<_1<<",")); cout<<endl;
	for_each(v.begin(), v.end(), if_then(_1, cout<<_1<<",")); cout<<endl;
	for_each(test.gv.begin(), test.gv.end(), cout<<_1<<","); cout<<endl;

	return 0;
}
