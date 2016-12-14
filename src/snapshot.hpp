#pragma once

#include <vector>

#include "timestamp.hpp"

auto list_snapshots(const std::string& dataset) -> std::vector<timestamp>;
auto create_snapshot(const std::string& dataset) -> timestamp;
