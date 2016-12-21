#include "snapshot.hpp"

#include <cstdlib>
#include <iostream>
#include <regex>
#include <stdio.h>
#include <unistd.h>

using namespace std::chrono;

auto snapshot_entries() -> std::vector<std::string> {
  std::vector<std::string> rslt;
  FILE *pipe = popen("zfs list -t snapshot", "r");
  if (pipe) {
    const int buf_sz = 2048;
    char buf[buf_sz];
    while (fgets(buf, buf_sz, pipe) != nullptr) {
      if (strlen(buf) > 1) {
	buf[strlen(buf)-1] = 0x00;    // Remove newline char
	rslt.push_back(std::string(buf));
      }
    }
    pclose(pipe);
  }
  return rslt;
}

void send(const remote_src_snapshot& snap, const std::string& ssh_cmd, const std::string& remote_path) {
  std::cout << "Sending initial\n";
  std::string cmd = "zfs send " + snap.name() + " | " + ssh_cmd + " " + remote_path;
  int rslt = system(cmd.c_str());
  if (rslt != 0)
    throw std::runtime_error("Failed to send snapshot with command " + cmd);
}

void send(const remote_src_snapshot& prev_snap, const remote_src_snapshot& snap, const std::string& ssh_cmd, const std::string& remote_path) {
  std::cout << "Sending incrementally\n";
  std::string cmd = "zfs send -i " + prev_snap.name() + " " + snap.name() + " | " + ssh_cmd + " " + remote_path;
  int rslt = system(cmd.c_str());
  if (rslt != 0)
    throw std::runtime_error("Failed to send snapshot with command " + cmd);
}

remote_src_snapshot last_remote_snapshot(const std::string& path) {
  std::cout << "Searching for last remote snapshot for " << path << "\n";
  auto snaps = load_snapshots<remote_src_snapshot>();
  if (snaps.size() > 0) {
    snaps.erase(std::remove_if(snaps.begin(), snaps.end(), [&path] (const remote_src_snapshot& s) {
	  return s.path() != path;
	}), snaps.end());
    std::sort(snaps.begin(), snaps.end(), [] (const remote_src_snapshot& l,
					      const remote_src_snapshot& r) {
		return l.ts() < r.ts();
	      });
    return snaps[snaps.size() - 1];
  } else {
    throw std::runtime_error("No existing remote src snapshots for " + path);
  }
}

    
