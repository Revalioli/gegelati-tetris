#include <iostream>
#include <vector>

#include <gegelati.h>

#include "Tetris.h"
#include "Render.h"
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
//    Learn::LearningAgent la(le, set, params);
    la.init();

    const TPG::TPGVertex* bestRoot = nullptr;

    /* === Render environment for replays === */

#ifndef NO_REPLAY
    std::atomic<bool> exitProgram = true; // (set to false by other thread)
    std::atomic<bool> resetDisplay = false;
    std::atomic<uint64_t> generation = 0;

    int speedReplay = 30;   // in frames/seconds

    std::thread replayThread(playFromRoot, std::ref(exitProgram), std::ref(resetDisplay), &bestRoot,
                             std::ref(set), std::ref(le), std::ref(params), std::ref(generation), 0, speedReplay);

    while(exitProgram); // replayThread will set exitProgram at false
#else
    std::atomic<bool> exitProgram = false;
#endif
    /* === Log logFile setup === */

    // Basic logger for std::out
    Log::LABasicLogger basicLogger(la);

    // Basic logger to logFile
    std::shared_ptr<Log::LABasicLogger> fileLogger;
    std::ofstream logFile("log");
    if(logFile.is_open())
        fileLogger = std::make_shared<Log::LABasicLogger>(la, logFile);
    else
        std::cerr << "Can't save logs, logFile opening failed" << std::endl;

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

        // Change name for exported TPG dot logFile for current generation
        char buff[13];
        sprintf(buff, "out_%04d.dot", i);
        dotExporter.setNewFilePath(buff);
        dotExporter.print();

        la.trainOneGeneration(i);

        std::cout << "Best game score : " << le.getGameScoreRecord() << "   Average number of forbidden moves : " << le.getAverageForbiddenMoves() << std::endl;
        le.resetGlobalData();

#ifndef NO_REPLAY
        generation = i;
        if (!exitProgram){
            bestRoot = la.getBestRoot().first;
            resetDisplay = true;

            while(resetDisplay);    // We let the Render use le to create a replay
        }
#endif

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

    logFile.close();

#ifndef NO_REPLAY
    std::cout << "Training finished, press [escape] to close replay session." << std::endl;
    replayThread.join();
#endif

    return 0;
}