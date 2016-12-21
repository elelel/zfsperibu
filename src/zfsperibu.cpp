#include <cstdlib>
#include <iostream>

#include "snapshot.hpp"
#include "prune.hpp"

void prune_local(const std::string& path) {
  auto now = std::chrono::system_clock::now();
  auto older_than_24h = [&now] (const local_snapshot& s) {
    return s.ts().time_point() < (now - std::chrono::hours{24});
  };
  auto group_daily = [] (const local_snapshot& s) {
    return s.ts().uint64() / 1000000;
  };
  prune_snapshots<local_snapshot>(path, older_than_24h, group_daily);
  auto older_than_1m =  [&now] (const local_snapshot& s) {
    return s.ts().time_point() < (now - std::chrono::hours{24 * 30});
  };
  auto group_monthly = [] (const local_snapshot& s) {
    return s.ts().uint64() / 100000000;
  };
  prune_snapshots<local_snapshot>(path, older_than_1m, group_monthly); 
}

void print_usage() {
  std::cout << "Usage:"
	    << "\tzfsperibu create path [path...]\n"
	    << "\tzfsperibu send-latest path ssh_command remote_path\n"
	    << "\tzfsperibu prune-local path [path...]\n"
    ;
}

int main(int argc, char *argv[]) {
  if (argc >= 2) {
    if (strlen(argv[1]) > 0) {
      std::string command(argv[1]);
      if (command == "create") {
	for (int i = 2; i < argc; ++i) {
	  auto ts = timestamp::create(std::chrono::system_clock::now());
	  std::cout << "Creating local snapshot with timestamp " << ts.string() << "\n";
	  local_snapshot snap(argv[i], ts.string());
	  snap.create();
	}
      } else if (command == "prune-local") {
	for (int i = 2; i < argc; ++i) {
	  std::cout << "Pruning local " << argv[i] << "\n";
	  prune_local(argv[i]);
	}
      } else if (command == "send-latest") {
	if (argc != 5) {
	  std::cout << "Invalid send-latest invocation\n";
	  print_usage();
	  exit(13);
	}
	auto ts = timestamp::create(std::chrono::system_clock::now());
	const auto& path = argv[2];
	const auto& ssh_cmd = argv[3];
	const auto& remote_path = argv[4];
	remote_src_snapshot snap(path, ts.string());
	try {
	  auto prev_snap = last_remote_snapshot(path);
	  std::cout << "Found previous snapshot " << prev_snap.name() << "\n";
	  snap.create();
	  try {
	    send(prev_snap, snap, ssh_cmd, remote_path);
	  } catch (std::runtime_error) {
	    std::cout << "Send failed, destroying stale remote snapshot\n";
	    snap.destroy();
	    exit(21);
	  }
	} catch (std::runtime_error) {
	  std::cout << "No previous backup found, sending initial snapshot\n";
	  // No incremental backup yet
	  snap.create();
	  try {
	    send(snap, ssh_cmd, remote_path);
	  } catch (std::runtime_error) {
	    std::cout << "Send failed, destroying stale remote snapshot\n";
	    snap.destroy();
	    exit(22);
	  }
	}
	auto snaps = load_snapshots<remote_src_snapshot>();
	for (const auto& s : snaps)
	  if (s.name() != snap.name()) {
	    s.destroy();
	  }
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

