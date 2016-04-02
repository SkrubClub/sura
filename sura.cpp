#include <iostream>

using std::cin;
using std::cout;
using std::endl;
using std::string;

enum Input { QUIT, NOTHING };

Input latestInput;

struct character
{
    int strength = 10;
    int agility = 10;
    int fortitude = 10;
    int health;
} player;

int getMaxHealth(character c)
{
    return c.fortitude;
}

void setup()
{
    player.health = getMaxHealth(player);
    cout << "Welcome to Sura!" << endl;
}

void mainLoop()
{
    switch(latestInput)
    {
        case NOTHING:
            cout << "You did nothing" << endl;
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
