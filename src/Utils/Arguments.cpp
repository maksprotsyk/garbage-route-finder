//
// Created by Максим Процик on 29.04.2023.
//

#include "Arguments.h"

Arguments* Arguments::m_arguments = nullptr;

Arguments &Arguments::GetArguments()
{
    if (m_arguments == nullptr)
    {
        m_arguments = new Arguments;
    }
    return *m_arguments;
}

void Arguments::ReadArguments(int argc, char **argv)
{
    po::options_description desc("Allowed options");
    desc.add_options()
            (InputFile, po::value<std::string>()->default_value("benchmarks/c50_t2_r1.json"),
             "problem input json")
            ("help", "produce help message")
            (BenchmarksFolder, po::value<std::string>()->default_value("benchmarks"),
             "Folder where benchmarks are stored")
            (OutputFile, po::value<std::string>()->default_value("output.json"),
             "Output json file")
            (BaseSolution, po::value<std::string>()->default_value("CLUSTER"),
             "Base solution used")
            (Heuristic, po::value<std::string>()->default_value("MOVE_IN_EXPORTATION"),
             "Heuristic used in the search")
            (SwapAnyToGenerate, po::value<size_t>()->default_value(10),
             "Number of containers for which SWAP_ANY will generate actions")
            (Search, po::value<std::string>()->default_value("GENETIC"),
             "Type of search performed after init solution")
            (Iterations, po::value<size_t>()->default_value(1000),
             "Iterations of search performed")
            (TempCoef, po::value<double>()->default_value(0.5),
             "Coefficient that affects speed of temperature decreasing")
            (Resets, po::value<size_t>()->default_value(4),
             "Resets in annealing search")
            (PopulationSize, po::value<size_t>()->default_value(20),
             "Number of solutions in population of genetic algorithm")
            (MutationProbability, po::value<double>()->default_value(0.3),
             "Mutation probability in genetic algorithm")
            (ClusterCoef, po::value<double>()->default_value(2.0),
             "Clustering coefficient if clusterization is used")
            (TabuQueueCoef, po::value<double>()->default_value(0.3),
             "Coefficient used for tabu queue size")
            (LogFile, po::value<std::string>()->default_value("log.txt"),
             "File used for logging information")
            (Config, po::value<std::string>()->default_value("config.json"),
             "File with trucks and containers configuration")
            (MapFile, po::value<std::string>()->default_value("data/Lviv/osrm/ukraine-latest.osrm"),
             "osrm files to use if finding real distances")
            (RealDistances, po::value<bool>()->default_value(false),
             "If true calculates distances and pathes using osrm")
            (Epsilon, po::value<double>()->default_value(0.01),
             "Min distance change to perform action")
            ;

    po::store(po::command_line_parser(argc, argv).options(desc).run(), m_variablesMap);
    po::notify(m_variablesMap);

}

