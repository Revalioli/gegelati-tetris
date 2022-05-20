#include "Render.h"

#include <iostream>
#include <stdexcept>
#include <SFML/Graphics.hpp>

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

    this->allTiles[0] = &tile_green;
    this->allTiles[1] = &tile_red;
    this->allTiles[2] = &tile_orange;
    this->allTiles[3] = &tile_blue;
    this->allTiles[4] = &tile_yellow;
    this->allTiles[5] = &tile_cyan;
    this->allTiles[6] = &tile_magenta;

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
    window = new sf::RenderWindow(sf::VideoMode(this->tileSize * this->gameEnvironment.WIDTH + 140,
                                                this->tileSize * this->gameEnvironment.HEIGHT),
                                                "Tetris LE view");
}

void Render::update() {
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
    this->window->draw(this->scoreLabel);

    this->window->draw(this->gridEdges);

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
