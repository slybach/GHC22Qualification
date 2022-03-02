#pragma once

#include <map>
#include <string>
#include <unordered_map>
#include <vector>

struct Contributor;

using SkillName = std::string;
using SkillLevel = unsigned int;
using SkillDistribution = std::map<SkillLevel, std::vector<Contributor*>>;
using SkillMap = std::unordered_map<SkillName, SkillDistribution>;

