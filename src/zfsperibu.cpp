#include <cstdlib>
#include <iostream>

#include "snapshot.hpp"
#include "prune.hpp"

void prune_snapshots(const std::string& dataset) {
  auto now = std::chrono::system_clock::now();
  auto older_than_24h = [&now] (const timestamp& ts) {
    return ts.time_point() < (now - std::chrono::hours{24});
  };
  auto group_daily = [] (const timestamp& ts) {
    return ts.uint64() / 1000000;
  };
  prune_snapshots_(dataset, older_than_24h, group_daily);
  auto older_than_1m =  [&now] (const timestamp& ts) {
    return ts.time_point() < (now - std::chrono::hours{24 * 30});
  };
  auto group_monthly = [] (const timestamp& ts) {
    return ts.uint64() / 100000000;
  };
  prune_snapshots_(dataset, older_than_1m, group_monthly); 
}

void print_usage() {
  std::cout << "Usage:"
	    << "\tzfsperibu [snapshot|prune] [dataset]\n"
	    << "\tzfsperibu receive [port] [dataset]\n"
	    << "\tzfsperibu send [address] [port] [dataset]\n"
    ;
}

int main(int argc, char *argv[]) {
  if (argc >= 2) {
    if (strlen(argv[1]) > 0) {
      std::string command(argv[1]);
      if (command == "snapshot") {
	for (int i = 2; i < argc; ++i) {
	  create_snapshot(argv[i]);
	}
      } else if (command == "prune") {
	for (int i = 2; i < argc; ++i) {
	  prune_snapshots(argv[i]);
	}
      } else if (command == "receive") {
	if (argc != 4) {
	  std::cout << "Invalid receive invocation\n";
	  print_usage();
	  exit(11);
	}
	uint16_t port{0};
	try {
	  port = std::stoul(argv[2]);
	} catch (int) {
	  std::cout << "Could not parse port number " << argv[2] << "\n";
	  print_usage();
	  exit(10);
	}
	receive_snapshot(port, argv[3]);
      } else if (command == "send") {
	if (argc != 5) {
	  std::cout << "Invalid send invocation\n";
	  print_usage();
	  exit(12);
	}
	uint16_t port{0};
	try {
	  port = std::stoul(argv[3]);
	} catch (int) {
	  std::cout << "Could not parse port number " << argv[3] << "\n";
	  print_usage();
	  exit(10);
	}
	send_snapshot(argv[2], port, argv[4]);
      }
    } else {
      std::cout << "Invalid command of length " << strlen(argv[1]) << "\n";
      print_usage();
    }
  } else {
    print_usage();
  }
  return 0;
}

