#include <cstdlib>
#include <iostream>

#include "snapshot.hpp"
#include "prune.hpp"

void prune_snapshots(const std::string& dataset) {
  auto now = std::chrono::system_clock::now();
  auto older_than_24h = [&now] (const timestamp& ts) {
    std::cout << "filter " << ts.dump() << "\n";
    return ts.time_point() < (now - std::chrono::hours{24});
  };
  auto group_daily = [] (const timestamp& ts) {
    std::cout << ts.dump() << "\n";
    return ts.uint64() / 10000;
  };
  prune_snapshots_(dataset, older_than_24h, group_daily);
  /*  auto older_than_1m =  [&now] (const timestamp& ts) {
    return ts.time_point() < (now - std::chrono::hours{24 * 30});
  };
  auto group_monthly = [] (const timestamp& ts) {
    return ts.uint64() / 1000000;
  };
  prune_snapshots_(dataset, older_than_1m, group_monthly);  */
}

int main(int argc, char *argv[]) {
  for (int i = 1; i < argc; ++i) {
    std::string dataset(argv[i]);
    auto s = create_snapshot(dataset);
    prune_snapshots(dataset);
  }
  return 0;
}

