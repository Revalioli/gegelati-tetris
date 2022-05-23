#ifndef GEGELATI_TETRIS_TETRIS_H
#define GEGELATI_TETRIS_TETRIS_H

#include <gegelati.h>
#include <SFML/System/Vector2.hpp>

/// Tetromino's blocks coordinates in the grid,
///  each tetromino having 4 blocks
using Tetromino = sf::Vector2<int>[4];


class Tetris : public Learn::LearningEnvironment {
private:

    /// The main grid of the game
    /// 0 is an empty tile, otherwise it's a positive number depending on the colour/parent shape of the tile :
    /// 1 : Cyan (I)
    /// 2 : Green (S)
    /// 3 : Red (Z)
    /// 4 : Magenta (T)
    /// 5 : Orange (L)
    /// 6 : Blue (J)
    /// 7 : Yellow (O)
    /// Its size is 10 columns * 20 lines (original tetris grid size) (might be configurable in the future).
    Data::PrimitiveTypeArray2D<double> grid;
    // When accessing data with getDataAt, content is in the form of a 1D array, line after line

    /// Number of available actions, at the moment : moving right (0), moving left (1),
    /// rotate clockwise (2), accelerate fall (3) and do nothing (4)
    static const int NB_ACTIONS;

    /// Number fo frame between each time the active tetromino falls from one block
    static const int FRAMES_PER_FALL;

    /// Initial coordinate of the top left block of a newly generated tetromino
    static const sf::Vector2<int> TETROMINO_INITIAL_POSITION;

    /// Template for tetrominos, gives block position
    /// in a 2 columns * 4 lines grid
    static const int TETROMINO_TEMPLATES[7][4];

    /// Type of the currently falling tetromino, this type follows the same association
    /// given for the grid content
    int activeTetrominoType;

    /// Active tetromino's block coordinates
    Tetromino activeTetrominoPos;

    /// Active tetromino's last coordinates
    Tetromino lastTetrominoPos;

    /// Frame counter since last fall
    int fallCounter;

    /// Is the game finished
    bool gameOver;

    /// Does the active tetromino has to fall faster
    bool accelerateFall;

    /// Randomness control, used for tetromino generation
    Mutator::RNG rng;

    /* Scoring */

    /// Score of the game, currently the number of cleared lines
    int gameScore;

    /// Number of forbidden movement since reset
    int nbForbiddenMoves;

    /// Number of played tetrominos since reset
    int nbPlayedTetrominos;

    /* Global scoring */

    /// Best game score since instantiation or record reset
    int gameScoreRecord;

    /// Total number of forbidden moves since last global reset
    double accumulateForbiddenMoves;

    /// Total number of games launched (aka reset) since last global reset
    int nbGames;

protected:

    /// Generates new active tetromino on top of the grid.
    void getNewTetromino();

    /// Rotate clockwise a tetromino
    void rotateTetromino(Tetromino& t);

    /// Clear completed lines and update the game Score
    void clearLines();

    /// Sets tile value at (x,y) in the grid.
    void setTileAt(int x, int y, double value);

public:

    /// Height of the grid
    static const int HEIGHT;
    /// Width of the grid
    static const int WIDTH;

    /**
     * \brief Default constructor.
     */
    Tetris() : LearningEnvironment(NB_ACTIONS), gameScore(0), activeTetrominoType(0),
               gameScoreRecord(0), accumulateForbiddenMoves(0), nbGames(0),
               grid(WIDTH, HEIGHT), gameOver(false), accelerateFall(false) {};

    /**
     * \brief Copy constructor.
     *
     * \param other the Tetris learning environment to be copied.
     */
    Tetris(const Tetris& other) = default;

    /// Destructor
    ~Tetris() override = default;

    /* LearningEnvironment methods */

    /// Clones the current Tetris learning environment.
    virtual Learn::LearningEnvironment *clone() const override;

    /// Returns true.
    virtual bool isCopyable() const override;

    /// Inherited via LearningEnvironment.
    virtual void doAction(uint64_t actionID) override;

    /// Inherited via LearningEnvironment.
    virtual void reset(size_t seed = 0, Learn::LearningMode mode = Learn::LearningMode::TRAINING) override;

    /// Inherited via LearningEnvironment.
    virtual std::vector<std::reference_wrapper<const Data::DataHandler>> getDataSources() override;

    /// Inherited via LearningEnvironment.
    virtual double getScore() const override;

    /// Inherited via LearningEnvironment.
    virtual bool isTerminal() const override;

    /// Gets tile value at (x,y) in the grid.
    double getTileAt(int x, int y) const;

    /// Returns a const reference to the grid PrimitiveTypeArray2D
    const Data::PrimitiveTypeArray2D<double>& getGrid();

    /// Resets global data field (such as gameScoreRecord)
    void resetGlobalData();

    /* Game methods */

    /**
     * \brief Checks if the active tetromino has a valid position.
     *
     * \return false if it overlaps with other block or is out of bounds, true otherwise.
     */
    bool checkActiveTetromino();

    int getGameScore();

    int getGameScoreRecord();

    double getAverageForbiddenMoves();

    /// Starts a singleplayer Tetris game
    void playSolo();

};


#endif //GEGELATI_TETRIS_TETRIS_H
