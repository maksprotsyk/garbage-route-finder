#include <iostream>
#include <vector>

#include "Arguments.h"
#include "Problem.h"
#include "MoveInExportation.h"
#include "TabuSearch.h"
#include "ClusterSolution.h"
#include "GreedySolution.h"
#include "AnnealingSearch.h"
#include "GeneticSearch.h"
#include "MultipleSelector.h"
#include "ProbabilisticSelector.h"
#include "RemoveLoops.h"
#include "SwapAny.h"
#include "SwapInExportation.h"
#include "SwapNeighbours.h"
#include "TimeDependenceSelector.h"
#include "FixedOrderSelector.h"


int main(int argc, char **argv)
{
    Arguments::GetArguments().ReadArguments(argc, argv);
    auto& problem = Problem::GetProblem();
    auto& arguments = Arguments::GetArguments();

    std::unordered_map<std::string, std::shared_ptr<BaseSolution>> baseSolutions;
    baseSolutions["SIMPLE_GREEDY"] = std::make_shared<GreedySolution>();

    auto cluster = std::make_shared<ClusterSolution>();
    cluster->clusterCoef = arguments.GetValue<double>(Arguments::ClusterCoef);
    baseSolutions["CLUSTER"] = cluster;

    std::unordered_map<std::string, std::shared_ptr<Search>> searches;

    auto tabu = std::make_shared<TabuSearch>();
    tabu->iterations = arguments.GetValue<size_t>(Arguments::Iterations);
    tabu->queueCoef = arguments.GetValue<double>(Arguments::TabuQueueCoef);
    tabu->threadsNum = 4;
    searches["TABU"] = tabu;

    auto no_search = std::make_shared<TabuSearch>();
    no_search->iterations = 0;
    no_search->queueCoef = 0;
    searches["NO_SEARCH"] = no_search;

    auto annealing = std::make_shared<AnnealingSearch>();
    annealing->iterations = arguments.GetValue<size_t>(Arguments::Iterations);
    annealing->resets = arguments.GetValue<size_t>(Arguments::Resets);
    annealing->tempCoef = arguments.GetValue<double>(Arguments::TempCoef);

    searches["ANNEALING"] = annealing;

    auto geneticSearch = std::make_shared<GeneticSearch>();
    geneticSearch->iterations = arguments.GetValue<size_t>(Arguments::Iterations);
    geneticSearch->populationSize = arguments.GetValue<size_t>(Arguments::PopulationSize);
    geneticSearch->mutationProbability = arguments.GetValue<double>(Arguments::MutationProbability);
    geneticSearch->threadsNum = 4;
    searches["GENETIC"] = geneticSearch;

    std::unordered_map<std::string, std::shared_ptr<Heuristic>> heuristics;

    heuristics["SWAP_IN_EXPORTATION"] = std::make_shared<SwapInExportation>();
    heuristics["SWAP_NEIGHBOURS"] =  std::make_shared<SwapNeighbours>();
    auto swapAny = std::make_shared<SwapAny>();
    swapAny->to_generate = arguments.GetValue<size_t>(Arguments::SwapAnyToGenerate);
    heuristics["SWAP_ANY"] = swapAny;
    heuristics["MOVE_IN_EXPORTATION"] = std::make_shared<MoveInExportation>();
    heuristics["REMOVE_LOOPS"] = std::make_shared<RemoveLoops>();

    heuristics["ANY_EXPORTATION_PROB2"] = std::make_shared<ProbabilisticSelector>(
            ProbabilisticSelector(
                    {heuristics["SWAP_ANY"], heuristics["SWAP_IN_EXPORTATION"]},
                    {0.75, 0.25}
            )
    );

    heuristics["ANY_EXPORTATION_PROB1"] = std::make_shared<ProbabilisticSelector>(
            ProbabilisticSelector(
                {heuristics["SWAP_ANY"], heuristics["SWAP_IN_EXPORTATION"]},
                {0.5, 0.5}
            )
    );

    heuristics["ANY_EXPORTATION_FIXED1"] =  std::make_shared<FixedOrderSelector>(
            FixedOrderSelector(
                    {heuristics["SWAP_ANY"], heuristics["SWAP_IN_EXPORTATION"]},
                    {1, 1}
                    )
    );

    heuristics["ANY_EXPORTATION_FIXED2"] = std::make_shared<FixedOrderSelector>(
            FixedOrderSelector(
                {heuristics["SWAP_ANY"], heuristics["SWAP_IN_EXPORTATION"]},
                {5, 3}
            )
    );

    heuristics["ANY_MOVE_EXPORTATION_FIXED2"] = std::make_shared<FixedOrderSelector>(
            FixedOrderSelector(
                {heuristics["SWAP_ANY"], heuristics["MOVE_IN_EXPORTATION"]},
                {5, 3}
            )
    );

    heuristics["ANY_EXPORTATION_TIME1"] = std::make_shared<TimeDependentSelector>(
            TimeDependentSelector(
                {heuristics["SWAP_ANY"],heuristics["SWAP_IN_EXPORTATION"]},
                {arguments.GetValue<size_t>(Arguments::Iterations) / 2}
            )
    );


    heuristics["ANY_EXPORTATION_TIME2"] = std::make_shared<TimeDependentSelector>(
            TimeDependentSelector(
                {heuristics["SWAP_ANY"], heuristics["SWAP_IN_EXPORTATION"]},
                {arguments.GetValue<size_t>(Arguments::Iterations) * 2 / 3}
            )
    );


    heuristics["ANY_EXPORTATION"] = std::make_shared<MultipleSelector>(
            MultipleSelector({
                heuristics["SWAP_IN_EXPORTATION"], heuristics["SWAP_ANY"]
            })
    );

    heuristics["ANY_MOVE_EXPORTATION"] = std::make_shared<MultipleSelector>(
            MultipleSelector({
                heuristics["MOVE_IN_EXPORTATION"], heuristics["SWAP_ANY"]
            })
    );

    heuristics["ANY_MOVE_SWAP"] = std::make_shared<FixedOrderSelector>(
            FixedOrderSelector(
                {
                    heuristics["SWAP_ANY"],
                    std::make_shared<ProbabilisticSelector>(
                        ProbabilisticSelector(
                            {heuristics["SWAP_IN_EXPORTATION"],
                            heuristics["MOVE_IN_EXPORTATION"]},
                            {0.5, 0.5}
                        ))
                },
                {5, 3}
            )
    );

    heuristics["REMOVE_LOOPS_SWAP_EXPORTATION"] = std::make_shared<MultipleSelector>(
            MultipleSelector(
                {heuristics["REMOVE_LOOPS"], heuristics["SWAP_IN_EXPORTATION"]}
            )
    );

    heuristics["REMOVE_LOOPS_SWAP_ANY_EXPORTATION"] = std::make_shared<ProbabilisticSelector>(
            ProbabilisticSelector(
                {
                    std::make_shared<MultipleSelector>(
                            MultipleSelector({heuristics["REMOVE_LOOPS"], heuristics["SWAP_IN_EXPORTATION"]}
                            )), heuristics["SWAP_ANY"]
                            },
                {0.6, 0.4}
            )
    );

    heuristics["REMOVE_LOOPS_MOVE_EXPORTATION"] = std::make_shared<MultipleSelector>(
            MultipleSelector(
                    {heuristics["REMOVE_LOOPS"], heuristics["MOVE_IN_EXPORTATION"]}
            )
    );

    heuristics["MOVE_SWAP_EXPORTATION"] = std::make_shared<FixedOrderSelector>(
            FixedOrderSelector(
                    {heuristics["SWAP_IN_EXPORTATION"], heuristics["MOVE_IN_EXPORTATION"]},
                    {15, 20}
            )
    );


    problem.ReadConfig(arguments.GetValue<std::string>(Arguments::Config));
    problem.ReadProblem(arguments.GetValue<std::string>(Arguments::InputFile));
    auto baseSolution = baseSolutions[arguments.GetValue<std::string>(Arguments::BaseSolution)];
    auto search = searches[arguments.GetValue<std::string>(Arguments::Search)];
    auto heuristic = heuristics[arguments.GetValue<std::string>(Arguments::Heuristic)];
    search->heuristic = heuristic;

    Solution sol = baseSolution->CreateSolution();
    std::cout << sol.CalculateTotalDistance() << std::endl;
    auto newSol = search->search(sol);
    std::cout << newSol.CalculateTotalDistance() << std::endl;

    std::ofstream outputFile(arguments.GetValue<std::string>(Arguments::OutputFile));
    nlohmann::json solJson = newSol.CreateJson();
    outputFile << std::setw(4) << solJson;

    return 0;
}
