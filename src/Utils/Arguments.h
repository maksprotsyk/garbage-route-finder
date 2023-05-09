//
// Created by Максим Процик on 29.04.2023.
//

#ifndef ROUTING_PROBLEM_ARGUMENTS_H
#define ROUTING_PROBLEM_ARGUMENTS_H

#include <boost/program_options.hpp>

namespace po=boost::program_options;

class Arguments
{

public:

    static constexpr char const* InputFile = "input";
    static constexpr char const* BenchmarksFolder = "benchmarks";
    static constexpr char const* OutputFile = "output";
    static constexpr char const* BaseSolution = "base_solution";
    static constexpr char const* Heuristic = "heuristic";
    static constexpr char const* Search = "search";
    static constexpr char const* Iterations = "iterations";
    static constexpr char const* TempCoef = "temp_coef";
    static constexpr char const* Resets = "resets";
    static constexpr char const* PopulationSize = "population_size";
    static constexpr char const* MutationProbability = "mutation_probability";
    static constexpr char const* ClusterCoef = "cluster_coef";
    static constexpr char const* TabuQueueCoef = "tabu_queue_coef";
    static constexpr char const* LogFile = "log_file";
    static constexpr char const* Config = "config";
    static constexpr char const* MapFile = "map_file";
    static constexpr char const* RealDistances = "real";
    static constexpr char const* GenesCount = "genes_count";
    static constexpr char const* SwapAnyToGenerate = "swap_any_to_generate";
    static constexpr char const* Epsilon = "epsilon";

    static Arguments& GetArguments();
    void ReadArguments(int argc, char **argv);

    template<class T>
    T GetValue(const char* argumentName);
private:
   static Arguments* m_arguments;
   Arguments() = default;
   po::variables_map m_variablesMap;
};

template<class T>
T Arguments::GetValue(const char *argumentName)
{
    return m_variablesMap[argumentName].as<T>();
}


#endif //ROUTING_PROBLEM_ARGUMENTS_H
