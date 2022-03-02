#pragma once

#include "Skill.h"

#include <algorithm>
#include <iostream>
#include <queue>
#include <vector>

struct Contributor;


using Date = unsigned int;

struct Role
{
    SkillName skill;
    SkillLevel level;
    bool has_mentor = false;
};

struct Project
{
    std::string name;
    std::vector<Role> roles;
    Date best_before;
    unsigned int duration;
    unsigned int score;

    using Priority = double;
    Priority ComputePriority(Date current_date) const
    {
        unsigned int const delay_in_days = ((current_date + duration) > best_before) ? current_date + duration - best_before : 0;
        unsigned int const penality = std::min(score, delay_in_days);
        unsigned int const true_score = score - penality;
        return static_cast<double>(true_score) / duration / roles.size();
    }
};

struct UndertakenProject
{
    Project* project;
    std::vector<Contributor*> contributors;
    Date end_date;

    void Cancel()
    {
        // revert assginment to project
        for (Contributor* contrib : contributors)
            contrib->is_available = true;

        // reset project status
        for (Role& r : project->roles)
            r.has_mentor = false;

        contributors.clear();
    }

    friend constexpr bool operator>(UndertakenProject const& lhs, UndertakenProject const& rhs) noexcept
    {
        return lhs.end_date > rhs.end_date;
    }

    friend std::ostream& operator<<(std::ostream& os, UndertakenProject const& up)
    {
        os << up.project->name << '\n';

        auto const contrib_near_end = --up.contributors.end();
        for (auto contrib_itr = up.contributors.begin(); contrib_itr != contrib_near_end; ++contrib_itr)
            os << (*contrib_itr)->name << ' ';
        auto const& last = *up.contributors.back();
        return os << last.name << '\n';
    }
};

using RunningProjects = std::priority_queue<std::reference_wrapper<UndertakenProject>, std::vector<std::reference_wrapper<UndertakenProject>>, std::greater<UndertakenProject> >;

