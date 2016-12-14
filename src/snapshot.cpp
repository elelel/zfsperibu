#include "snapshot.hpp"

#include <regex>
#include <stdio.h>
#include <unistd.h>

using namespace std::chrono;

auto list_snapshots(const std::string& dataset) -> std::vector<timestamp> {
  std::vector<timestamp> rslt;
  FILE *pipe = popen("zfs list -t snapshot", "r");
  if (pipe) {
    const int buf_sz = 2048;
    char buf[buf_sz];
    while (fgets(buf, buf_sz, pipe) != nullptr) {
      buf[strlen(buf)-1] = 0x00;    // Remove newline char
      std::string s(buf);
      std::regex re(R"((.*?)@(\d+)\s.*)");
      std::smatch ms;
      if (std::regex_match(s, ms, re) && (ms.size() == 3)) {
	const auto& r_dataset = ms[1];
	const auto& r_timestamp = ms[2];
	if (r_dataset == dataset) {
	  rslt.push_back(timestamp::create(r_timestamp));
	}
      }
    }
    pclose(pipe);
  }
  return rslt;
}

auto create_snapshot(const std::string& dataset) -> timestamp {
  auto ts = timestamp::create(system_clock::now());
  pid_t fk = fork();
  if (fk == 0) { // in child
    std::string cmd = std::string("zfs snapshot -r ") + dataset + "@" + ts.string();
    system(cmd.c_str());
    exit(0);
  }
  return ts;
}
