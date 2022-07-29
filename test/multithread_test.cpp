#include "multithread/concurrent_queue.h"
#include "multithread/dispatcher.h"
#include "gtest/gtest.h"
#include <random>

bool Varify(vector<uint32>& v) {
	sort(v.begin(), v.end());
	al_for(i,0,v.size()) {
		if (i != v[i]) {
			return false;
		}
	}
	return true;
}

TEST(DispatcherTest, MultithreadTest) {

	al_for(i, 0, 50) {
		std::uniform_int_distribution<uint32> s_distribution(0, 10000);
		std::uniform_int_distribution<uint32> t_distribution(0, 16);
		std::default_random_engine seed_generator(time(NULL));
		uint32 size = s_distribution(seed_generator), threadn = t_distribution(seed_generator);

		ConcurrentQueue<uint32> q;
		al_for(j, 0, size) {
			q.push(j);
		}
		vector<uint32> vec;
		mutex m;

		Dispatcher dispatcher(threadn);
		dispatcher.Dispatch([&](uint32 i) {
			while (true) {
				uint32 v;
				if (auto res = q.try_pop(); !res.has_value()) {
					break;
				}
				else {
					v = res.value();
				}
				m.lock();
				vec.push_back(v);
				m.unlock();
			}
		});

		ASSERT_TRUE(Varify(vec)) << "varify fails at " << i << " threadn " << threadn << " size " << size;
	}
}



int main() {
	testing::InitGoogleTest();
	RUN_ALL_TESTS();
}