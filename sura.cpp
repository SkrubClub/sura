#include <iostream>
#include <stdlib.h>
#include <stdlib.h>
#include <time.h>
#include <algorithm>
#include <string>

using std::cin;
using std::cout;
using std::endl;
using std::getline;
using std::max;
using std::min;
using std::size_t;
using std::string;
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
    int dexterity = 0;
    int fortitude = 0;
    int agility = 0;
    int intellect = 2;

    /* Traits

    0   feeble      strength < min
    1   clumsy      dexterity < min
    2   anorexic    fortitude < min
    3   sluggish    agility < min
    4   witless     intellect < min
    5   muscular    strength > max
    6   deft        dexterity > max
    7   healthy     fortitude > max
    8   nimble      agility > max
    9   shrewd      intellect > max

    obese       fortitude > strength
    heavyhanded strength > agility

    */
    bool trait[10] = {0,0,0,0,0,0,0,0,0,0};

    /* Perks

    Experienced
    Lucky

    */
    // Derived stats

        // Modifiers
    int damageMod;
    int weightCapacityMod;
    int critThresh1Mod;
    int critThresh2Mod;
    int healthMod;
    int actpointMod;
    int weightMod;
    int dodgeThresh1Mod;
    int dodgeThresh2Mod;
    int movespeedMod;

        // Reduction Modifiers
    int reduceDamageMod;
    int reduceActpointMod;

        // Pseudo-stats
    int health;
    int actpoint;

        // Variables for use with stats
    int critRangeRand;
    /*
    int Base;
    int Mod;
    int Total;
    */
    //int

    int level = 1;
    int freePoints = 10;
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
    int statInput = 0;
    int prevAmnt = 0;
    string input;
    string tempInput;

    cout << "Welcome to Sura!" << endl << endl;
    cout << "Please allocate your stats:" << endl << endl;
    cout << "Strength: " << player.strength << endl;
    cout << "Dexterity: " << player.dexterity << endl;
    cout << "Fortitude: " << player.fortitude << endl;
    cout << "Agility: " << player.agility << endl;
    cout << "Unallocated skill points: " << player.freePoints << endl << endl;
    input = 'n';

    while (input.front() != 'y')
    {
        cout << "How many points would you like in strength?" << endl;
        cin >> statInput;
        prevAmnt = player.strength;
        player.strength = min(statInput,min(player.freePoints,3));
        player.freePoints -= (player.strength-prevAmnt);

        cout << "How many points would you like in dexterity?" << endl;
        cin >> statInput;
        prevAmnt = player.dexterity;
        player.dexterity = min(statInput,min(player.freePoints,3));
        player.freePoints -= (player.dexterity-prevAmnt);

        cout << "How many points would you like in fortitude?" << endl;
        cin >> statInput;
        prevAmnt = player.dexterity;
        player.fortitude = min(statInput,min(player.freePoints,3));
        player.freePoints -= (player.dexterity-prevAmnt);

        cout << "How many points would you like in agility?" << endl;
        cin >> statInput;
        prevAmnt = player.agility;
        player.agility = min(statInput,min(player.freePoints,3));
        player.freePoints -= (player.agility-prevAmnt);

        cin.sync();

        cout << "Your current stats are:" << endl;
        cout << "Strength: " << player.strength << endl;
        cout << "Dexterity: " << player.dexterity << endl;
        cout << "Fortitude: " << player.fortitude << endl;
        cout << "Agility: " << player.agility << endl;
        cout << "Unallocated points: " << player.freePoints << endl << endl;
        cout << "Would you like to proceed?" << endl;
        //getInput();
        input = getInput();
        cout << "input = \"" << input << "\"" << endl;
    }
    //player.health = getMaxHealth(player);
}

void addStatPoints(int &stat, int amount)
{

}

bool isBelowMin(int level, int stat)
{
    return stat <= level * 0.8 - 6;
}

void applyTraits(character c)
{
    /*
    if(c.strength < 0)
    {
        c.freePoints -= c.strength;
        c.strength = 0;
    }
    if (c.agility < 0) {c.freePoints -= c.agility; c.agility = 0;}
    if (c.fortitude < 0) {c.fortitude -= c.strength; c.fortitude = 0;}
    if (c.intellect < 0) {c.freePoints -= c.intellect; c.intellect = 0;}
    */

    c.trait[0] = isBelowMin(c.level, c.strength);
    if (c.strength <= c.level*0.8 - 6) {c.trait[0] = 1;}
    if (c.dexterity <= c.level*0.8 - 6) {c.trait[1] = 1;}
    if (c.fortitude <= c.level*0.8 - 6) {c.trait[2] = 1;}
    if (c.agility <= c.level*0.8 - 6) {c.trait[3] = 1;}
    if (c.intellect <= c.level*0.8 - 6) {c.trait[4] = 1;}
    if (c.strength >= c.level*1.25 + 3) {c.trait[5] = 1;}
    if (c.dexterity >= c.level*1.25 + 3) {c.trait[6] = 1;}
    if (c.fortitude >= c.level*1.25 + 3) {c.trait[7] = 1;}
    if (c.agility <= c.level*0.8 - 6) {c.trait[8] = 1;}
    if (c.intellect >= c.level*1.25 + 3) {c.trait[9] = 1;}
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
