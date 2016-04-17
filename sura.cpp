#include <iostream>
#include <stdlib.h>
#include <stdlib.h>
#include <time.h>

using std::cin;
using std::cout;
using std::endl;
using std::string;

enum Input { QUIT, NOTHING };

int playerX;
int playerY;

Input latestInput;

struct item
{
    string name;
    string description;
    int strength;
    int agility;
    int fortitude;
    int health;
};

struct object
{
    string name;
    string description;
    void (*interact)();
};

struct character
{
    int strength = 10;
    int agility = 10;
    int fortitude = 10;
    int health;
    item items[4];
} player;

int getMaxHealth(character c)
{
    return c.fortitude;
}

struct room
{
    
} map[4][4];

void playerMoveNorth()
{
    if(playerY >0)
    {
        playerY --;
        cout << "You moved north" << endl;
    }
        else
        {
            cout << "You are already as north as you can get" << endl;
        }
}
//void playerMoveEast;
//void playerMoveSouth;
//void playerMoveWest;

void setup()
{
    player.health = getMaxHealth(player);
    cout << "Welcome to Sura!" << endl << endl;
    cout << "Please allocate your stats:" << endl;
    cout << "Welcome to Sura!" << endl;
    cout << "Welcome to Sura!" << endl;
}

void actionNothing() {
    cout << "You did nothing" << endl;
}

void mainLoop()
{
    switch(latestInput)
    {
        case NOTHING:
            actionNothing();
            break;
        default:
            std::cerr << "invalid Input" << endl;
            break;
    }
}


Input getInput()
{
    cout << "What is your action this turn? ";
    string input;
    std::getline(cin, input);
    latestInput = NOTHING;
    return NOTHING;
}

int main()
{
    setup();
    do
    {
        mainLoop();
    } while(getInput() != QUIT);
    
    return 0;
}
