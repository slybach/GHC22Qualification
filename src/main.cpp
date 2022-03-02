#include "TestCase.h"

#include <fstream>

void SolveTestCase(std::string filename)
{
    TestCase tc;

    std::ifstream input(filename);
    if (!input)
        return; // for simple debug breakpoint

    input >> tc;
    tc.ComputeUndertakenProjects();

    std::ofstream output(filename + ".out.txt");
    output << tc;
}

int main()
{
	char const* const files[]
	{
		"a_an_example.in.txt",
		"b_better_start_small.in.txt",
		"c_collaboration.in.txt",
		"d_dense_schedule.in.txt",
		"e_exceptional_skills.in.txt",
		"f_find_great_mentors.in.txt",
	};

	for (auto file : files)
		SolveTestCase(file);

	return 0;
}
