#ifndef GEGELATI_TETRIS_RENDER_H
#define GEGELATI_TETRIS_RENDER_H

#include "Tetris.h"
#include <SFML/Graphics.hpp>

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
    void update();

    /// Closes the Render.
    void close();
};


#endif //GEGELATI_TETRIS_RENDER_H
