#include "timestamp.hpp"

#include <iomanip>
#include <sstream>
#include <iostream>

using namespace std::chrono;

timestamp::timestamp(const timestamp& other) :
  tm_(other.tm_) {
}

timestamp::timestamp(timestamp&& other) :
  tm_(std::move(other.tm_)) {
}

void timestamp::swap(timestamp& other) {
  std::swap(tm_, other.tm_);
}

timestamp& timestamp::operator=(const timestamp& other) {
  timestamp tmp(other);
  swap(tmp);
  return *this;
}

timestamp::timestamp(const tm& _tm) :
  tm_(_tm) {
}

timestamp timestamp::create(const std::string& s) {
  tm tm_;
  std::stringstream ss(s);
  ss >> std::get_time(&tm_, "%Y%m%d%H%M%S");
  if (ss.fail()) {
    std::stringstream ss(s);
    ss >> std::get_time(&tm_, "%Y-%m-%d_%H-%M-%S");
    if (ss.fail()) {
      std::stringstream ss(s);
      ss >> std::get_time(&tm_, "%Y-%m-%d %H-%M-%S");
      if (ss.fail()) {
        throw std::runtime_error("Failed to parse timestamp: " + s);
      }
    }
  }
  return timestamp(tm_);
}

timestamp timestamp::create(const std::time_t& tt) {
  tm tm_;
  tm_ = *gmtime(&tt);
  return timestamp(tm_);
}

timestamp timestamp::create(const std::chrono::system_clock::time_point& tp) {
  std::time_t tt = system_clock::to_time_t(tp);
  return timestamp::create(tt);
}

bool timestamp::operator==(const timestamp& other) const {
  return time_point() == other.time_point();
}

bool timestamp::operator<(const timestamp& other) const {
  return time_point() < other.time_point();
}

bool timestamp::operator<=(const timestamp& other) const {
  return (time_point() == other.time_point()) || (time_point() < other.time_point());
}

int timestamp::year() const {
  return tm_.tm_year + 1900;
}

int timestamp::month() const {
  return tm_.tm_mon + 1;
}

int timestamp::day() const {
  return tm_.tm_mday;
}

int timestamp::hour() const {
  return tm_.tm_hour;
}

int timestamp::min() const {
  return tm_.tm_min;
}
  
int timestamp::sec() const {
  return tm_.tm_sec;
}

std::time_t timestamp::time_t() const {
  struct tm * timeinfo;
  std::cout << "created timeinfo\n";
  std::cout << "This " << (void*)this << "\n";
  std::cout << dump() ;
  *timeinfo = this->tm_;
  std::cout << "copied timeinfo\n";
  std::time_t tt = std::mktime(timeinfo);
  std::cout << "make time \n";
  if (tt == -1) {
    std::cout << "Could not convert " + string() + " to time_t\n";
    throw std::runtime_error("Could not convert " + string() + " to time_t");
  }
  return tt;
}

system_clock::time_point timestamp::time_point() const {
  return system_clock::from_time_t(time_t());
}

std::string timestamp::string() const {
  std::stringstream ss;
  ss << std::put_time(&tm_, "%Y%m%d%H%M%S");
  return ss.str();
}

uint64_t timestamp::uint64() const {
  return uint64_t(sec()) +
    uint64_t(min() * 100) +
    uint64_t(hour()) * 100 * 100 +
    uint64_t(day()) * 100 * 100 * 100 +
    uint64_t(month()) * 100 * 100 * 100 * 100 +
    uint64_t(year()) * 100 * 100 * 100 * 100 * 100;
}

std::string timestamp::dump() const {
  std::stringstream ss;
  ss << "Year: " << year() << "\n"
     << "Month: " << month() << "\n"
     << "Day: " << day() << "\n"
     << "Hour: " << hour() << "\n"
     << "Min: " << min() << "\n"
     << "Sec: " << sec() << "\n"
     << "uint64: " << uint64() << "\n";
  return ss.str();
}
