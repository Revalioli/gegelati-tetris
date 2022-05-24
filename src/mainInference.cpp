#include <iostream>
#include <vector>

#include <gegelati.h>

#include "Tetris.h"
#include "Render.h"
#include "instructions.h"
#include <SFML/System.hpp>

int main(int argc, char *argv[]){

    std::string dotFilePath(argv[1]);
    std::cout << "Start Tetris TPG inference from dot file " << dotFilePath << std::endl;

    /* === Learning environment and agent setup === */

    // Loads the instruction set for the program
    Instructions::Set set;
    fillInstructionSet(set);

    // Loads parameters from params.json
    Learn::LearningParameters params;
    File::ParametersParser::loadParametersFromJson(ROOT_DIR "/params.json", params);

    // Instantiates the learning environment
    Tetris le;
    size_t seed = 90;
    le.reset(seed);

    // Loads graph from dot file
    Environment dotEnv(set, le.getDataSources(), params.nbRegisters, params.nbProgramConstant);
    TPG::TPGGraph dotGraph(dotEnv);
    File::TPGGraphDotImporter dot(dotFilePath.c_str(), dotEnv, dotGraph);
    dot.importGraph();

    // Prepares for inference
    TPG::TPGExecutionEngine tee(dotEnv);
    const TPG::TPGVertex* root(dotGraph.getRootVertices().back());

    // Loads Render and display setup
    int replaySpeed = 20;

    std::atomic<bool> exitProgram = true; // (set to false by other thread)
    std::atomic<bool> resetDisplay = false; // Not used here
    std::atomic<uint64_t> generation = 0;   // Not used here

    std::thread replayThread(playFromRoot, std::ref(exitProgram), std::ref(resetDisplay), &root,
                             std::ref(set), std::ref(le), std::ref(params), std::ref(generation), 2, replaySpeed);

    while(exitProgram); // replayThread will set exitProgram at false

    resetDisplay = true;

    replayThread.join();

}