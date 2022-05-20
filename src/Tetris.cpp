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
        3, 5, 4, 6, // Z
        3, 5, 4, 7, // T
        2, 3, 5, 7, // L
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

    // Placing block on grid
    for(auto& block : this->activeTetrominoPos)
        setTileAt(block.x, block.y, this->activeTetrominoType);

    this->fallCounter = 0;
    this->gameScore = 0;
    this->gameOver = false;
}

std::vector<std::reference_wrapper<const Data::DataHandler>> Tetris::getDataSources() {
    auto result = std::vector<std::reference_wrapper<const Data::DataHandler>>();
    result.push_back(this->grid);
    return result;
}

double Tetris::getScore() const {
    return this->gameScore;
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

        if(!checkActiveTetromino()){
            this->gameOver = true;
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

    const sf::Vector2<int>& rotationPoint = t[1];

    for(auto& block : t){
        int x = block.y - rotationPoint.y;
        int y = block.x - rotationPoint.x;
        block.x = rotationPoint.x - x;
        block.y = rotationPoint.y + y;
    }

}

const Data::PrimitiveTypeArray2D<double>& Tetris::getGrid(){
    return this->grid;
}

int Tetris::getGameScore() { return this->gameScore; }

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