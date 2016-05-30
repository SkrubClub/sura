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
    int level = 1;
    int freePoints = 0; // 12 total
    // Attributes
    int strength = 4;
    int dexterity = 4;
    int fortitude = 4;
    int agility = 4;
    int intellect = 2;
        //int wisdom = 2;
    float luck = 0;   // Random number from 0-1;
        //int karma = 0;

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
    bool oath[10] = {0,0,0,0,0,0,0,0,0,0};
    /*
    0 = Oath of The Gentle
    1 = Oath of The Aimless
    2 = Oath of The Frail
    3 = Oath of The Still
    4 = Oath of The Mindless
    5 = Oath of The Naive
    6 = Oath of The Cursed
    7 = Oath of The Blessed
    8 = Oath of The Pacifist
    9 = Oath of The Crusader
    */

    /* Perks

    Experiencedplayer.
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

    int x;
    int y;
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
    return player.fortitude*3 + 1;
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

void resetStatPoints()
{
    player.freePoints += player.strength;
    player.freePoints += player.dexterity;
    player.freePoints += player.fortitude;
    player.freePoints += player.agility;

    player.strength = 0;
    player.dexterity = 0;
    player.fortitude = 0;
    player.agility = 0;
}

void allocateStat(string name, int& stat, bool startup)
{
    int statInput;
    int prevAmnt;

    cout << "How many points would you like in " << name << "?" << endl;
    cin >> statInput;
    cin.sync();
    prevAmnt = stat;

    if(startup)
    {
        stat = min(statInput, min(player.freePoints, 5));
    }
    else
    {
        stat = min(statInput, player.freePoints);
    }

    player.freePoints -= (stat - prevAmnt);
}

void allocateStatPoints(bool startup)
{
    allocateStat("strength", player.strength, startup);
    allocateStat("dexterity", player.dexterity, startup);
    allocateStat("fortitude", player.fortitude, startup);
    allocateStat("agility", player.agility, startup);
}

void setup()
{
    shouldExit = false;

    cout << "Welcome to Sura!" << endl << endl;
    cout << "Your current stats are:" << endl;
    cout << "Strength: " << player.strength << endl;
    cout << "Dexterity: " << player.dexterity << endl;
    cout << "Fortitude: " << player.fortitude << endl;
    cout << "Agility: " << player.agility << endl;
    cout << "Would you like to proceed?" << endl;
    while (!getYesNo())
    {
        resetStatPoints();
        allocateStatPoints(true);
        cout << "Your current stats are:" << endl;
        cout << "Strength: " << player.strength << endl;
        cout << "Dexterity: " << player.dexterity << endl;
        cout << "Fortitude: " << player.fortitude << endl;
        cout << "Agility: " << player.agility << endl;
        cout << "Unallocated points: " << player.freePoints << endl;
        cout << "Allocated points: " << player.strength+player.dexterity+player.fortitude+player.agility << endl << endl;

        cout << "Would you like to proceed?" << endl;
        //getInput();
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

bool isAboveMax(int level, int stat)
{
    return stat >= level * 1.25 + 3;
}

/*

int isAboveMax(int level, int stat)
{
    if (stat < level*2)
    {
        int threshold = 10;

        while(stat > level*(1+(1/(threshold-1))))
        {
            threshold -= 1;
        } else
        {
            return 11-threshold;
        }
    }
}

*/

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

    player.trait[0] = isBelowMin(player.level, player.strength);
    player.trait[1] = isBelowMin(player.level, player.dexterity);
    player.trait[2] = isBelowMin(player.level, player.fortitude);
    player.trait[3] = isBelowMin(player.level, player.agility);
    player.trait[4] = isAboveMax(player.level, player.strength);
    player.trait[5] = isAboveMax(player.level, player.dexterity);
    player.trait[6] = isAboveMax(player.level, player.fortitude);
    player.trait[7] = isAboveMax(player.level, player.agility);
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
