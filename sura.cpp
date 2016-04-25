#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <algorithm>
#include <cstdio>

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

string getHealthBar(int health, int maxHealth)
{
    string healthBar = "[";
    for(int i = 0; i < health; i++)
    {
        healthBar += "=";
    }
    for(int i = 0; i < maxHealth - health; i++)
    {
        healthBar += " ";
    }
    healthBar += "]";
    return healthBar;
}

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
//void playerMoveEast;
//void playerMoveSouth;
//void playerMoveWest;

void inspectSelf()
{
    printf("-- Stats --\n");
    printf("Fortitude:%4i    Health:%6s %s\n", player.fortitude, player.health + "/" + player.maxHealth, getHealthBar(player.health, player.maxHealth).c_str());
    printf("Strength:%5i    Damage:%6i\n", player.strength, player.damage);
    printf("Agility:%6i\n", player.agility);
}

void actionInspect(string inspection)
{
    if(inspection == "self")
    {
        inspectSelf();
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
    
    if(input.substr(0, 8) == "inspect ")
    {
        actionInspect(input.substr(8));
    }
    else if(input == "quit" || input == "exit" || input == "q")
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
