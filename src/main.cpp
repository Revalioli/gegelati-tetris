#include <iostream>
#include <vector>

#include <gegelati.h>
#include <SFML/System/Vector2.hpp>

#include "Tetris.h"
#include "Render.h"

int main(){

    Tetris le;
    le.playSolo();

//    le.reset(6);

//    Render render(le, 18);
//    render.initialise();

//    sf::Time timeToSleep = sf::milliseconds(500);

//    std::cout << "Initial state" << std::endl;
//    render.update();

//    uint64_t actions[10] = { 0, 1, 1, 3, 3, 2, 2, 2, 4, 3};
//    uint64_t actions[10] = { 4, 4, 4, 4, 4, 4, 4, 4, 4, 4};

//    for(int i = 0; i < 10; i++){
//        std::cout << "Turn " << i+1 << std::endl;
//
//        le.doAction(actions[i]);
//        render.update();
//
//        sf::sleep(timeToSleep);
//    }

}