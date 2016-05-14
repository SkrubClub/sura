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

struct item
{
    string name;
    string description;
    int strength;
    int agility;
    int fortitude;
    int health;
    int damage;
};

struct object
{
    string name;
    string description;
    void (*interact)();
};

struct playercharacter
{
    int x;
    int y;
    int strength;
    int agility;
    int fortitude;
    int maxHealth;
    int health;
    int damage;
    item items[16];
} player;

struct character
{
    int damage;
    int maxHealth;
    int health;
};

string getInput()
{
    string input;
    getline(cin, input);
    transform(input.begin(), input.end(), input.begin(), ::tolower);
    return input;
}

bool getYesNo()
{
    string answer;
    while(true)
    {
        answer = getInput();
        if(answer.at(0) == 'y')
        {
            return true;
        }
        else if(answer.at(0) == 'n')
        {
            return false;
        }
        else
        {
            cout << "Invalid input \"" << answer << "\". Please answer yes/no/y/n: " << endl;
        }
    }
}

int getMaxHealth()
{
    return player.fortitude;
}

struct room
{
    item items[16];
    object objects[16];
} map[4][4];

void playerMoveNorth()
{
    if(player.y > 0)
    {
        player.y--;
        cout << "You moved north" << endl;
    }
    else
    {
        cout << "You are already as far north as you can go" << endl;
    }
}

void playerMoveEast()
{
    if(player.x > 0)
    {
        player.x++;
        cout << "You moved east" << endl;
    }
    else
    {
        cout << "You are already as far east as you can go" << endl;
    }
}

void playerMoveSouth()
{
    if(player.y > 0)
    {
        player.y++;
        cout << "You moved south" << endl;
    }
    else
    {
        cout << "You are already as far south as you can go" << endl;
    }
}

void playerMoveWest()
{
    if(player.x > 0)
    {
        player.x--;
        cout << "You moved west" << endl;
    }
    else
    {
        cout << "You are already as far west as you can go" << endl;
    }
}

void actionNothing()
{
    cout << "You did nothing" << endl;
}

void actionQuit()
{
    cout << "Are you sure you want to quit? ";
    shouldExit = getYesNo();
}

string getAction()
{
    cout << "What is your action this turn? ";
    return getInput();
}

void setup()
{
    shouldExit = false;
    player.maxHealth = getMaxHealth();
    player.health = player.maxHealth;
    
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

void endgame()
{
    
}

int main()
{
    setup();
    while(!shouldExit)
    {
        mainLoop();
    }
    endgame();
    return 0;
}
