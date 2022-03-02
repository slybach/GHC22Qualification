#include "TestCase.h"

namespace
{
    struct Candidate
    {
        Contributor* contributor;
        unsigned int nb_mentoring{ 0u };
        unsigned int nb_unused_skills{ 0u };

        friend constexpr bool operator<(Candidate const& lhs, Candidate const& rhs) noexcept
        {
            return lhs.nb_mentoring < rhs.nb_mentoring || (lhs.nb_mentoring == rhs.nb_mentoring && lhs.nb_unused_skills > rhs.nb_unused_skills);
        }
    };

    /** @return true if project was added */
    bool AssignContributorToProject(Project& project, SkillMap& skill_map, UndertakenProject& undertaken)
    {
        undertaken.project = &project;

        // check if there exists a candidate for each role
        undertaken.contributors.reserve(project.roles.size());
        auto const role_end = project.roles.cend();
        for (auto role_itr = project.roles.begin(); role_itr != role_end; ++role_itr)
        {
            SkillDistribution& skill_holders = skill_map[role_itr->skill];
            auto const start_level = role_itr->level - role_itr->has_mentor;

            std::vector<Candidate> candidates;

            for (auto level_itr = skill_holders.lower_bound(start_level), level_end = skill_holders.end(); level_itr != level_end; ++level_itr)
            {
                auto const& contributors_at_level = level_itr->second;
                for (Contributor* contributor : contributors_at_level)
                {
                    if (contributor->is_available)
                    {
                        Candidate candidate;
                        candidate.contributor = contributor;

                        std::unordered_set<SkillName> used_skills;
                        used_skills.emplace(role_itr->skill);

                        for (auto next_role_itr = role_itr + 1; next_role_itr != role_end; ++next_role_itr)
                        {
                            if (!next_role_itr->has_mentor)
                            {
                                auto const contrib_skill = contributor->skills.find(next_role_itr->skill);
                                if (contrib_skill != contributor->skills.end() && contrib_skill->second >= next_role_itr->level)
                                {
                                    ++candidate.nb_mentoring;
                                    used_skills.emplace(next_role_itr->skill);
                                }
                            }
                        }
                        candidate.nb_unused_skills = static_cast<unsigned int>(contributor->skills.size() - used_skills.size());
                        candidates.push_back(candidate);
                    }
                }
            }

            if (!candidates.empty())
            {
                auto candidate_itr = std::max_element(candidates.begin(), candidates.end());

                auto& contrib = *candidate_itr->contributor;
                contrib.is_available = false;
                undertaken.contributors.push_back(&contrib);

                // update has_mentor
                for (auto next_role_itr = role_itr + 1; next_role_itr != role_end; ++next_role_itr)
                {
                    auto const contrib_skill = contrib.skills.find(next_role_itr->skill);
                    if (contrib_skill != contrib.skills.end() && contrib_skill->second >= next_role_itr->level)
                        next_role_itr->has_mentor = true;
                }
            }
            else
            {
                undertaken.Cancel();
                return false;
            }
        }
        return  true;
    }

}

void TestCase::OnProjectEnd(UndertakenProject& undertaken)
{
    for (unsigned int role_idx = 0u; role_idx < undertaken.project->roles.size(); ++role_idx)
    {
        Role& role = undertaken.project->roles[role_idx];
        auto& le_mec = *undertaken.contributors[role_idx];

        auto skill_itr = le_mec.skills.find(role.skill);
        assert(skill_itr != undertaken.contributors[role_idx]->skills.end());
        assert(skill_itr->second + 1 >= role.level);
        if (skill_itr->second <= role.level)
        {
            SkillDistribution& distribution = skill_map[role.skill];
            auto& skill_level_holders = distribution[skill_itr->second];
            auto const found = std::find(skill_level_holders.begin(), skill_level_holders.end(), &le_mec);
            assert(found != skill_level_holders.end());
            std::swap(*found, skill_level_holders.back());
            skill_level_holders.pop_back();

            ++skill_itr->second;

            distribution[skill_itr->second].push_back(&le_mec);
        }

        le_mec.is_available = true;
    }
}


void TestCase::ComputeUndertakenProjects()
{
    // reset
    undertaken_projects.clear();
    undertaken_projects.reserve(projects.size());

    std::vector<std::reference_wrapper<Project> > pending_projects(projects.begin(), projects.end());
    Date current_day = 0;
    RunningProjects running_projects;

    while (true)
    {
        // sort projects in decreasing prio
        std::sort(pending_projects.begin(), pending_projects.end(), [current_day](Project const& lhs, Project const& rhs)
        {
            return lhs.ComputePriority(current_day) > rhs.ComputePriority(current_day);
        });


        // loop over projects to see what we can start
        auto proj_itr = pending_projects.begin();
        while (proj_itr != pending_projects.end())
        {
            if (UndertakenProject undertaken; AssignContributorToProject(*proj_itr, skill_map, undertaken))
            {
                undertaken.end_date = current_day + proj_itr->get().duration;
                undertaken_projects.push_back(std::move(undertaken));
                running_projects.push(undertaken_projects.back());
                proj_itr = pending_projects.erase(proj_itr);
            }
            else
                ++proj_itr;
        }

        // increment day to first project that finishes -> level up
        if (running_projects.empty())
            break;

        current_day = running_projects.top().get().end_date;
        while (!running_projects.empty() && running_projects.top().get().end_date == current_day)
        {
            UndertakenProject& p = running_projects.top();
            OnProjectEnd(p);
            running_projects.pop();
        }
    }
}
