#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <algorithm>
#include <cstdio>
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
    int strength;
    int agility;
    int fortitude;
    int health;
    int damage;
} emptyItem;

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
    bool trait[10] = { 0,0,0,0,0,0,0,0,0,0 };

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

    int damage;
    int maxHealth;

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

bool strEquals( const std::string& str1, const std::string& str2 ) {
    std::string str1Cpy( str1 );
    std::string str2Cpy( str2 );
    std::transform( str1Cpy.begin(), str1Cpy.end(), str1Cpy.begin(), ::tolower );
    std::transform( str2Cpy.begin(), str2Cpy.end(), str2Cpy.begin(), ::tolower );
    return ( str1Cpy == str2Cpy );
}


int getMaxHealth()
{
    return player.fortitude*3 + 1;
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
    if(player.x < 3)
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
    if(player.y < 3)
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

void actionMove(string dir)
{
    while(true)
    {
        if(dir == "north" || dir == "n")
        {
            playerMoveNorth();
        }
        else if(dir == "south" || dir == "s")
        {
            playerMoveSouth();
        }
        else if(dir == "east" || dir == "e")
        {
            playerMoveEast();
        }
        else if(dir == "west" || dir == "w")
        {
            playerMoveWest();
        }
        else
        {
            cout << "Invalid input. Please enter a cardinal direction to move in: ";
            dir = getInput();
            continue;
        }
        break;
    }
}

void inspectSelf()
{
    printf("-- Stats --\n");
    printf("Fortitude:%4i    Health: %02i/%02i %s\n", player.fortitude, player.health, player.maxHealth, getHealthBar(player.health, player.maxHealth).c_str());
    printf("Strength:%5i    Damage:%6i\n", player.strength, player.damage);
    printf("Agility:%6i\n", player.agility);

    cout << "Items: ";
    for(int i = 0; i < 16; i++)
    {
        if(player.items[i].name.length() > 0)
        {
            if(i > 0)
            {
                cout << ", ";
            }
            cout << player.items[i].name;
        }
    }
}

void inspectRoom()
{
    cout << "Items: ";
    for(int i = 0; i < 16; i++)
    {
        if(map[player.y][player.x].items[i].name.length() > 0)
        {
            if(i > 0)
            {
                cout << ", ";
            }
            cout << map[player.y][player.x].items[i].name;
        }
    }
}

void actionInspect(string inspection)
{
    if(inspection == "self")
    {
        inspectSelf();
    }
    else if(inspection == "room")
    {
        inspectRoom();
    }
}

void actionPickup(string name)
{
    for(int i = 0; i < 16; i++)
    {
        if(strEquals(map[player.y][player.x].items[i].name, name))
        {
            for(int j = 0; j < 16; j++)
            {
                if(player.items[j].name.length() == 0)
                {
                    cout << "Picked up " << name << endl;
                    player.items[j] = map[player.y][player.x].items[i];
                    map[player.y][player.x].items[i] = emptyItem;
                    break;
                }
                if(j == 15)
                {
                    cout << "Your inventory is full" << endl;
                }
            }
            break;
        }
    }
}

void actionDrop(string name)
{
    for(int i = 0; i < 16; i++)
    {
        if(strEquals(player.items[i].name, name))
        {
            for(int j = 0; j < 16; j++)
            {
                if(map[player.y][player.x].items[j].name.length() == 0)
                {
                    cout << "Dropped " << name << endl;
                    map[player.y][player.x].items[j] = player.items[i];
                    player.items[i] = emptyItem;
                    break;
                }
                if(j == 15)
                {
                    cout << "The room is full, you can not drop your item." << endl;
                }
            }
            break;
        }
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

void setupMap()     //item starting locations
{
    item knife;
    knife.name = "Knife";
    knife.damage = 1;
    map[0][0].items[0] = knife;

    item leatherArmor;
    leatherArmor.name = "Leather Armor";
    leatherArmor.health = 5;
    map[1][1].items[0] = leatherArmor;

    item shield;
    shield.name = "Shield";
    shield.health = 3;
    map[2][1].items[0] = shield;
    map[1][2].items[0] = shield;

    item ironArmor;
    ironArmor.name = "Iron Armor";
    ironArmor.health = 10;
    map[2][2].items[0] = ironArmor;

    item sword;
    sword.name = "Sword";
    sword.damage = 5;
    map[3][2].items[0] = sword;

    item axe;
    axe.name = "Axe";
    axe.damage = 5;
    map[2][3].items[0] = axe;
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
    player.maxHealth = getMaxHealth();
    player.health = player.maxHealth;
    setupMap();

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
    }
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

void applyTraits(character c)
{
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

    if(input.substr(0, 8) == "inspect ")
    {
        actionInspect(input.substr(8));
    }
    else if(input.substr(0, 5) == "move ")
    {
        actionMove(input.substr(5));
    }
    else if(input.substr(0, 7) == "pickup ")
    {
        actionPickup(input.substr(7));
    }
    else if(input.substr(0, 5) == "drop ")
    {
        actionDrop(input.substr(5));
    }
    else if(input == "quit" || input == "q")
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
