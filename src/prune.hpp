#pragma once

#include <algorithm>
#include <map>
#include <stdio.h>
#include <unistd.h>
#include <iostream>

#include "timestamp.hpp"
#include "snapshot.hpp"

template <typename key_t, typename snapshot_t>
void leave_one_per_key(const std::map<key_t, std::vector<snapshot_t>>& m) {
  for (const auto& p : m) {
    const auto& v = p.second;
    if (v.size() > 1) {
      bool need_delim = false;
      std::string cmd;
      std::vector<snapshot_t> sorted(v.begin(), v.end());
      std::sort(sorted.begin(), sorted.end());
      sorted.pop_back();
      for (const auto& s : sorted) {
	  s.destroy();
      }
    }
  }
}

template <typename snapshot_t, typename predicate_t, typename grouping_fn_t>
void prune_snapshots(const std::string& path, predicate_t pred, grouping_fn_t grouping_fn) {
  auto snaps = load_snapshots<snapshot_t>();
  snaps.erase(std::remove_if(snaps.begin(), snaps.end(), [&path] (const snapshot_t& s) {
	return s.path() != path;
      }));

  snaps.erase(std::remove_if(snaps.begin(), snaps.end(), [pred] (const snapshot_t& s) {
	return !pred(s);
      }));

  std::map<uint64_t, std::vector<snapshot_t> > grouped;

  for (const auto& s : snaps) {
    auto key = grouping_fn(s);
    grouped[key].push_back(s);
  }
  leave_one_per_key(grouped);
}

