#pragma once

#include <vector>

#include "timestamp.hpp"

auto list_snapshots(const std::string& dataset) -> std::vector<timestamp>;
auto create_snapshot(const std::string& dataset) -> timestamp;
auto receive_snapshot(const uint16_t& port, const std::string& pool, const std::string& dataset) -> timestamp;
void send_snapshot(const std::string& address, const uint16_t& port, const std::string& pool, const std::string& dataset);

