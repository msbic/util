#include "stdafx.h"
#include <cstdio>
#include <string>
#include <memory>
#include <functional>
#include <algorithm>
#include <vector>
#include <array>
#include <tuple>
#include <thread>
#include <mutex>
#include <future>

using std::string;
using std::vector;
using std::function;
using std::array;
using std::tuple;
using std::unique_ptr;
using std::thread;
using std::shared_ptr;
using std::future;

/*----------------------------------------
  Demonstrates some of the features
  of the C++11 standard
-----------------------------------------*/

class Base
{
public:
	//Base& operator= (const Base&) = delete; // Can't make copies
	//Base (const Base&) = delete; // Can't make copies
	virtual void PrintNumber(int i) = 0;
	virtual ~Base() {}
};

class Derived : public Base
{
public:
	Derived() : Derived("Unknown")
	{}

	Derived(string&& strName)
	{
		m_strName = std::move(strName);
	}

	Derived(const string& strName) 
	{
		m_strName = strName;
	}

	/*void PrintNumber(long l) override; This line does not compile. Overriding method must have same signature */
	void PrintNumber(int i) override /* Makes sure the correct virtual function is ovverriden */
	{
		printf("Number squared = %d\n", i * i);
	}

	~Derived() 
	{
		printf("In %s My Name: %s\n", __FUNCTION__, m_strName.data());
	}

private:
	string m_strName = "Me";
};

class Test
{
public:
	Test(int nAge) : m_nAge(nAge) {}
	Test(const string& strAge) : Test(std::stoi(strAge)) {} // Delegating constructor
	string GetFullNameAndAge () const
	{
		return m_strFirstName + " " + m_strLastName + " " + std::to_string(m_nAge); 
	}
private:
	string m_strFirstName = "Miguel"; // In class member initialization
	string m_strLastName = "Cervantez";
	int m_nAge = 200;
};

void TestDelegatingConstructor()
{
	printf("\nIn %s\n", __FUNCTION__);
	Test t("45");
	printf("%s\n", t.GetFullNameAndAge().data());
}

// Demostrates the override feature
// for virtual functions
// See Base and Derived classes
// Also shows off unique_ptr
void TestOverrideAndUniqPtr()
{
	printf("\nIn %s\n", __FUNCTION__);
	std::unique_ptr<Base> derived(new Derived());
	derived->PrintNumber(5);
}

// Demostrates in class member initialization feature
// See class Test
void TestClassInitializer()
{
	printf("\nIn %s\n", __FUNCTION__);
	Test t(36);
	printf("%s\n", t.GetFullNameAndAge().data());	
}

// Demostrates Raw strings
void TestRawLiterals()
{
	printf("\nIn %s\n", __FUNCTION__);
	string test (R"(C:\A\B\C\D\file1.txt)");
	printf("%s\n", test.data());
}

// Shows off the auto feature
// as well as vector initialzer list
void TestAutoAndVectorInitList()
{
	printf("\nIn %s\n", __FUNCTION__);
	vector<string> weekdays = { "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "Sunday" };
	for (auto v = weekdays.cbegin(); v != weekdays.cend(); ++v)
	{
		printf("\n%s ", v->data());		
	}
}

// Demos the decltype c++11 feature
void TestDeclType()
{
	printf("\nIn %s\n", __FUNCTION__);
	int a = 0;

	// b will have the same time as a
	decltype(a) b = 10;
	
}

// Demostrates mem_fn function
// Creating "pointers" to member functions
void TestMemFn()
{
	printf("\nIn %s\n", __FUNCTION__);
	function <string(Test&)> func = std::mem_fn(&Test::GetFullNameAndAge);
	Test t(33);
	printf ("%s\n", func(t).data());
}

// Shows off lambda functions
// as well as std::array data structure
void TestLambdaAndArray()
{
	printf("\nIn %s\n", __FUNCTION__);
	array<int, 10> numList = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };

	// Copy filtered results into a new vector
	vector<int> filteredList;
	std::copy_if(std::begin(numList), std::end(numList), std::back_inserter(filteredList), [](int x) { return (x % 2 == 0); });

	printf("Even Numbers: ");
	for (auto n : filteredList)
	{
		printf("%d ", n);
	}

	printf("\n");
}

// Demostrates the tuple data structure
void TestTuple()
{
	printf("\nIn %s\n", __FUNCTION__);
	auto t = std::make_tuple(1, "Hello", 0.5);
	int i /*= std::get<0>(t)*/;
	string str/* = std::get<1>(t)*/;
	double d/* = std::get<2>(t)*/;
	std::tie(i, str, d) = t;

	printf("int = %d string = %s double = %.2lf\n", i, str.data(), d);

}

// Shows off the std::bind function
void TestBind()
{}

enum class Color { Red, Green, Yellow };

void TestEnumClass()
{
	printf("\nIn %s\n", __FUNCTION__);
	Color c = Color::Red;
	printf("Color = %i\n", c);
}

vector<string> MakeStringVector()
{
	vector<string> v = { "Red", "Green", "Blue", "Yellow" };

	auto& x = v[0];

	printf("\nAddress of vector first element: 0x%x\n", &x);
	return v;
}

// Testing move semantics
// STL data structures use move ctor and move assignment operator
void TestMove()
{
	printf("\nIn %s\n", __FUNCTION__);

	// It is now very efficient to return
	// STL collections by value, because
	// they implement the move ctor/move assignment operator
	auto v = MakeStringVector();
	auto& x = v[0];

	printf("\nAddress of vector first element: 0x%x\n", &x);
}

// Shows off storing unique_ptr objects in a vector
void TestUniquePtrInVector()
{
	printf("\nIn %s\n", __FUNCTION__);
	vector<unique_ptr<Base>> v;
	v.push_back(unique_ptr<Base> (new Derived("Oval")));
	v.push_back(unique_ptr<Base>(new Derived("Square")));
	v.push_back(unique_ptr<Base>(new Derived("Triangle")));
	v.push_back(unique_ptr<Base>(new Derived("Rectangle")));
	v.push_back(unique_ptr<Base>(new Derived("Hexagon")));
}

// Using new standard thread class with a labmda function
void TestThreadAndLambda()
{
	printf("\nIn %s main thread id: 0x%x\n", __FUNCTION__, std::this_thread::get_id());
	thread th([]() { printf("In a new thread, thread ID: 0x%x\n", std::this_thread::get_id()); });
	th.join();
}

int PerformSimpleCalculation(int i)
{
	printf("\nIn %s thread id: 0x%x\n", __FUNCTION__, std::this_thread::get_id());
	return i * i;
}

int PerformComplicatedCalculation(int i, int j)
{
	printf("\nIn %s thread id: 0x%x\n", __FUNCTION__, std::this_thread::get_id());
	return i * j;
}

// Shows simple async functionality in C++11
void TestAsync()
{
	printf("\nIn %s main thread id: 0x%x\n", __FUNCTION__, std::this_thread::get_id());
	auto result = std::async(PerformSimpleCalculation, 5);
	auto result2 = std::async(PerformComplicatedCalculation, 5, 10);

	for (int i = 0; i < 10; ++i)
	{
		printf("Doing some heavy duty work: %d\n", i);
	}

	printf ("Result of async: %d\n", result.get() * result2.get());
	
}

void TestSharedPtr()
{
	printf("\nIn %s\n", __FUNCTION__);
	shared_ptr<Base> ptr = std::make_shared<Derived>("Capitain Hook");

}

//constexpr int ConstTestFunc(int x)
//{
//	return x + 1;
//}
//
//void TestConstExpr()
//{
//	printf("\nIn %s\n", __FUNCTION__);
//	const int nNumber = ConstTestFunc(15);
//	printf("const number = %d\n", nNumber);
//}

// Demostrates std::function usage
// std::function is safer than function pointers
// and more convenient than a struct with operator()
void TestStdFunction()
{	
	vector<function<void()>> functionList =
	{
		TestRawLiterals,
		TestClassInitializer,
		TestDelegatingConstructor,
		TestOverrideAndUniqPtr,
		TestAutoAndVectorInitList,
		TestMemFn,
		TestLambdaAndArray,
		TestTuple,
		TestEnumClass,
		TestMove,
		TestUniquePtrInVector,
		TestThreadAndLambda,
		TestAsync,
		TestSharedPtr
	};

	for (auto& f : functionList)
	{
		f();
	}

	//for (auto& v = functionList.begin(); v != functionList.end(); ++v)
	//{
	//	v->operator()();
	//}
}

int main(int argc, char const *argv[])
{
	TestStdFunction();
	
	int x;
	scanf_s("%d", &x);
	return 0;
}