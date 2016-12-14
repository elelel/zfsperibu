#pragma once

#include <map>
#include <stdio.h>
#include <unistd.h>

#include "timestamp.hpp"

template <typename key_t>
void leave_one_per_key(const std::string& dataset, const std::map<key_t, std::vector<timestamp>>& m) {
  for (const auto& p : m) {
    const auto& v = p.second;
    if (v.size() > 1) {
      bool need_delim = false;
      std::string cmd;
      for (size_t i = 1; i < v.size(); ++i) {
	if (need_delim) cmd += "; ";
	cmd += std::string("zfs destroy ") + dataset + "@" + v[i].string();
	need_delim = true;
      }
      pid_t fk = fork();
      if (fk == 0) {
	system(cmd.c_str());
	exit(0);
      }
    }
  }
}

template <typename filter_predicate_t, typename grouping_fn_t>
void prune_snapshots_(const std::string& dataset, filter_predicate_t pred, grouping_fn_t grouping_fn) {
  auto snaps = list_snapshots(dataset);
  std::vector<timestamp> filtered;
  for (const auto& s : snaps) 
    if (pred(s)) filtered.push_back(s);
  std::map<int, std::vector<timestamp> > grouped;
  
  for (const auto& s : filtered) {
    auto key = grouping_fn(s);
    grouped[key].push_back(s);
  }
  leave_one_per_key(dataset, grouped);
}
