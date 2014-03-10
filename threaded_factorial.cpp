#include <iostream>
#include <algorithm>
#include <unordered_map>
#include <vector>
#include <future>
#include <atomic>
#include <string>

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
  bool cached(int i, long long &fact) {
    bool locked = false, rsl = false;
    fact = 0;
    while (! _flag.compare_exchange_weak(locked, true)) locked = false;
    auto iter = _cache.find(i);
    if (iter != _cache.end()) { fact = iter->second; rsl = true; }
    _flag = false;
    return rsl;
  }

  void store(int i, long long fact) {
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

int main(int argc, char ** argv) {
  vector<int> numbers;
  std::generate_n(std::back_inserter(numbers), argc - 1,
                  [&argv](void)->int{static int i = 1; return std::stoi(argv[i++]); });

  cout << numbers << endl;

  return 0;
}
