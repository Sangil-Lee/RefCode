#include <boost/signals2/signal.hpp>
#include <boost/bind.hpp> 
#include <iostream>  

using namespace boost;
//using namespace boost::signals;
using namespace std;  

class SignalClass {     
public:
	signals2::signal<void ()>    SigA;     
	signals2::signal<void (int)> SigB; 
};  

class SlotClass {     
public:
	void PrintFoo()      { cout << "Foo" << endl; }     
	void PrintInt(int i) { cout << "Bar: " << i << endl; } 
	void Multiply(int i) { cout << "Multiple: " << i*i << endl; }
};  

int main() 
{     
	SignalClass a;     
	SlotClass b, b2;      

	a.SigA.connect(bind(&SlotClass::PrintFoo, &b));     
	a.SigB.connect(bind(&SlotClass::PrintInt, &b,  _1));     
	a.SigB.connect(bind(&SlotClass::PrintInt, &b2, _1));      

	a.SigB.connect(bind(&SlotClass::Multiply, &b, _1));      
	a.SigB.connect(bind(&SlotClass::Multiply, &b2, _1));      

	a.SigA();     
	a.SigB(4); 
	a.SigB(11); 
} 
