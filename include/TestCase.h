#pragma once

#include "Contributor.h"
#include "Project.h"
#include "Skill.h"

#include <algorithm>
#include <cassert>
#include <iostream>
#include <unordered_set>
#include <vector>

class TestCase
{
    std::vector<Contributor> contributors;
    std::vector<Project> projects;
    std::vector<UndertakenProject> undertaken_projects;
    SkillMap skill_map;

    void Clear()
    {
        skill_map.clear();
        undertaken_projects.clear();
        contributors.clear();
        projects.clear();
    }

    friend std::istream& operator>>(std::istream& is, TestCase& tc)
    {
        // reset test case
        tc.Clear();

        unsigned int nb_contributors;
        unsigned int nb_projects;

        is >> nb_contributors >> nb_projects;

        // allocate memory
        tc.contributors.reserve(nb_contributors);
        tc.projects.reserve(nb_projects);

        // parse contributors
        std::generate_n(std::back_inserter(tc.contributors), nb_contributors, [&is]() -> Contributor
        {
            Contributor contributor;
            unsigned int nb_skills;
            is >> contributor.name >> nb_skills;

            contributor.skills.reserve(nb_skills);
            std::generate_n(std::inserter(contributor.skills, contributor.skills.begin()), nb_skills, [&is]() -> std::pair<SkillName, SkillLevel>
            {
                SkillName name;
                SkillLevel level;
                is >> name >> level;
                return { name, level };
            });

            return contributor;
        });

        // parse projects
        // extra: make sure the skill_map has an entry for every skill used in a project
        std::generate_n(std::back_inserter(tc.projects), nb_projects, [&is, &tc/*access to tc.skill_map*/]() -> Project
        {
            Project project;
            unsigned int nb_roles;
            is
                >> project.name
                >> project.duration
                >> project.score
                >> project.best_before
                >> nb_roles
                ;

            project.roles.reserve(nb_roles);
            std::generate_n(std::back_inserter(project.roles), nb_roles, [&is, &tc/*access to tc.skill_map*/]() -> Role
            {
                Role role;
                is >> role.skill >> role.level;
                static_cast<void>(tc.skill_map[role.skill]);    // ensure the skill_map has an entry for that skill name
                return role;
            });

            return project;
        });

        // setup extra structures
        for (auto& contributor : tc.contributors)
        {
            for (auto& [skill_name, distribution] : tc.skill_map)
            {
                if (auto const found = contributor.skills.find(skill_name); found != contributor.skills.end())
                    distribution[found->second].push_back(&contributor);
                else
                {
                    distribution[0].push_back(&contributor);
                    contributor.skills.emplace(skill_name, 0);
                }
            }
        }

        return is;
    }

    friend std::ostream& operator<<(std::ostream& os, TestCase const& tc)
    {
        os << tc.undertaken_projects.size() << '\n';
        for (auto const& project : tc.undertaken_projects)
            os << project;
        return os;
    }

    void OnProjectEnd(UndertakenProject& project);

public:
    void ComputeUndertakenProjects();
};

