#include <algorithm>
#include <cstdio>
#include <iostream>
#include <sstream>
#include <stdlib.h>
#include <string>
#include <time.h>

using std::cin;
using std::cout;
using std::endl;
using std::getline;
using std::max;
using std::min;
using std::size_t;
using std::string;
using std::stringstream;
using std::transform;

bool shouldQuitGame;
bool shouldQuitSura;

struct item
{
    string name;
    int strength = 0;
    int agility = 0;
    int fortitude = 0;
    int health = 0;
    int damage = 0;
} emptyItem;

void printItem(item it) //prints out the details of the given item
{
    cout << it.name << endl;
    if(it.strength > 0)
    {
        cout << "Strength: +" << it.strength;
    }
    if(it.agility > 0)
    {
        cout << "Agility: +" << it.agility;
    }
    if(it.fortitude > 0)
    {
        cout << "Fortitude: +" << it.fortitude;
    }
    if(it.health > 0)
    {
        cout << "Health: +" << it.health;
    }
    if(it.damage > 0)
    {
        cout << "Damage: +" << it.damage;
    }
}

struct object
{
    string name;
    string description;
    void (*interact)();
};

struct playercharacter //this represents the character; there is oly ever one instance of it
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

string getHealthBar(int health, int maxHealth) //gets a string that visually represents a healthbar
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

string getInput() //gets a line of text that the user enters and returns that line, lowercase
{
    string input;
    getline(cin, input);
    transform(input.begin(), input.end(), input.begin(), ::tolower);
    return input;
}

int getInt() //gets an integer from the user
{
    string answer;
    while(true)
    {
        answer = getInput();
        stringstream ss(answer);
        int i;

        if(ss >> i)
        {
            return i;
        }

        cout << "Invalid input. Please enter an integer: ";
    }
}

bool getYesNo() //returns true/false depending on whether the user inputs positively or negtively
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

bool strEquals(const std::string& str1, const std::string& str2) //returns whether the two strings are equal, IGNORING CASE
{
    std::string str1Cpy(str1);
    std::string str2Cpy(str2);
    std::transform(str1Cpy.begin(), str1Cpy.end(), str1Cpy.begin(), ::tolower);
    std::transform(str2Cpy.begin(), str2Cpy.end(), str2Cpy.begin(), ::tolower);
    return (str1Cpy == str2Cpy);
}


int getMaxHealth() //calculates the players max health
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
        cout << "You are already as far north as you can move" << endl;
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
        cout << "You are already as far east as you can move" << endl;
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
        cout << "You are already as far south as you can move" << endl;
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
        cout << "You are already as far west as you can move" << endl;
    }
}

void actionMove(string dir) //moves the player in the direction specified by dir
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
            cout << "Invalid input.  Please enter a cardinal direction to move in (North/N, East/E, South/S, West/W)." << endl;
            dir = getInput();
            continue;
        }
        break;
    }
}

void inspectSelf() //prints the details of the player
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

void inspectRoom() //pritns the details of the current room
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

void actionInspect(string inspection) //prints the details of the thing specified by "inspection"
{
    if(inspection == "self") //inspect self
    {
        inspectSelf();
    }
    else if(inspection == "room") //inspect room
    {
        inspectRoom();
    }
    else if(inspection.length() > 0) //assume inspect item
    {
        bool itemFound = false;
        for(int i = 0; i < 16; i++)
        {
            if(strEquals(player.items[i].name, inspection))
            {
                printItem(player.items[i]);
                itemFound = true;
            }
        }

        for(int j = 0; j < 16; j++)
        {
            if(strEquals(map[player.y][player.x].items[j].name, inspection))
            {
                printItem(map[player.y][player.x].items[j]);
                itemFound = true;
            }
        }
        if(!itemFound)
        {
            cout << "Cannot inspect " << inspection << endl;
        }
    }
    else
    {
        cout << "Cannot inspect " << inspection << endl;
    }
}

void actionPickup(string name) //picks up the given item from the room
{
    for(int i = 0; i < 16; i++)
    {
        if(strEquals(map[player.y][player.x].items[i].name, name))
        {
            for(int j = 0; j < 16; j++)
            {
                if(player.items[j].name.length() == 0)
                {
                    cout << "Picked up:" << endl;
                    printItem(map[player.y][player.x].items[i]);
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

void actionDrop(string name) //drops the given item from inventory to the room
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
    shouldQuitGame = getYesNo();
}

string getAction()
{
    cout << "What is your action this turn? ";
    return getInput();
}

void setupMap() //fills the rooms with items, objects, and enemies
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

void resetStatPoints() //sets the players stats to 0, and puts the points back into the unallocated pool
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

void allocateStat(string name, int& stat, bool startup) //asks the player to move points from "unallocated" to the given variable
{
    cout << "You have " << player.freePoints << " points left. How many would you like in " << name << "? ";

    int prevAmnt = stat;
    int statInput = getInt();

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

void allocateStatPoints(bool startup) //asks the player to allocate their unallocated stat points
{
    allocateStat("strength", player.strength, startup);
    allocateStat("dexterity", player.dexterity, startup);
    allocateStat("fortitude", player.fortitude, startup);
    allocateStat("agility", player.agility, startup);
    cout << endl;
}

void setup() //the function called when starting a new game
{
    shouldQuitGame = false;
    player.maxHealth = getMaxHealth();
    player.health = player.maxHealth;
    setupMap();

    cout << "Welcome to Sura!" << endl << endl;
    cout << "We have given you balanced starting stats." << endl << "Would you like to continue without customizing them? ";
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

        cout << "Would you like to proceed? ";
    }
}

void addStatPoints(int &stat, int amount)
{

}

bool isBelowMin(int level, int stat) //checks if the player should have a trait
{
    return stat <= level * 0.8 - 6;
}

bool isAboveMax(int level, int stat) //checks if the player should have a trait
{
    return stat >= level * 1.25 + 3;
}

void applyTraits(character c) //sets traits to what they should be
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

void printHelp()
{
    cout << "At the beginning of your turn you have any of the following actions:" << endl << endl;
    cout << "Movement:" << endl << "\"Move North\" or \"Move N\"." << endl << "\"Move East\" or \"Move E\"." << endl << "\"Move South\" or \"Move S\"." << endl << "\"Move West\" or \"Move W\"." << endl << endl;
    cout << "Inspection:" << endl << "\"Inspect Room\" (Inspects room for items, objects, and enemies)." << endl << "\"Inspect Self\" (Shows player inventory and stats)" << endl << endl;
    cout << "Picking up and dropping items:" << endl << "\"PickUp (item name)\"." << endl << "\"Drop (item name)\"." << endl << endl;
    cout << "Doing nothing (idle) for your turn:" << endl << "\"Nothing\"" << endl << endl;
    cout << "Quitting game:" <<  endl << "\"Quit\"" << endl << endl;
}

void mainLoop() //the function called at the beginning of the player's turn
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
    else if(input == "help" || input == "h")
    {
        printHelp();
    }
    else
    {
        cout << "Invalid input \"" << input << "\"" << endl;
        actionNothing();
    }
}

void endgame() //the function called when exiting to main menu
{
    
}

void mainMenu() //the function called when going to main menu
{
    cout << "Welcome to Sura!" << endl << endl;
    cout << "   ~Play Game~" << endl << "      ~Help~" << endl << "         ~Credits~" << endl << "            ~Quit Sura~" << endl;
    
    
    string input = getInput();
    
    if(input == "play game")
    {
        setup();
        while(!shouldQuitGame)
        {
            mainLoop();
        }
    }
    else if(input == "help")
    {
        printHelp();
    }
    else if(input == "credits")
    {
        cout << "Two guys went over to a skrub's house and decided to learn C++.  The two guys and the one skrub then decided to form SkrubClub, and to make the project, sura, to better learn C++." << endl;
    }
    else if(input == "quit sura")
    {
        shouldQuitSura = true;
        endgame();
    }
    else
    {
        cout << "Invalid input \"" << input << "\"" << endl;
    }
}

int main()
{
    shouldQuitSura = false;
    while(!shouldQuitSura)
    {
        mainMenu();
    }

    return 0;
}
