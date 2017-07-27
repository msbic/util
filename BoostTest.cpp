// BoostTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <boost/locale.hpp>
#include <boost/system/system_error.hpp>
#include <boost/thread.hpp>
#include <boost/format.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/foreach.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/function.hpp>
#include <iostream>
#include <strstream>
#include <fstream>

using namespace std;
using namespace boost::locale;
using boost::thread;

void TestLocale ()
{
	generator gen;	
	locale loc = gen("fr_FR.UTF-8");		
	wstringstream stream;
	stream.imbue(loc);
	stream << as::currency << 123.45;

	// str is a string that contains $123.45
	// formatted for a French (France) locale
	auto str = stream.str();
	wcout << str << endl;
}

void func()
{
	cout << __FUNCTION__ << endl;
}

void func1 (int param)
{
	cout << __FUNCTION__ << " " << param << endl;
}

class Test
{
public:
	Test() {}
	void run () { cout << __FUNCTION__ << endl; }
};

void TestThread()
{
	// Thread is a template
	// can take functions of any number of parameters
	thread th (func);

	// th is detached - we're not waiting for it
	th.detach();
	thread th2 (func1, 0);

	// wait for th2 to return
	th2.join();

	// It can take a member function
	// of any object
	Test t;
	boost::thread th3 (boost::bind(&Test::run, &t)); // Not a good example, t is on the stack
	th3.detach();
}

void TestFormat()
{
	// Type safe formatting
	cout << boost::format("%1% %2% %3%\n") % 1234 % "Younge" % "street"; 
}

// A function that returns multiple 
// values instead of just 1
boost::tuple<string, int, bool> FunctionThatReturnsTuple()
{
	return boost::make_tuple("Hello", 25, false);
}

void TestTuple()
{
	auto t = FunctionThatReturnsTuple();
	cout << get<0>(t) << endl;
	cout << get<1>(t) << endl;
}

// .NET style foreach loop
void TestForEach()
{
	std::string colors[] = { "red", "black", "white" };
	BOOST_FOREACH(string s, colors)
	{
		cout << s << endl;
	}
}

// Test trimg / split
void TestTextUtils()
{
	string s = "    Hello World   ";
	boost::trim_left(s);
	boost::trim(s); 
	cout << s << endl;

	vector<string> lst;
	string s2 = "Yellow,green,red";
	boost::split(lst, s2, boost::is_any_of(","), boost::token_compress_on);

	// lst = {"Yellow", "green", "red"};
}

class C
{
public:
	C() {}
	~C() { printf ("in %s\n", __FUNCTION__); }
	void SayHello () { printf("Hello from %s\n", __FUNCTION__); }
};

void TestC(C* pC)
{
	pC->SayHello();
}

void TestSmartPtr()
{
	boost::scoped_ptr<C> c(new C);
	TestC(c.get());
}


typedef boost::function<void()> fun;

int _tmain(int argc, _TCHAR* argv[])
{	
	boost::function<void()> funcs [] = 
	{
		TestSmartPtr,
		TestTextUtils,
		TestTuple,
		TestFormat,
		TestThread,
		TestLocale
	};

	BOOST_FOREACH(fun f, funcs)
	{
		f();
	}

	int i;
	cin >> i;
	return 0;
}

