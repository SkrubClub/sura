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
    // Attributes
    int strength = 0;
    int agility = 0;
    int fortitude = 0;
    int intellect = 2;

    /* Traits

    0   feeble      strength < minimum
    1   clumsy      agility < minimum
    2   anorexic    fortitude < minimum
    3   witless     intellect < minimum
    4   muscular    strength > maximum
    5   nimble      agility > maximum
    6   healthy     fortitude > maximum
    7   shrewd      intelligence > maximum

    obese       fortitude > strength
    heavyhanded strength > agility

    */
    bool trait[8] = {0,0,0,0,0,0,0,0};

    /* Perks

    Experienced
    Lucky

    */
    // Derived stats

    int weight;
    int speed;
    //int

    int level = 1;
    int freePoints = 8;
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
    return c.fortitude*3 + 1;
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
    player.health = getMaxHealth(player);
    cout << "Welcome to Sura!" << endl << endl;
    cout << "Please allocate your stats:" << endl << endl;
    cout << "Strength: " << player.strength << endl;
    cout << "Agility: " << player.agility << endl;
    cout << "Fortitude: " << player.fortitude << endl;
    cout << "Unallocated skill points: " << player.freePoints << endl;

    cout << "Would you like to reallocate your skill points?" << endl;
    string input = getInput();
    if (input.front() == 'y')
    {
        cout << "Please allocate your stats in the form of:" << endl << endl;
        cout << "Strength Agility Fortitude" << endl;
        cout << "e.g." << endl;
        cout << "3 4 1" << endl << endl;
        string input = getInput();
    }


}

bool isBelowMin(int level, int stat)
{
    return stat <= level * 0.8 - 6;
}

void applyTraits(character c)
{
    if(c.strength < 0)
    {
        c.freePoints -= c.strength;
        c.strength = 0;
    }
    if (c.agility < 0) {c.freePoints -= c.agility; c.agility = 0;}
    if (c.fortitude < 0) {c.fortitude -= c.strength; c.fortitude = 0;}
    if (c.intellect < 0) {c.freePoints -= c.intellect; c.intellect = 0;}

    c.trait[0] = isBelowMin(c.level, c.strength);
    if (c.strength <= c.level*0.8 - 6) {c.trait[0] = 1;}
    if (c.agility <= c.level*0.8 - 6) {c.trait[1] = 1;}
    if (c.fortitude <= c.level*0.8 - 6) {c.trait[2] = 1;}
    if (c.intellect <= c.level*0.8 - 6) {c.trait[3] = 1;}
    if (c.strength >= c.level*1.25 + 3) {c.trait[4] = 1;}
    if (c.agility >= c.level*1.25 + 3) {c.trait[5] = 1;}
    if (c.fortitude >= c.level*1.25 + 3) {c.trait[6] = 1;}
    if (c.intellect >= c.level*1.25 + 3) {c.trait[7] = 1;}
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
