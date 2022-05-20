#include <iostream>
#include <vector>

#include <gegelati.h>
#include <SFML/System/Vector2.hpp>

#include "Tetris.h"
#include "instructions.h"

int main(){

    std::cout << "Start Tetris learning application" << std::endl;

    /* === Learning environment and agent setup === */

    // Loads the instruction set for the program
    Instructions::Set set;
    fillInstructionSet(set);

    // Loads parameters from params.json
    Learn::LearningParameters params;
    File::ParametersParser::loadParametersFromJson(ROOT_DIR "/params.json", params);

    // Instantiates the learning environment
    Tetris le;

    std::cout << "Number of threads: " << params.nbThreads << std::endl;

    // Instantiate and init the learning agent
    Learn::ParallelLearningAgent la(le, set, params);
    la.init();

    // TODO No console control mode

    std::atomic<bool> exitProgram = false; // (set to false by other thread)


    /* === Log file setup === */

    // Basic logger
    Log::LABasicLogger basicLogger(la);

    // Create an exporter for all graphs
    File::TPGGraphDotExporter dotExporter("out_0000.dot", *la.getTPGGraph());

    // Logging best policy stat.
    std::ofstream stats;
    stats.open("bestPolicyStats.md");
    Log::LAPolicyStatsLogger policyStatsLogger(la, stats);

    // Export parameters before starting training.
    // These may differ from imported parameters because of LE or machine specific
    // settings such as thread count of number of actions.
    File::ParametersParser::writeParametersToJson("exported_params.json", params);

    /* === Training === */

    for(int i = 0; i < params.nbGenerations && !exitProgram; i++){

        // Change name for exported TPG dot file for current generation
        char buff[13];
        sprintf(buff, "out_%04d.dot", i);
        dotExporter.setNewFilePath(buff);
        dotExporter.print();

        la.trainOneGeneration(i);

        // TODO No console control mode

    }

    // Keep best policy
    la.keepBestPolicy();

    // Clear introns instructions
    la.getTPGGraph()->clearProgramIntrons();

    // Export the graph
    dotExporter.setNewFilePath("out_best.dot");
    dotExporter.print();

    // Export stats on the best policy
    TPG::PolicyStats ps;
    ps.setEnvironment(la.getTPGGraph()->getEnvironment());
    ps.analyzePolicy(la.getBestRoot().first);
    std::ofstream bestStats;
    bestStats.open("out_best_stats.md");
    bestStats << ps;
    bestStats.close();
    stats.close();

    // Cleanup instructions
    for (unsigned int i = 0; i < set.getNbInstructions(); i++) {
        delete (&set.getInstruction(i));
    }

    // TODO No console control mode

    return 0;
}