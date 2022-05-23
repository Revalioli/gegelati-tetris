#ifndef GEGELATI_TETRIS_RENDER_H
#define GEGELATI_TETRIS_RENDER_H

#include "Tetris.h"
#include <SFML/Graphics.hpp>

#include <thread>
#include <atomic>

class Render {

    /// The associated Tetris learning environment
    Tetris& gameEnvironment;

    /// Tile size in pixels
    int tileSize;

    /// Is the Renderer initialised
    bool isInitialised;

    /// Pointer to the window for display
    sf::RenderWindow* window;

    /// Coloured allTiles
    sf::RectangleShape tile_cyan;       // I
    sf::RectangleShape tile_green;      // S
    sf::RectangleShape tile_red;        // Z
    sf::RectangleShape tile_magenta;    // T
    sf::RectangleShape tile_orange;     // L
    sf::RectangleShape tile_blue;       // J
    sf::RectangleShape tile_yellow;     // O

    sf::RectangleShape* allTiles[7];

    /// Grid edges shape
    sf::RectangleShape gridEdges;

    /// Text font to be used
    sf::Font font;

    /// Text objects for game score display
    sf::Text scoreTextLabel;
    sf::Text scoreLabel;


    friend class Tetris;

    friend void playFromRoot(std::atomic<bool>& exit, std::atomic<bool>& resetDisplay, const TPG::TPGVertex** bestRoot,
                             const Instructions::Set& set, Tetris& tetrisLE, const Learn::LearningParameters& params,
                             std::atomic<uint64_t>& generation, int seed);

public:

    /**
     *  \brief Constructor
     *
     * \param t the Tetris learning environment
     * \param tileSize the size in pixel of the allTiles on screen
     */
    Render(Tetris& t, int tileSize = 18);

    /// Destructor
    ~Render();

    /// Initialises SFML and opens the main window
    void initialise();

    /// Updates the window using the current data in the gameEnvironment.
    void update(bool display = true);

    /// Closes the Render.
    void close();
};

/// Displays a game played using a TPG
void playFromRoot(std::atomic<bool>& exit, std::atomic<bool>& resetDisplay, const TPG::TPGVertex** bestRoot,
                  const Instructions::Set& set, Tetris& tetrisLE, const Learn::LearningParameters& params,
                  std::atomic<uint64_t>& generation, int seed = 0);


#endif //GEGELATI_TETRIS_RENDER_H
