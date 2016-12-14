#include <cstdlib>

#include "snapshot.hpp"
#include "prune.hpp"

void prune_snapshots(const std::string& dataset) {
  auto now = std::chrono::system_clock::now();
  auto filter_24h = [&now] (const timestamp& ts) {
    return ts.time_point() < (now - std::chrono::hours{24});
  };
  auto group_daily = [] (const timestamp& ts) {
    return ts.time_t() / (60 * 60 * 24);
  };
  prune_snapshots_(dataset, filter_24h, group_daily);
  auto filter_1y =  [&now] (const timestamp& ts) {
    return ts.time_point() < (now - std::chrono::hours{24 * 30 * 12});
  };
  auto group_monthly = [] (const timestamp& ts) {
    return ts.time_t() / (60 * 60 * 24 * 30);
  };
  prune_snapshots_(dataset, filter_1y, group_monthly);
}

int main(int argc, char *argv[]) {
  for (int i = 1; i < argc; ++i) {
    std::string dataset(argv[i]);
    create_snapshot(dataset);
    prune_snapshots(dataset);
  }
  return 0;
}

