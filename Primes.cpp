#include  <vector>

using std::vector;

vector<uint32_t> FindPrimes (uint32_t nMax)
{
	vector<uint32_t> primes = { 2, 3, 5, 7, 11, 13 };
	for (uint32_t i = 14; i <= nMax; ++i)
	{
		bool bIsPrime = true;
		for (auto p : primes)
		{
			if (i % p == 0)
			{
				bIsPrime = false;
			}
		}

		if (bIsPrime)
		{
			primes.push_back(i);
		}
	}

	return primes;
}

void PrintVec (const vector<uint32_t>& primes)
{
	for (auto p : primes)
	{
		printf("%u ", p);
	}
	printf("\n");
}

int main ()
{	
	PrintVec (FindPrimes (10000));

	return 0;
}