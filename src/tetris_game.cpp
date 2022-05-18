/**
 * This file is an adapted version of the tetris clone you can get on the github repository https://github.com/Zedespook/tetris-clone
 */

#include <SFML/Graphics.hpp>
#include <time.h>
#include <iostream>

#define FPS 60

using namespace sf;


float framePeriod = 1.f/FPS;

/// Size of the main grid
const int height = 20;
const int width = 10;

/// Main grid of the game
static int grid[height][width];
// 0 : empty tile
// !0 : not empty tile

struct Point {
    int x, y;
} currentPos[4], lastPosition[4];   // Coordinates in the grid of the tiles of the current and last position of the active tetromino

// Tetrominos are defined in a 2*4 sub-grid.
// They all have 4 tile, and are here described using indexes of
// their tiles in the 2*4 grid
int shapes[7][4] = {
    1, 3, 5, 7, // I
    2, 4, 5, 7, // Z
    3, 5, 4, 6, // S
    3, 5, 4, 7, // T
    2, 3, 5, 7, // L
    3, 5, 7, 6, // J
    2, 3, 4, 5  // O
};

bool check()
{
    for (int i = 0; i < 4; i++)
    {
        // Check for the shape being in the grid
        if (currentPos[i].x < 0 || currentPos[i].x >= width || currentPos[i].y >= height)
            return false;

        if (grid[currentPos[i].y][currentPos[i].x])
            return false;
    }

    return true;
};

// Main function for running the game.
int main()
{
    // Generates a random seed for the session
    srand(time(nullptr));

    // The main render for the window
    RenderWindow window(VideoMode(320, 480), "Tetris Clone");

    // Tetromino tiles
    RectangleShape tile_green(Vector2f(18.f, 18.f));
    tile_green.setFillColor(Color::Green);
    RectangleShape tile_red(Vector2f(18.f, 18.f));
    tile_red.setFillColor(Color::Red);
    RectangleShape tile_orange(Vector2f(18.f, 18.f));
    Color colourOrange(255,153,51);
    tile_orange.setFillColor(colourOrange);
    RectangleShape tile_blue(Vector2f(18.f, 18.f));
    tile_blue.setFillColor(Color::Blue);
    RectangleShape tile_yellow(Vector2f(18.f, 18.f));
    tile_yellow.setFillColor(Color::Yellow);
    RectangleShape tile_cyan(Vector2f(18.f, 18.f));
    tile_cyan.setFillColor(Color::Cyan);
    RectangleShape tile_magenta(Vector2f(18.f, 18.f));
    tile_magenta.setFillColor(Color::Magenta);

    RectangleShape* allTiles[7] = {&tile_cyan, &tile_red, &tile_green, &tile_magenta, &tile_orange, &tile_blue, &tile_yellow};

    RectangleShape gridEdges(Vector2f(18.f * width, 18.f * height));
    gridEdges.setFillColor(Color::Transparent);
    gridEdges.setOutlineThickness(-1);
    gridEdges.setOutlineColor(Color::White);
    gridEdges.setPosition(0,0);

    // Tile
    int dx = 0;
    bool rotate = false;
    int currentTetrominoType = rand() % 7;  // Choose inital tetromino

    for (int i = 0; i < 4; i++)
    {
        currentPos[i].x = shapes[currentTetrominoType-1][i] % 2;
        currentPos[i].y = shapes[currentTetrominoType-1][i] / 2;
    }


    // Game clock variables.
    float fallTimer = 0.f;
    float delay = 0.3f; // time between two frames
    static Clock clock;
    Time frameTime = seconds(framePeriod);

    int score = 0.f;

    static Font font;
    if (!font.loadFromFile(ROOT_DIR "/dat/hemi.ttf"))
        std::cout << "Error loading font file." << std::endl;

    static Text scoreTextLabel;
    scoreTextLabel.setFont(font);
    scoreTextLabel.setString("Cleared:");
    scoreTextLabel.setCharacterSize(24);
    scoreTextLabel.setFillColor(Color::White);

    static Text scoreLabel;
    scoreLabel.setFont(font);
    scoreLabel.setCharacterSize(24);
    scoreLabel.setFillColor(Color::White);

    bool gameOver = false;

    while (!gameOver)
    {
        float time = clock.getElapsedTime().asSeconds();    // Time elapsed during last game loop iteration
        clock.restart();
        fallTimer += time;

        Event event;
        while (window.pollEvent(event))
        {
            switch (event.type)
            {
            case Event::Closed:
                window.close();
                break;

            case Event::KeyPressed:

                switch (event.key.code)
                {
                case Keyboard::Up:
                    rotate = true;
                    break;
                case Keyboard::Left:
                    dx = -1;
                    break;
                case Keyboard::Right:
                    dx = 1;
                    break;
                case Keyboard::Escape:
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

        if (Keyboard::isKeyPressed(Keyboard::Down))
            delay = 0.05;

        // * Movement
        for (int i = 0; i < 4; i++)
        {
            lastPosition[i] = currentPos[i];
            currentPos[i].x += dx;
        }

        // Out of bounds check
        if (!check())
        {
            for (int i = 0; i < 4; i++)
                currentPos[i] = lastPosition[i];
        }

        // * Rotation
        if (rotate)
        {
            // Center of the rotation.
            Point point = currentPos[1];

            for (int i = 0; i < 4; i++)
            {
                int x = currentPos[i].y - point.y;
                int y = currentPos[i].x - point.x;
                currentPos[i].x = point.x - x;
                currentPos[i].y = point.y + y;
            }
        }

        if (!check())
        {
            for (int i = 0; i < 4; i++)
                currentPos[i] = lastPosition[i];
        }


        // * Falling check
        if (fallTimer > delay)
        {
            for (int i = 0; i < 4; i++)
            {
                lastPosition[i] = currentPos[i];
                currentPos[i].y += 1;
            }

            if (!check())
            {
                // Tetromino has touch the ground
                for (int i = 0; i < 4; i++)
                    grid[lastPosition[i].y][lastPosition[i].x] = currentTetrominoType;

                int n = rand() % 7;
                currentTetrominoType = 1 + n;  // Choosing the next tetromino

                for (int i = 0; i < 4; i++)
                {
                    currentPos[i].x = shapes[n][i] % 2;
                    currentPos[i].y = shapes[n][i] / 2;
                }

                // Checking for game over
                if(!check())
                    gameOver = true;

            }

            fallTimer = 0;
        }

        dx = 0;
        rotate = false;
        delay = 0.3f;

        // * Line clear
        int k = height - 1;

        for (int i = height - 1; i > 0; i--)
        {
            int count = 0;

            for (int j = 0; j < width; j++)
            {
                if (grid[i][j])
                    count++;

                grid[k][j] = grid[i][j];
            }

            if (count < width)
                k--;
        }

        // * Scores
        score += k;
        scoreLabel.setString(std::to_string(score));

        // * Draw to window
        window.clear(Color::Black);

        for (int i = 0; i < height; i++)
        {
            for (int j = 0; j < width; j++)
            {
                if (grid[i][j] == 0)
                    continue;

                RectangleShape * tileToDraw = allTiles[grid[i][j] - 1];

                tileToDraw->setPosition(j * 18, i * 18);
                window.draw(*tileToDraw);
            }
        }

        for (int i = 0; i < 4; i++)
        {
            RectangleShape * tileToDraw = allTiles[currentTetrominoType - 1];

            tileToDraw->setPosition(currentPos[i].x * 18, currentPos[i].y * 18);
//            window.draw(*allTiles[currentTetrominoType-1]);
            window.draw(*tileToDraw);
        }

        scoreTextLabel.setPosition(200, 240);
        window.draw(scoreTextLabel);

        scoreLabel.setPosition(235, 270);
        window.draw(scoreLabel);

        window.draw(gridEdges); // Drawing edges of the grid

        window.display();

        // Wait until next frame

        Time activeFrameTime = clock.getElapsedTime();
        Time endOfFrameDelay =  frameTime - activeFrameTime;
        if(endOfFrameDelay.asMilliseconds() > 0)
            sleep(endOfFrameDelay);
    }

    std::cout << "Game over" << std::endl << "Final score : " << score;

    return 0;
}