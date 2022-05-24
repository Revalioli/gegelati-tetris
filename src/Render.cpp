#include "Render.h"
#include "Tetris.h"

#include <iostream>
#include <stdexcept>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

Render::Render(Tetris &t, int tileSize) : gameEnvironment(t), tileSize(tileSize), isInitialised(false), window(nullptr) {

    this->tile_green = sf::RectangleShape(sf::Vector2f(tileSize, tileSize));
    this->tile_red = sf::RectangleShape(sf::Vector2f(tileSize, tileSize));
    this->tile_orange = sf::RectangleShape(sf::Vector2f(tileSize, tileSize));
    this->tile_blue = sf::RectangleShape(sf::Vector2f(tileSize, tileSize));
    this->tile_yellow = sf::RectangleShape(sf::Vector2f(tileSize, tileSize));
    this->tile_cyan = sf::RectangleShape(sf::Vector2f(tileSize, tileSize));
    this->tile_magenta = sf::RectangleShape(sf::Vector2f(tileSize, tileSize));

    this->tile_green.setFillColor(sf::Color::Green);
    this->tile_red.setFillColor(sf::Color::Red);
    this->tile_orange.setFillColor(sf::Color(255, 153, 51));
    this->tile_blue.setFillColor(sf::Color::Blue);
    this->tile_yellow.setFillColor(sf::Color::Yellow);
    this->tile_cyan.setFillColor(sf::Color::Cyan);
    this->tile_magenta.setFillColor(sf::Color::Magenta);

    this->allTiles[0] = &tile_cyan;
    this->allTiles[1] = &tile_green;
    this->allTiles[2] = &tile_red;
    this->allTiles[3] = &tile_magenta;
    this->allTiles[4] = &tile_orange;
    this->allTiles[5] = &tile_blue;
    this->allTiles[6] = &tile_yellow;

    this->gridEdges = sf::RectangleShape(
            sf::Vector2f(tileSize * gameEnvironment.WIDTH, tileSize * gameEnvironment.HEIGHT));
    this->gridEdges.setFillColor(sf::Color::Transparent);
    this->gridEdges.setOutlineThickness(-1);
    this->gridEdges.setOutlineColor(sf::Color::White);
    this->gridEdges.setPosition(0, 0);

    if (!font.loadFromFile(ROOT_DIR "/dat/hemi.ttf"))
        throw std::runtime_error("Error loading font file.");

    this->scoreTextLabel.setFont(this->font);
    this->scoreTextLabel.setString("Cleared:");
    this->scoreTextLabel.setCharacterSize(24);
    this->scoreTextLabel.setFillColor(sf::Color::White);

    this->scoreLabel.setFont(font);
    this->scoreLabel.setCharacterSize(24);
    this->scoreLabel.setFillColor(sf::Color::White);

}

void Render::initialise() {
    window = new sf::RenderWindow(sf::VideoMode(this->tileSize * this->gameEnvironment.WIDTH + 300,
                                                this->tileSize * this->gameEnvironment.HEIGHT),
                                                "Tetris LE view");
}

void Render::update(bool display) {
    this->scoreLabel.setString(std::to_string(this->gameEnvironment.getGameScore()));

    this->window->clear();

    for(int i = 0; i < Tetris::HEIGHT; i++){
        for(int j = 0; j < Tetris::WIDTH; j++){

            int tile = (int)this->gameEnvironment.getTileAt(j, i);

            if(tile != 0){
                sf::RectangleShape& tileToDraw = *this->allTiles[tile - 1];

                tileToDraw.setPosition(j * this->tileSize, i * this->tileSize);
                window->draw(tileToDraw);
            }

        }
    }

    this->scoreTextLabel.setPosition(this->tileSize * Tetris::WIDTH + 20, 50);
    this->window->draw(this->scoreTextLabel);

    this->scoreLabel.setPosition(this->tileSize * Tetris::WIDTH + 55, 80);
    this->scoreLabel.setString(std::to_string(this->gameEnvironment.getGameScore()));
    this->window->draw(this->scoreLabel);

    this->window->draw(this->gridEdges);

    if(display)
        this->window->display();
}

void Render::close() {
    this->window->close();
    delete this->window;
    this->window = nullptr;
}



Render::~Render() {
    delete window;
}



void playFromRoot(std::atomic<bool>& exit, std::atomic<bool>& resetDisplay, const TPG::TPGVertex** bestRoot,
                  const Instructions::Set& set, Tetris& tetrisLE, const Learn::LearningParameters& params,
                  std::atomic<uint64_t>& generation, int seed, int replaySpeed){

    /* Replay display */
    Tetris simuEnv(tetrisLE);   // Tetris environment used for replay, is a deep copy of tetrisLE
    Render replayRender(simuEnv);
    replayRender.initialise();

    /* Replay computation */
    std::vector<uint64_t> replay;
    Environment env(set, simuEnv.getDataSources(), params.nbRegisters, params.nbProgramConstant);
    TPG::TPGExecutionEngine tee(env);
    uint64_t frame = 0;

    /* Replay control */
    bool waitEndOfReplay = false;   // True if the current replay must end before playing the next one
    float sleepTime = 1.f/(float)replaySpeed;

    std::cout << "---- Replay control ----" << std::endl
                << "[W] : Toggle wait end of replay" << std::endl
                << "[S] : Stop current replay" << std::endl;

    /* Render additional inforamtions */
    sf::Text generationLabel("Generation : ", replayRender.font, 24);
    generationLabel.setPosition(replayRender.tileSize * Tetris::WIDTH + 10, 110);
    sf::Text generationNumberLabel("0", replayRender.font, 24);
    generationNumberLabel.setPosition(replayRender.tileSize * Tetris::WIDTH + 170, 110);
    sf::Text frameLabel("Frame :", replayRender.font, 24);
    frameLabel.setPosition(replayRender.tileSize * Tetris::WIDTH + 10, 140);
    sf::Text frameNumberLabel("0", replayRender.font, 24);
    frameNumberLabel.setPosition(replayRender.tileSize * Tetris::WIDTH + 110, 140);
    sf::Text moveLabel;
    moveLabel.setFont(replayRender.font);
    moveLabel.setCharacterSize(24);


    bool isDisplay = false;
    sf::Event event;
    exit = false;

    while(!exit){

        if( resetDisplay && ( !waitEndOfReplay || (waitEndOfReplay && !isDisplay) )){
            // Restarting tetris environments
            tetrisLE.reset(seed, Learn::LearningMode::VALIDATION);
            simuEnv.reset(seed, Learn::LearningMode::VALIDATION);

            replay.clear();

            // Computing replay
            for(int i = 0; i < params.maxNbActionsPerEval && !tetrisLE.isTerminal(); i++){
                auto vertexList = tee.executeFromRoot(**bestRoot);
                const auto actionID = ((const TPG::TPGAction*)vertexList.back())->getActionID();
                replay.push_back(actionID);
                tetrisLE.doAction(actionID);
            }

            generationNumberLabel.setString(std::to_string(generation));

            isDisplay = true;
            frame = 0;

            resetDisplay = false;
        }

        if(isDisplay){
            // Playing one game frame
            simuEnv.doAction(replay[frame]);
            replayRender.update(false);

            // Drawing additional information
            frameNumberLabel.setString(std::to_string(frame));

            replayRender.window->draw(generationLabel);
            replayRender.window->draw(generationNumberLabel);
            replayRender.window->draw(frameLabel);
            replayRender.window->draw(frameNumberLabel);

            replayRender.window->display();

            sf::sleep(sf::seconds(sleepTime));

            frame++;
            isDisplay = frame < replay.size();
        }

        while (replayRender.window->pollEvent(event)){

            switch (event.type){
                case sf::Event::Closed:
                    exit = true;
                    break;

                case sf::Event::KeyPressed:

                    switch (event.key.code) {
                        case sf::Keyboard::Escape :
                            exit = true;
                            resetDisplay = false;   // In cas the main thread was waiting for a replay to end
                            break;
                        case sf::Keyboard::W :
                            waitEndOfReplay = !waitEndOfReplay;
                            std::cout << "Wait end of replay : " << waitEndOfReplay << std::endl;
                            break;
                        case sf::Keyboard::S :
                            isDisplay = false;
                            break;
                        default :
                            break;
                    }

                default:
                    break;
            }

        }

    }

    replayRender.close();

}
