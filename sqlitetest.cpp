// cpptest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <tuple>
#include <string>
#include <iostream>
#include <vector>
#include <memory>
#include <locale>
#include <sstream>
#include <chrono>

extern "C"
{
#include "sqlite3.h"
}

using std::tuple;
using std::string;
using std::make_tuple;
using std::tuple_element;
using std::cout;
using std::vector;
using std::weak_ptr;


namespace db
{
	enum class DataType
	{
		DB_INT,
		DB_DATE_TIME,
		DB_STRING
	};

	template <class T, class... Ts> class Query
	{
	public:

		const tuple<T, Ts...>& Records() const
		{
			return m_record;
		}

	private:
		Query();
		vector< tuple<T, Ts...> > m_records;
	};

	class IDBEngine
	{
		virtual void Connect(const string& dbname) = 0;
		virtual bool IsConnected() const = 0;
		virtual void Disconnect() = 0;
		virtual long Execute(const string& query) = 0; // Delete/Insert/Update

		//template <class T, class... Ts>
		//virtual Query<T, Ts...> Execute(const string& query, const vector<string>& fieldNames, const vector<DataType>& fieldTypes) = 0; // Select
	};

	
}

template <class... Args>
struct type_list
{
	template <std::size_t N>
	using type = typename std::tuple_element<N, std::tuple<Args...>>::type;
};


using std::locale;

struct Track
{
	int ID = 0;
	string Name;
	int EdgeID = 1;
	int start = 0;
	int end = 0;
};


class SQLiteException : public std::exception
{
public:
	SQLiteException(const char* msg) : std::exception(msg)
	{}
};

class SQLiteDB
{
public:
	void Open(const std::string& dbname);
	void Execute(const std::string& query);
	void FetchData(const std::string& query);

	~SQLiteDB()
	{
		if (m_db != nullptr)
		{
			sqlite3_close(m_db);
		}
	}
private:
	sqlite3* m_db = nullptr;
};

void GetTableData(sqlite3* dbfile, vector<Track*>& tracks)
{
	sqlite3_stmt *statement;

	const char *query = "SELECT * FROM Tracks";

	if (sqlite3_prepare(dbfile, query, -1, &statement, 0) == SQLITE_OK)
	{
		int ctotal = sqlite3_column_count(statement);
		int res = 0;

		while (1)
		//for(int i = 0; i < 100000; ++i)
		{
			res = sqlite3_step(statement);

			if (res == SQLITE_ROW)
			{
				int ID = std::stoi ((const char*)sqlite3_column_text(statement, 0));
				string Name = (const char*)sqlite3_column_text(statement, 1);
				int EdgeID = std::stoi((const char*)sqlite3_column_text(statement, 2));
				int start = std::stoi((const char*)sqlite3_column_text(statement, 3));;
				int end = std::stoi((const char*)sqlite3_column_text(statement, 4));

				tracks.emplace_back(new Track{ ID, Name, EdgeID, start, end });			
			}

			if (res == SQLITE_DONE || res == SQLITE_ERROR)
			{/*
				cout << "done " << endl;*/
				break;
			}
		}
	}
}

const char* createTable = "CREATE TABLE Tracks \
(ID INTEGER PRIMARY KEY,\
	Name TEXT NOT NULL,\
	EdgeID INTEGER NOT NULL,\
	StartDisplacement INTEGER NOT NULL CHECK(StartDisplacement >= 0),\
	EndDisplacement INTEGER NOT NULL CHECK(EndDisplacement > StartDisplacement)) ";

int main()
{
	const char* dbName = "i:\\tmp\\test.db";
	char* errMsg = nullptr;

	sqlite3* dbfile;
	if (sqlite3_open(dbName, &dbfile) == SQLITE_OK)
	{
		auto rc = sqlite3_exec(dbfile, createTable, NULL, 0, &errMsg);
		if (rc != SQLITE_OK)
		{
			cout << errMsg << std::endl;
			sqlite3_free(errMsg);
		}

		vector<Track*> tracks;
		using namespace std::chrono;

		high_resolution_clock::time_point t1 = high_resolution_clock::now();

		GetTableData(dbfile, tracks);

		high_resolution_clock::time_point t2 = high_resolution_clock::now();
		duration<double> time_span = std::chrono::duration_cast<duration<double>>(t2 - t1);

		std::cout << "It took me " << time_span.count() << " seconds.";

		return 0;

		int result = -1;
		sqlite3_stmt *statement = nullptr;
		std::stringstream s;

		const char* begin = "BEGIN TRANSACTION;";
		const char* commit = "COMMIT;";

		if (sqlite3_prepare(dbfile, begin, -1, &statement, 0) == SQLITE_OK)
		{
			int res = sqlite3_step(statement);
			result = res;
			sqlite3_finalize(statement);
		}

		for (int i = 1; i <= 1000000; ++i)
		{
			s << "INSERT INTO Tracks VALUES( " << i << " , '" << std::to_string(i) << "' , " << i << " , " << "0, 50);";

			if (sqlite3_prepare(dbfile, s.str().c_str(), -1, &statement, 0) == SQLITE_OK)
			{
				int res = sqlite3_step(statement);
				result = res;
				sqlite3_finalize(statement);
			}

			s.str("");
		}

		if (sqlite3_prepare(dbfile, commit, -1, &statement, 0) == SQLITE_OK)
		{
			int res = sqlite3_step(statement);
			result = res;
			sqlite3_finalize(statement);
		}
		

		sqlite3_close(dbfile);
	}
    return 0;
}

