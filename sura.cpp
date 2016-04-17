#include <iostream>
#include <stdlib.h>
#include <stdlib.h>
#include <time.h>
#include <algorithm>

using std::cin;
using std::cout;
using std::endl;
using std::string;
using std::getline;
using std::transform;

bool shouldExit;

int playerX;
int playerY;

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

string getInput()
{
    string input;
    getline(cin, input);
    transform(input.begin(), input.end(), input.begin(), ::tolower);
    return input;
}

int getMaxHealth(character c)
{
    return c.fortitude;
}

struct room
{
    
} map[4][4];

void playerMoveNorth()
{
    if(playerY > 0)
    {
        playerY--;
        cout << "You moved north" << endl;
    }
    else
    {
        cout << "You are already as north as you can go" << endl;
    }
}
//void playerMoveEast;
//void playerMoveSouth;
//void playerMoveWest;

void actionNothing() {
    cout << "You did nothing" << endl;
}

void actionQuit() {
    cout << "Are you sure you want to quit? ";
    string answer = getInput();
    if(answer.front() == 'y')
    {
        shouldExit = true;
    }
    else if(answer.front() != 'n')
    {
        cout << "Invalid input" << endl;
        actionQuit();
    }
}

string getAction()
{
    cout << "What is your action this turn? ";
    return getInput();
}

void setup()
{
    shouldExit = false;
    player.health = getMaxHealth(player);
    cout << "Welcome to Sura!" << endl << endl;
    cout << "Please allocate your stats:" << endl;
    cout << "Welcome to Sura!" << endl;
    cout << "Welcome to Sura!" << endl;
}

void mainLoop()
{
    cout << endl;
    string input = getAction();
    
    if(input == "quit" || input == "exit" || input == "q")
    {
        actionQuit();
    }
    else if(input == "nothing" || input == "n")
    {
        actionNothing();
    }
    else
    {
        cout << "Invalid input \"" << input << "\"" << endl;
        actionNothing();
    }
}

int main()
{
    setup();
    while(!shouldExit)
    {
        mainLoop();
    }
    
    return 0;
}
