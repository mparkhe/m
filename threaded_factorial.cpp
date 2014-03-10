#include <iostream>
#include <algorithm>
#include <unordered_map>
#include <vector>
#include <future>
#include <atomic>
#include <string>
#include <random>

using std::cout;
using std::endl;
using std::unordered_map;
using std::vector;
using std::atomic;

class FactorialStore {
  unordered_map<int, long long> _cache;
  atomic<bool>                  _flag;
public:
  FactorialStore(void) : _cache({{1, 1}, {2, 2}}), _flag(false) {}
  bool get(int i, long long &fact) {
    bool locked = false, rsl = false;
    fact = 0;
    while (! _flag.compare_exchange_weak(locked, true)) locked = false;
    auto iter = _cache.find(i);
    if (iter != _cache.end()) { fact = iter->second; rsl = true; }
    _flag = false;
    return rsl;
  }

  void set(int i, long long fact) {
    bool locked = false;
    while (! _flag.compare_exchange_weak(locked, true)) locked = false;
    _cache.insert(std::make_pair(i, fact));
    _flag = false;
  }
};

template<typename IO, typename Type>
IO & operator << (IO & io, const vector<Type> & v) {
  bool first = true;
  for (const Type & t : v) {
    if (! first) io << " ";
    io << t;
    first = false;
  }
  return io;
}

long long factorial(int i, FactorialStore &cache) {
  long long rsl;
  if (cache.get(i, rsl)) return rsl;
  rsl = factorial(i - 1, cache) + factorial(i - 2, cache);
  cache.set(i, rsl);
  return rsl;
}

int main(int argc, char ** argv) {
  vector<int> numbers;
  int n = std::stoi(argv[1]);
  std::random_device dev;
  std::generate_n(std::back_inserter(numbers), n, [&dev]{return dev() % 100 + 1; });

  cout << numbers << endl;

  FactorialStore cache;
  for (auto n : numbers) { cout << n << "->" << factorial(n, cache) << endl; }

  return 0;
}
