#include <SFML/System/Clock.hpp>
#include <SFML/System/Sleep.hpp>
#include <SFML/Window/Event.hpp>

#include "Tetris.h"
#include "Render.h"

const int Tetris::HEIGHT = 20;
const int Tetris::WIDTH = 10;
const int Tetris::NB_ACTIONS = 5;
const sf::Vector2<int> Tetris::TETROMINO_INITIAL_POSITION = sf::Vector2<int>(4,0);
const int Tetris::FRAMES_PER_FALL = 20;

const int Tetris::TETROMINO_TEMPLATES[7][4] = {
        1, 3, 5, 7, // I
        2, 4, 5, 7, // S
        3, 4, 5, 6, // Z
        3, 5, 4, 7, // T
        2, 5, 3, 7, // L
        3, 5, 7, 6, // J
        2, 3, 4, 5  // O
};

void Tetris::reset(size_t seed, Learn::LearningMode mode) {
    // Create seed from seed and mode
    size_t hash_seed = Data::Hash<size_t>()(seed) ^ Data::Hash<Learn::LearningMode>()(mode);

    // Reset the rng generator
    this->rng.setSeed(hash_seed);

    // Set the initial state
    for(int i = 0; i < this->grid.getAddressSpace(typeid(double)); i++)
        this->grid.setDataAt(typeid(double), i, 0.0);

    this->getNewTetromino();
    this->nbTetroRotations = 0;

    // Placing block on grid
    for(auto& block : this->activeTetrominoPos)
        setTileAt(block.x, block.y, this->activeTetrominoType);

    this->fallCounter = 0;
    this->gameScore = 0;
    this->nbForbiddenMoves = 0;
    this->nbPlayedTetrominos = 0;
    this->nbPlayedFrames = 0;
    this->rewardBonus = 0;
    this->nbGames++;

    // std::cout << accumulateForbiddenMoves << std::endl;

    this->gameOver = false;
}

void Tetris::resetGlobalData(){
    this->gameScoreRecord = 0;
    this->accumulateForbiddenMoves = 0;
    this->nbGames = 0;
}

std::vector<std::reference_wrapper<const Data::DataHandler>> Tetris::getDataSources() {
    auto result = std::vector<std::reference_wrapper<const Data::DataHandler>>();
    result.push_back(this->grid);
    return result;
}

double Tetris::getScore() const {
    return this->gameScore * 100 + this->nbPlayedTetrominos - this->nbForbiddenMoves * 0.05 + this->rewardBonus;
}

bool Tetris::isTerminal() const {
    return this->gameOver;
}

Learn::LearningEnvironment *Tetris::clone() const {
    return new Tetris(*this);
}

bool Tetris::isCopyable() const {
    return true;
}

void Tetris::doAction(uint64_t actionID) {
    Learn::LearningEnvironment::doAction(actionID);

    if(isTerminal())
        return;

    this->nbPlayedFrames++;

    // Remove active tetromino from the grid to avoid false collision when checking
    for(auto& block : this->activeTetrominoPos)
        setTileAt(block.x, block.y, 0);

    for (int i = 0; i < 4; ++i)
        this->lastTetrominoPos[i] = this->activeTetrominoPos[i];

    switch (actionID) {
        // Move right
        case 0:
            for(auto& block : this->activeTetrominoPos)
                block.x += 1;
            break;
        // Move left
        case 1:
            for(auto& block : this->activeTetrominoPos)
                block.x -= 1;
            break;
        // Rotate
        case 2:
            rotateTetromino(this->activeTetrominoPos);
            this->nbTetroRotations++;
            break;
        // Accelerate fall
        case 3:
            this->accelerateFall = true;
            break;
        // Do nothing
        default:
            break;
    }

    // Revert position if invalid
    if(!checkActiveTetromino()){
        for (int i = 0; i < 4; ++i)
            this->activeTetrominoPos[i] = this->lastTetrominoPos[i];

        if(actionID == 2)
            this->nbTetroRotations--;

        this->nbForbiddenMoves++;
    }

    if(this->nbTetroRotations == 4){
        this->nbTetroRotations = 0;
        this->nbForbiddenMoves += 4;
    }

    this->fallCounter++;

    if(this->fallCounter == FRAMES_PER_FALL || this->accelerateFall){
        for(auto& block : this->activeTetrominoPos)
            block.y += 1;

        this->fallCounter = 0;
        this->accelerateFall = false;
    }

    if(!checkActiveTetromino()){
        // Active tetromino can't fall

        for(auto& block : this->activeTetrominoPos)
            block.y -= 1;

        // Freezing tetromino in grid
        for(auto& block : this->activeTetrominoPos)
            setTileAt(block.x, block.y, this->activeTetrominoType);

        clearLines();

        getNewTetromino();
        this->nbPlayedTetrominos++;

        // Played frames malus computation
        this->rewardBonus -= exp((double)this->nbPlayedFrames/300.0);
        this->nbPlayedFrames = 0;

        if(!checkActiveTetromino()){
            this->gameOver = true;
            if(this->gameScore > this->gameScoreRecord)
                this->gameScoreRecord = this->gameScore;

            this->accumulateForbiddenMoves += this->nbForbiddenMoves;
        }
    }

    // Replacing active tetromino in grid for learning agent
    for(auto& block : this->activeTetrominoPos)
        setTileAt(block.x, block.y, this->activeTetrominoType);

}

bool Tetris::checkActiveTetromino() {

    for(auto& block : this->activeTetrominoPos){

        // Edges check
        if(block.x < 0 || block.x >= WIDTH || block.y >= HEIGHT)
            return false;

        // Overlap with other blocks check
        if(getTileAt(block.x, block.y) != 0)
            return false;
    }

    return true;
}

double Tetris::getTileAt(int x, int y) const {
    return *this->grid.getDataAt(typeid(double), y*WIDTH + x).getSharedPointer<double>();
}

void Tetris::setTileAt(int x, int y, double value) {
    this->grid.setDataAt(typeid(double), y*WIDTH + x, value);
}

void Tetris::getNewTetromino(){
    // Generates new tetromino type
    this->activeTetrominoType = this->rng.getInt32(1, 7);

    for(int i = 0; i < 4; i++){
        // Column
        this->activeTetrominoPos[i].x = TETROMINO_TEMPLATES[this->activeTetrominoType - 1][i] % 2
                                  + TETROMINO_INITIAL_POSITION.x;
        // Line
        this->activeTetrominoPos[i].y = TETROMINO_TEMPLATES[this->activeTetrominoType - 1][i] / 2
                                  + TETROMINO_INITIAL_POSITION.y;
    }

}

void Tetris::rotateTetromino(Tetromino& t){

    // Check for O tetromino because they don't rotate
    if(activeTetrominoType != 7){

        if(activeTetrominoType == 1){
            // I tetromino don't have a central point of rotation
            if(t[0].y == t[1].y){   // Tetromino is horizontal
                t[0].x -= 2; t[0].y -= 2;
                t[1].x -= 1; t[1].y -= 1;
                t[3].x += 1; t[3].y += 1;
            }
            else{   // Tetromino is vertical
                t[0].x += 2; t[0].y += 2;
                t[1].x += 1; t[1].y += 1;
                t[3].x -= 1; t[3].y -= 1;
            }
        }
        else{
            const sf::Vector2<int>& rotationPoint = t[1];

            for(auto& block : t){
                int x = block.y - rotationPoint.y;
                int y = block.x - rotationPoint.x;
                block.x = rotationPoint.x - x;
                block.y = rotationPoint.y + y;
            }
        }

    }

}

const Data::PrimitiveTypeArray2D<double>& Tetris::getGrid(){
    return this->grid;
}

int Tetris::getGameScore() { return this->gameScore; }

int Tetris::getGameScoreRecord() { return this->gameScoreRecord; }

double Tetris::getAverageForbiddenMoves() { return (this->accumulateForbiddenMoves / (double)this->nbGames); }

void Tetris::clearLines() {

    int k = HEIGHT-1;   // Destination line for falling lines

    for(int line = HEIGHT-1; line > 0; line--){
        int count = 0;

        for(int tile = 0; tile < WIDTH; tile++){
            double currentTile = getTileAt(tile, line);
            if(currentTile != 0)
                count++;

            setTileAt(tile, k, currentTile);
        }

        if(count < WIDTH)
            k--;
    }

    gameScore += k;

}



void Tetris::playSolo() {
    this->reset(time(nullptr));
    int actionID = 0;

    Render render(*this, 18);
    render.initialise();
    render.update();

    int fps = 60;
    sf::Clock clk;
    sf::Time frameTime = sf::seconds(1.f/fps);

    // To slow down the game for the human player
    int framePerMove = 5;
    int frameCounter = 0;

    while(!isTerminal()){
        clk.restart();

        sf::Event event;
        while (render.window->pollEvent(event))
        {
            switch (event.type)
            {
                case sf::Event::Closed:
                    gameOver = true;
                    break;

                case sf::Event::KeyPressed:

                    switch (event.key.code)
                    {
                        case sf::Keyboard::Up:
                            actionID = 2;
                            break;
                        case sf::Keyboard::Left:
                            actionID = 1;
                            break;
                        case sf::Keyboard::Right:
                            actionID = 0;
                            break;
                        case sf::Keyboard::Down:
                            actionID = 3;
                            break;
                        case sf::Keyboard::Escape:
                            gameOver = true;
                            break;

                        default:
                            break;
                    }
                    break;

                default:
                    break;
            }
        }

        frameCounter++;
        if(frameCounter >= framePerMove){
            doAction(actionID);
            actionID = 4;
            frameCounter = 0;
        }
        else
            doAction(4);

        render.update();

        sf::Time activeFrameTime = clk.getElapsedTime();
        sf::Time endOfFrameDelay =  frameTime - activeFrameTime;
        if(endOfFrameDelay.asMilliseconds() > 0)
            sf::sleep(endOfFrameDelay);
    }

    render.close();

    std::cout << "Game Over" << std::endl << "Score : " << this->gameScore << std::endl;

}