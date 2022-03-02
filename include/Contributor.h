#pragma once

#include "Skill.h"

#include <string>
#include <unordered_map>

struct Contributor
{
    std::string name;
    std::unordered_map<SkillName, SkillLevel> skills;
    bool is_available = true;
};

