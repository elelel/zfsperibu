#pragma once

#include <cstdlib>
#include <iostream>
#include <vector>
#include <regex>

#include "timestamp.hpp"

auto snapshot_entries() -> std::vector<std::string>;

template <typename derived_t>
struct snapshot {
  snapshot(const std::string& local_path, const std::string& snapname_date) :
    local_path_(local_path),
    snapname_date_(snapname_date) {
  }

  snapshot(const snapshot& other) :
    local_path_(other.local_path_),
    snapname_date_(other.snapname_date_) {
  }

  snapshot(snapshot&& other) :
    local_path_(std::move(other.local_path_)),
    snapname_date_(std::move(other.snapname_date_)) {
  }

  void swap(snapshot& other) {
    std::swap(local_path_, other.local_path_);
    std::swap(snapname_date_, other.snapname_date_);
  }

  snapshot& operator=(const snapshot& other) {
    snapshot tmp(other);
    swap(tmp);
    return *this;
  }
									      
  std::string name() const {
    return static_cast<const derived_t*>(this)->path() + "@" + static_cast<const derived_t*>(this)->snapname();
  }

  bool operator<(const snapshot& other) const {
    return name() < other.name();
  }

  void create() const {
    std::string cmd = std::string("zfs snapshot -r ") + name();
    int rslt = system(cmd.c_str());
    if (rslt != 0)
      throw std::runtime_error("Failed to create snapshot with command " + cmd);
    std::cout << "Created snapshot " << name() << "\n";
  }

  void destroy() const {
    std::string cmd = std::string("zfs destroy ") + name();
    int rslt = system(cmd.c_str());
    if (rslt != 0)
      throw std::runtime_error("Failed to destroy snapshot with command " + cmd);
    std::cout << "Destroyed snapshot " << name() << "\n";
  }

  const std::string& date() const {
    return snapname_date_;
  }

  timestamp ts() const {
    return timestamp::create(snapname_date_);
  }
  
protected:
  std::string local_path_;
  std::string snapname_date_;
};

struct local_snapshot : public snapshot<local_snapshot> {
  local_snapshot(const std::string& local_path, const std::string& snapname_date) :
    snapshot<local_snapshot>::snapshot(local_path, snapname_date) {
  }

  local_snapshot(const local_snapshot& other) :
    snapshot<local_snapshot>::snapshot(other) {
  }

  local_snapshot(local_snapshot&& other) :
    snapshot<local_snapshot>::snapshot(std::move(other)) {
  }

  void swap(local_snapshot& other) {
    snapshot<local_snapshot>::swap(other);
  }

  local_snapshot& operator=(const local_snapshot& other) {
    local_snapshot tmp(other);
    swap(tmp);
    return *this;
  }

  static local_snapshot from_name(const std::string& name) {
    std::regex re(R"((.*?)@local_(\d+)\s.*)");
    std::smatch ms;
    if (std::regex_match(name, ms, re) && (ms.size() == 3)) {
      return local_snapshot(ms[1], ms[2]);
    } else {
      throw std::runtime_error("Bad local_snapshot entry string");
    }
  }
  
  std::string path() const {
    return local_path_;
  }

  std::string snapname() const {
    return "local_" + snapname_date_;
  }

};

struct remote_src_snapshot : public snapshot<remote_src_snapshot> {
  remote_src_snapshot(const std::string& local_path, const std::string& snapname_date) :
    snapshot<remote_src_snapshot>::snapshot(local_path, snapname_date) {
  }

  remote_src_snapshot(const remote_src_snapshot& other) :
    snapshot<remote_src_snapshot>::snapshot(other) {
  }

  remote_src_snapshot(remote_src_snapshot&& other) :
    snapshot<remote_src_snapshot>::snapshot(std::move(other)) {
  }

  void swap(remote_src_snapshot& other) {
    snapshot<remote_src_snapshot>::swap(other);
  }

  remote_src_snapshot& operator=(const remote_src_snapshot& other) {
    remote_src_snapshot tmp(other);
    swap(tmp);
    return *this;
  }

  static remote_src_snapshot from_name(const std::string& name) {
    std::regex re(R"((.*?)@remote_(\d+)\s.*)");
    std::smatch ms;
    if (std::regex_match(name, ms, re) && (ms.size() == 3)) {
      return remote_src_snapshot(ms[1], ms[2]);
    } else {
      throw std::runtime_error("Bad remote_src_snapshot entry string");
    }
  }
  
  std::string path() const {
    return local_path_;
  }

  std::string snapname() const {
    return "remote_" + snapname_date_;
  }

};

struct remote_dst_snapshot : public snapshot<remote_dst_snapshot> {
  remote_dst_snapshot(const std::string& local_path,
		 const std::string& src_server_name,
		 const std::string& src_server_path,
		 const std::string& snapname_date) :
    snapshot<remote_dst_snapshot>::snapshot(local_path, snapname_date),
    src_server_name_(src_server_name),
    src_server_path_(src_server_path) {
    while ((src_server_path_.size() > 0) && (src_server_path_[0] == '/'))
      src_server_path_ = src_server_path_.substr(1, src_server_path_.length());
  }

  remote_dst_snapshot(const remote_dst_snapshot& other) :
    snapshot<remote_dst_snapshot>::snapshot(other),
    src_server_name_(other.src_server_name_),
    src_server_path_(other.src_server_path_) {
  }

  remote_dst_snapshot(remote_dst_snapshot&& other) :
    snapshot<remote_dst_snapshot>::snapshot(std::move(other)),
    src_server_name_(other.src_server_name_),
    src_server_path_(other.src_server_path_) {
  }

  void swap(remote_dst_snapshot& other) {
    snapshot<remote_dst_snapshot>::swap(other);
    std::swap(src_server_name_, other.src_server_name_);
    std::swap(src_server_path_, other.src_server_path_);
  }

  remote_dst_snapshot& operator=(const remote_dst_snapshot& other) {
    remote_dst_snapshot tmp(other);
    swap(tmp);
    return *this;
  }

  static remote_dst_snapshot from_name(const std::string& name) {
    std::regex re(R"(^(.*?)/(.*?)_(.*)@remote_(\d+)\s.*)");
    std::smatch ms;
    if (std::regex_match(name, ms, re) && (ms.size() == 5)) {
      return remote_dst_snapshot(ms[1], ms[2], ms[3], ms[4]);
    } else {
      throw std::runtime_error("Bad remote_dst_snapshot entry string");
    }
  }

  std::string path() const {
    return local_path_ + "/" + src_server_name_ + "_" + src_server_path_;
  }

  std::string snapname() const {
    return "remote_" + snapname_date_;
  }
  
private:
  std::string src_server_name_;
  std::string src_server_path_;
};

template <typename snapshot_t>
std::vector<snapshot_t> load_snapshots() {
  std::vector<snapshot_t> rslt;
  auto entries = snapshot_entries();
  for (const auto s : entries) {
    try {
      std::cout << "Trying snapshot constructor for name '" << s << "'\n";
      rslt.push_back(snapshot_t::from_name(s));
    } catch (std::runtime_error) {
      std::cout << "Not constructed\n";
    }
  }
  std::sort(rslt.begin(), rslt.end());
  return rslt;
}

void send(const remote_src_snapshot& s, const std::string& ssh_cmd, const std::string& remote_path);
void send(const remote_src_snapshot& prev_snap, const remote_src_snapshot& new_snap, const std::string& ssh_cmd, const std::string& remote_path);
remote_src_snapshot last_remote_snapshot(const std::string& path);
