#include <boost/thread.hpp>
#include <boost/shared_ptr.hpp>
#include <string>

class Data
{
public:
	int i;
	std::string s;
	~Data() { printf ("in %s\n", __FUNCTION__);}
};

class ThreadTester
{
public:
	ThreadTester()
	{
		printf("in %s\n", __FUNCTION__);
		boost::thread tr(&ThreadTester::AllocData, this);
		tr.detach();
	}

	void AllocData()
	{
		printf("in %s\n", __FUNCTION__);
		boost::mutex::scoped_lock lock(m_mutex);
		m_pData.reset(new Data);
		m_bWorkDone = true;
		m_cond.notify_one();
	}

	const boost::shared_ptr<Data>& GetData()
	{
		return m_pData;
	}

	void Wait()
	{
		boost::mutex::scoped_lock lock(m_mutex);
		while (!m_bWorkDone)
			m_cond.wait(lock);
	}
private:
	bool m_bWorkDone = false;
	boost::mutex m_mutex;
	boost::condition_variable  m_cond;
	boost::shared_ptr<Data> m_pData;
};

void DeleteData(Data* pData)
{
	printf("in %s\n", __FUNCTION__);
}

int main ()
{
	printf("in %s\n", __FUNCTION__);
	ThreadTester t;
	t.Wait();
	boost::shared_ptr<Data> pData = t.GetData();
	printf("i = %d s = %s\n", pData->i, pData->s.c_str());
	DeleteData(pData.get());
	return 0;
}