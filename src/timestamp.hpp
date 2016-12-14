#pragma once

#include <chrono>
#include <ctime>
#include <string>

struct timestamp {
  timestamp(const timestamp& other);
  timestamp(timestamp&& other);
  void swap(timestamp& other);
  timestamp& operator=(const timestamp& other);
  
  static timestamp create(const std::string& s);
  static timestamp create(const std::time_t& tt);
  static timestamp create(const std::chrono::system_clock::time_point& tp);

  bool operator==(const timestamp& other) const;
  bool operator<=(const timestamp& other) const;
  bool operator<(const timestamp& other) const;

  int year() const;
  int month() const;
  int day() const;
  int hour() const;
  int min() const;
  int sec() const;

  std::time_t time_t() const;
  std::chrono::system_clock::time_point time_point() const;

  std::string string() const;
  uint64_t uint64() const;

  std::string dump() const;
private:
  timestamp(const tm& _tm);
  
  tm tm_{0};
};


