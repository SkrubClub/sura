#include <algorithm>
#include <chrono>
#include <cstdio>
#include <iostream>
#include <math.h>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <thread>
#include <time.h>

using std::chrono::milliseconds;
using std::cin;
using std::cout;
using std::endl;
using std::getline;
using std::max;
using std::min;
using std::size_t;
using std::string;
using std::stringstream;
using std::this_thread::sleep_for;
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
    string description;
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
    int power;
    void (*interact)(object &);
};

void printObject(object obj)
{
    cout << obj.name << endl << obj.description << endl;
}

struct playercharacter //this represents the character; there is only ever one instance of it
{
    bool isDead = false;

    int level = 1;
    int freePoints = 0; // 12 total
    // Attributes
    int strength = 4;
    int dexterity = 4;
    int fortitude = 4;
    int agility = 4;
    int intellect = 2;
    int wisdom = 2;
    int karma = 0;
    int xpStr;
    int xpDex;
    int xpFor;
    int xpAgi;
    int xpInt;
    int xpWis;

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

    /*

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

void damagePlayer(int damage)
{
    player.health -= damage;
    if(player.health <= 0)
    {
        player.health = 0;
        player.isDead = true;
        shouldQuitGame = true;
    }
}

struct enemy
{
    string name;
    int damage;
    int maxHealth;
    int health;
    bool first = false;
} emptyEnemy;

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

void printEnemy(enemy en)
{
    cout << en.name << ": " << en.damage << " damage, " << getHealthBar(en.health, en.maxHealth) << endl;
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

void wait()
{
    string s;
    getline(cin, s);
}

bool strEquals(const std::string& str1, const std::string& str2) //returns whether the two strings are equal, IGNORING CASE
{
    string str1Cpy(str1);
    string str2Cpy(str2);
    transform(str1Cpy.begin(), str1Cpy.end(), str1Cpy.begin(), ::tolower);
    transform(str2Cpy.begin(), str2Cpy.end(), str2Cpy.begin(), ::tolower);
    return (str1Cpy == str2Cpy);
}


int getMaxHealth() //calculates the players max health
{
    return player.fortitude * 3 + 1;
}

struct room
{
    bool isRevealed = false;
    item items[16];
    object objects[16];
    enemy enemies[16];
} map[4][4];

bool doorH[4][3] = {
    {true , false, true },
    {true , true , true },
    {false, false, true },
    {true , true , true }
};
bool doorV[3][4] = {
    {true , true , true , false},
    {false, true , false, true },
    {false, true , true , true }
};

void printMap(bool allVision)
{
    char result[9][13];
    for(int y = 0; y < 9; y++)
    {
        for(int x = 0; x < 13; x++)
        {
            result[y][x] = ':';
        }
    }

    for(int y = 0; y < 4; y++)
    {
        for(int x = 0; x < 4; x++)
        {
            if(map[y][x].isRevealed || allVision)
            {
                bool doorN = (y == 0 || !doorV[y - 1][x]);
                bool doorS = (y == 3 || !doorV[y][x]);
                bool doorE = (x == 3 || !doorH[y][x]);
                bool doorW = (x == 0 || !doorH[y][x - 1]);
                bool isPlayer = x == player.x && y == player.y;

                result[y * 2    ][x * 3    ] = '+';
                result[y * 2    ][x * 3 + 1] = doorN ? '-' : ' ';
                result[y * 2    ][x * 3 + 2] = doorN ? '-' : ' ';
                result[y * 2    ][x * 3 + 3] = '+';
                result[y * 2 + 1][x * 3    ] = doorW ? '|' : ' ';
                result[y * 2 + 1][x * 3 + 1] = isPlayer ? '@' : ' ';
                result[y * 2 + 1][x * 3 + 2] = isPlayer ? '@' : ' ';
                result[y * 2 + 1][x * 3 + 3] = doorE ? '|' : ' ';
                result[y * 2 + 2][x * 3    ] = '+';
                result[y * 2 + 2][x * 3 + 1] = doorS ? '-' : ' ';
                result[y * 2 + 2][x * 3 + 2] = doorS ? '-' : ' ';
                result[y * 2 + 2][x * 3 + 3] = '+';
            }
        }
    }

    for(int y = 0; y < 9; y++)
    {
        for(int x = 0; x < 13; x++)
        {
            cout << result[y][x];
        }
        cout << endl;
    }
}

void playerMoveNorth()
{
    if(player.y > 0 && doorV[player.y - 1][player.x])
    {
        player.y--;
        cout << "You moved north" << endl;
    }
    else
    {
        cout << "You cannot move north" << endl;
    }
}

void playerMoveEast()
{
    if(player.x < 3 && doorH[player.y][player.x])
    {
        player.x++;
        cout << "You moved east" << endl;
    }
    else
    {
        cout << "You cannot move east" << endl;
    }
}

void playerMoveSouth()
{
    if(player.y < 3 && doorV[player.y][player.x])
    {
        player.y++;
        cout << "You moved south" << endl;
    }
    else
    {
        cout << "You cannnot move south" << endl;
    }
}

void playerMoveWest()
{
    if(player.x > 0 && doorH[player.y][player.x - 1])
    {
        player.x--;
        cout << "You moved west" << endl;
    }
    else
    {
        cout << "You cannot move west" << endl;
    }
}

string getAction()
{
    cout << "What is your action this turn? ";
    return getInput();
}

void printStats()
{
    cout << "Your current stats are:" << endl;
    cout << "Strength: " << player.strength << endl;
    cout << "Dexterity: " << player.dexterity << endl;
    cout << "Fortitude: " << player.fortitude << endl;
    cout << "Agility: " << player.agility << endl;
    cout << "Unallocated points: " << player.freePoints << endl;
    cout << "Allocated points: " << player.strength+player.dexterity+player.fortitude+player.agility << endl;
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
    statInput = getInt();
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

bool isBelowMin(int level, int stat)
{
    return stat <= level * 0.8 - 6;
}

bool isAboveMax(int level, int stat)
{
    return stat >= level * 1.25 + 3;
}

void applyTraits()
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

void interactATM(object &ATM)
{
    string input;
    int random;
    char pi[18] = {'3','.','1','4','1','5','9','2','6','5','3','5','8','9','7','9','3','2'};

    if(1==1)
    {
        cout << "Hello! I am an Automated Telling Machine, or \'ATM\' for short." << endl;
        cout << "I am an invaluable resource at your disposal, standing at the ready to elucidate your surroundings at a moment's notice!" << endl;
        cout << "My heart's only desire is to explain. Please, ask me a question!" << endl;
    }
    input = getInput();
    if(input.at(0)!=' ')
    {
        srand(time(NULL));
        random = rand() % 4;
        if(random==0)
        {
            cout << "Why indeed, my friend. Why indeed." << endl;
        } else
        if(random==1)
        {
            cout << "\"Time I'm done?\" Why, done explaining how to use me, of course!" << endl;
        } else
        if(random==2)
        {
            cout << "Want to hear a knock knock joke?" << endl;
            getInput();
            cout << "OK, here goes:" << endl << endl;
            cout << "Knock knock" << endl;
            cout << "Open up the door" << endl;
            cout << "It's real" << endl;
            cout << "With the nonstop pop-pop" << endl;
            cout << "And stainless steel" << endl;
        }
        if(random==3)
        {
            for(int i = 0; i < 18; i++)
            {
                cout << pi[i] << endl;
            }
        }
    }
}

void interactHappyStatue(object &sadStatue)
{
    int randomMessage;
    randomMessage = rand() % 1000;
    cout << "It seems happy, somehow." << endl;
    if(randomMessage == 0)
    {
        cout << "\"Thank you\"" << endl;
    }
}

void interactSadStatue(object &sadStatue)
{
    int randomMessage;
    randomMessage = rand() % 100;
    cout << "It seems sad." << endl;
    if(randomMessage == 0)
    {
        cout << "Comfort the statue?" << endl;
        if(getYesNo())
        {
            cout << "The tears cease." << endl;
            player.wisdom++;
            sadStatue.name = "Statue";
            sadStatue.description = "A statue of a child.";
            sadStatue.interact = &interactHappyStatue;
        }
    }
}

void interactDoctorIt(object &doctorIt)
{
    int random;
    string animal[12] = {"Ram","Bull","Twin","Crab","Lion","Maiden","Scale","Scorpion","Centaur","Sea-Goat","Water-Bearer","Fish"};
    random = rand() % 12;

    player.health = player.fortitude*3 + 1;
    cout << "All done! You look healthy as a " << animal[random] << endl;
}

void interactGRI(object &GRI)
{
    int loopLen = 1;

    cout << "Genetic Reconstitution Interface booting..." << endl;
    cout << "GRI at ";
    for(float i = 0; i <= 100; i++)
    {
        if(i >= 10)
        {
            loopLen = 2;
        }
        if(i >= 100)
        {
            loopLen = 3;
        }
        cout << i << "% power, ";
        if(i < 100)
        {
            cout << "charging...";
            for(int i2 = 0; i2 < 20+loopLen; i2++)
            {
                cout << '\b';
            }
        }
        if(i == 100)
        {
            cout << "launching..." << endl;
            break;
        }
        sleep_for(milliseconds(20));
    }

    string strAdj[6] = {"sexy","devilish","enchanting","fascinating","spellbinding","majestic"};
    string strNoun[7] = {"bad boy","wolf in sheep's clothing","baby kangaroo","siren","angel","mystery figure","minx"};
    int randomAdj = rand() % 6;
    int randomNoun = rand() % 7;

    printStats();
    allocateStatPoints(false);
    applyTraits();
    printStats();
    cout << "Enjoy your new body, you " << strAdj[randomAdj] << " " << strNoun[randomNoun] << "." << endl;
}

void interactHoop(object &hoop)
{
    cout << "You're too short, kid." << endl;
}

void interactJoint(object &joint)
{
    cout << "It's a joint!" << endl;
}

void interactPit(object &pit)
{
    long randomDist = rand() % 98999 + 1000;
    long fallDist;
    int distLen = 0;
    int dots = 1;

    cout << endl << "You gaze into the abyss";
    for(int i = 0; i < 3; i++)
    {
        sleep_for(milliseconds(1000));
        cout << ".";
    }
    sleep_for(milliseconds(1000));
    cout << endl << "   It gazes into you";
    for(int i = 0; i < 3; i++)
    {
        sleep_for(milliseconds(1000));
        cout << ".";
    }
    sleep_for(milliseconds(1000));

    cout << endl << endl << "Suddenly, you lose your balance and fall in!" << endl;
    cout << "You feel as if you've fallen approximately ";
    for(fallDist = 1; fallDist < randomDist; fallDist++)
    {
        if(fallDist > 0) {distLen = 1;}
        if(fallDist > 9) {distLen = 2;}
        if(fallDist > 99) {distLen = 3;}
        if(fallDist > 999) {distLen = 4;}
        if(fallDist > 9999) {distLen = 5;}
        if(fallDist > 99999) {distLen = 6;}
        if(fallDist > 999999) {distLen = 7;}
        cout << fallDist << " metres.";
        if(fallDist != randomDist)
        {
            for(int rep = 0; rep < distLen+8; rep++)
            {
                cout << '\b';
            }
        }
    }
    cout << fallDist << " metres." << endl << endl;

    int fallTimeSec = floor(fallDist/10);
    int fallTimeMin = floor(fallTimeSec/60);
    fallTimeSec -= fallTimeMin*60;
    cout << "You die on impact, having fallen for ";
    if(fallTimeMin == 0)
    {
        cout << fallTimeSec << " seconds.";
    } else
    {
        cout << fallTimeMin << " minutes and " << fallTimeSec << " seconds." << endl;
    }
    cout << "You're the second person to die like this today.";
}

void interactPitSign(object &pitSign)
{
    cout << "+----------------+" << endl;
    cout << "|     Warning!   |" << endl;
    cout << "|                |" << endl;
    cout << "| We don't really|" << endl;
    cout << "|  know how far  |" << endl;
    cout << "| this pit goes. |" << endl;
    cout << "|    So yeah.    |" << endl;
    cout << "|    Maybe be    |" << endl;
    cout << "|    careful?    |" << endl;
    cout << "|      idk       |" << endl;
    cout << "|                |" << endl;
    cout << "|      -Nietzsche|" << endl;
    cout << "+----------------+" << endl;
}

void interactPunchingBag(object &punchingBag)
{
    int randomDir;
    int randomMod;
    string swingDirection[8] = {"to the left.","diagonally left and backwards.","backwards.","diagonally right and backwards.","to the right.","diagonally right and forwards.","forwards and hits you in the face!","diagonally left and forwards."};
    string swingMod[10] = {"lackadaisically","apathetically","vigorously","energetically","excitedly","happily","tragically","boredly","balefully","rapidly"};
    randomDir = rand() % 8;
    randomMod = rand() % 10;
    cout << "Hit the punching bag?" << endl;
    if(getYesNo())
    {
        cout << "The bag swings " << swingMod[randomMod] << " " << swingDirection[randomDir] << endl;
        if(randomDir == 6)
        {
            player.health = max(player.health-((player.strength/2)+1),1);
        }
    }
}

void interactTotallyNormalWallN(object &totallyNormalWallN)
{
    cout << "That wasn't so hard." << endl;
    player.y -= 1;
}

void interactTotallyNormalWallS(object &totallyNormalWallS)
{
    cout << "That wasn't so hard." << endl;
    player.y += 1;
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

    cout << endl << "Objects: ";
    for(int i = 0; i < 16; i++)
    {
        if(map[player.y][player.x].objects[i].name.length() > 0)
        {
            if(i > 0)
            {
                cout << ", ";
            }
            cout << map[player.y][player.x].objects[i].name;
        }
    }

    cout << endl << "Enemies: ";
    for(int i = 0; i < 16; i++)
    {
        if(map[player.y][player.x].enemies[i].name.length() > 0)
        {
            if(i > 0)
            {
                cout << ", ";
            }
            cout << map[player.y][player.x].enemies[i].name;
        }
    }
    cout << endl;
}

//      _        _   _
//     / \   ___| |_(_) ___  _ __  ___
//    / _ \ / __| __| |/ _ \| '_ \/ __|
//   / ___ \ (__| |_| | (_) | | | \__ \
//  /_/   \_\___|\__|_|\___/|_| |_|___/
//

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
    map[player.y][player.x].isRevealed = true;
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
        bool thingFound = false;
        for(int i = 0; i < 16; i++) //items in inventory
        {
            if(strEquals(player.items[i].name, inspection))
            {
                printItem(player.items[i]);
                thingFound = true;
            }
        }

        for(int j = 0; j < 16; j++) //items in room
        {
            if(strEquals(map[player.y][player.x].items[j].name, inspection))
            {
                printItem(map[player.y][player.x].items[j]);
                thingFound = true;
            }
        }

        for(int j = 0; j < 16; j++) //objects in room
        {
            if(strEquals(map[player.y][player.x].objects[j].name, inspection))
            {
                printObject(map[player.y][player.x].objects[j]);
                thingFound = true;
            }
        }

        for(int j = 0; j < 16; j++) //enemies in room
        {
            if(strEquals(map[player.y][player.x].enemies[j].name, inspection))
            {
                printEnemy(map[player.y][player.x].enemies[j]);
                thingFound = true;
            }
        }

        if(!thingFound)
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
                    return;
                }
            }
            cout << "Your inventory is full." << endl;
            return;
        }
    }
    cout << "No item \"" << name << "\" found in this room." << endl;
}

bool actionDrop(string name) //drops the given item from inventory to the room. returns whether it successfully dropped the item
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
                    return true;
                }
            }
            cout << "The room is full, you can not drop your item." << endl;
            return false;
        }
    }
    cout << "No item \"" << name << "\" found in your inventory." << endl;
}

void actionInteract(string name) //interacts with the oject of the given name
{
    for(int i = 0; i < 16; i++)
    {
        if(strEquals(map[player.y][player.x].objects[i].name, name))
        {
            map[player.y][player.x].objects[i].interact(map[player.y][player.x].objects[i]);
            return;
        }
    }
    cout << "You stand alone, surroundings barren of anything helpful." << endl;
}

void actionMap()
{
    printMap(false);
}

void actionNothing()
{
    cout << "You did nothing." << endl;
}

void actionQuit()
{
    cout << "Are you sure you want to quit? ";
    shouldQuitGame = getYesNo();
}

void printHelp() //prints help menu
{
    cout << "At the beginning of your turn you have any of the following actions:" << endl << endl;
    cout << "Movement:" << endl << "\"Move North\" or \"Move N\"." << endl << "\"Move East\" or \"Move E\"." << endl << "\"Move South\" or \"Move S\"." << endl << "\"Move West\" or \"Move W\"." << endl << endl;
    cout << "Inspection:" << endl << "\"Inspect Room\" (Inspects room for items, objects, and enemies)." << endl << "\"Inspect Self\" (Shows player inventory and stats)" << endl << endl;
    cout << "Picking up and dropping items:" << endl << "\"PickUp (item name)\"." << endl << "\"Drop (item name)\"." << endl << endl;
    cout << "Doing nothing (idle) for your turn:" << endl << "\"Nothing\"" << endl << endl;
    cout << "Quitting game:" <<  endl << "\"Quit\"" << endl << endl;
    wait();
}

void setupMap() //fills the rooms with items, objects, and enemies
{
    map[player.y][player.x].isRevealed = true;

    //items

    item knife;
    knife.name = "Knife";
    knife.damage = 1;
    knife.description = "A short blade.";
    map[0][0].items[0] = knife;

    item leatherArmor;
    leatherArmor.name = "Leather Armor";
    leatherArmor.health = 5;
    knife.description = "Decently protective (and smelly!) armor.";
    map[1][1].items[0] = leatherArmor;

    item shield;
    shield.name = "Shield";
    shield.health = 3;
    knife.description = "The best offence is not getting killed.";
    map[2][1].items[0] = shield;
    map[1][2].items[0] = shield;

    item ironArmor;
    ironArmor.name = "Iron Armor";
    ironArmor.health = 10;
    knife.description = "How do you walk in this?";
    map[2][2].items[0] = ironArmor;

    item sword;
    sword.name = "Sword";
    sword.damage = 5;
    knife.description = "A long blade.";
    map[3][2].items[0] = sword;

    item axe;
    axe.name = "Axe";
    axe.damage = 5;
    knife.description = "A brutish blade.";
    map[2][3].items[0] = axe;

    item telepot;
    telepot.name = "Telepot";
    telepot.damage = 0;
    knife.description = "Blaze through time and space, man";
    map[3][0].items[0] = telepot;

    //objects

    object atm;
    atm.name = "ATM";
    atm.description = "Doesn't look the way you remember.";
    atm.power = 0;
    atm.interact = &interactATM;
    map[0][0].objects[0] = atm;

    object sadStatue;
    sadStatue.name = "Crying Statue";
    sadStatue.description = "A statue of a child. Water pours from its eyes.";
    sadStatue.power = 0;
    sadStatue.interact = &interactSadStatue;
    map[3][1].objects[0] = sadStatue;

    object doctorIt;
    doctorIt.name = "DoctorIt";
    doctorIt.description = "Probably from a diploma factory.";
    doctorIt.power = 0;
    doctorIt.interact = &interactDoctorIt;
    map[0][3].objects[0] = doctorIt;

    object gri;
    gri.name = "Genetic Reconstitution Interface";
    gri.description = "A mysterious machine.";
    gri.power = 0;
    gri.interact = &interactGRI;
    map[3][3].objects[0] = gri;

    object hoop;
    hoop.name = "Hoop";
    hoop.description = "Fibrous material, woven into a mesh, affixed to a metal torus, welded to a plastic board, supported by a formidable cylinder.";
    hoop.power = 0;
    hoop.interact = &interactHoop;
    map[2][3].objects[0] = hoop;

    object joint;
    joint.name = "Joint";
    joint.description = "How morbid.";
    joint.power = 0;
    joint.interact = &interactJoint;
    map[3][0].objects[1] = joint;

    object pit;
    pit.name = "Pit";
    pit.description = "ooooo spoopy";
    pit.power = 0;
    pit.interact = &interactPit;
    map[1][2].objects[1] = pit;

    object pitSign;
    pitSign.name = "Danger Sign";
    pitSign.description = "It seems to be trying to tell you something.";
    pitSign.power = 0;
    pitSign.interact = &interactPitSign;
    map[1][2].objects[0] = pitSign;

    object punchingBag;
    punchingBag.name = "Punching Bag";
    punchingBag.description = "From the tangles of my heart...";
    punchingBag.power = 0;
    punchingBag.interact = &interactPunchingBag;
    map[1][1].objects[0] = punchingBag;

    object totallyNormalWallN;
    totallyNormalWallN.name = "Totally Normal Wall";
    totallyNormalWallN.description = "A Totally Normal Wall.";
    totallyNormalWallN.power = 0;
    totallyNormalWallN.interact = &interactTotallyNormalWallN;
    map[3][0].objects[0] = totallyNormalWallN;

    object totallyNormalWallS;
    totallyNormalWallS.name = "Totally Normal Wall";
    totallyNormalWallS.description = "A Totally Normal Wall.";
    totallyNormalWallS.power = 0;
    totallyNormalWallS.interact = &interactTotallyNormalWallS;
    map[2][0].objects[0] = totallyNormalWallS;

    //enemies

    enemy worm;
    worm.name = "Worm";
    worm.damage = 0;
    worm.maxHealth = 5;
    worm.health = worm.maxHealth;
    map[0][1].enemies[0] = worm;

    enemy worm2;
    worm2.name = "Grub";
    worm2.damage = 0;
    worm2.maxHealth = 5;
    worm2.health = worm2.maxHealth;
    map[0][2].enemies[0] = worm2;

    enemy worm3;
    worm3.name = "Caterpiller";
    worm3.damage = 0;
    worm3.maxHealth = 5;
    worm3.health = worm3.maxHealth;
    map[2][1].enemies[0] = worm3;

    enemy worm4;
    worm4.name = "Worm";
    worm4.damage = 0;
    worm4.maxHealth = 5;
    worm4.health = worm4.maxHealth;
    map[2][2].enemies[0] = worm4;

    enemy worm5;
    worm5.name = "Grub";
    worm5.damage = 0;
    worm5.maxHealth = 5;
    worm5.health = worm5.maxHealth;
    map[2][2].enemies[1] = worm5;

    enemy worm6;
    worm6.name = "Worm";
    worm6.damage = 0;
    worm6.maxHealth = 5;
    worm6.health = worm6.maxHealth;
    map[3][3].enemies[0] = worm6;

    enemy worm7;
    worm7.name = "Grub";
    worm7.damage = 0;
    worm7.maxHealth = 5;
    worm7.health = worm7.maxHealth;
    map[3][3].enemies[1] = worm7;

    enemy worm8;
    worm8.name = "Caterpillar";
    worm8.damage = 0;
    worm8.maxHealth = 5;
    worm8.health = worm8.maxHealth;
    map[3][3].enemies[2] = worm8;

    enemy worm9;
    worm9.name = "Inch Worm";
    worm9.damage = 0;
    worm9.maxHealth = 5;
    worm9.health = worm9.maxHealth;
    map[3][3].enemies[3] = worm9;
}

void setup() //the function called when starting a new game
{
    srand(time(NULL));
    shouldQuitGame = false;
    player.maxHealth = getMaxHealth();
    player.health = player.maxHealth;
    player.damage = player.strength / 2;
    setupMap();

    cout << "Welcome to Sura!" << endl << endl;
    cout << "We have given you balanced starting stats." << endl << "Would you like to continue without customizing them? ";
    while (!getYesNo())
    {
        resetStatPoints();
        allocateStatPoints(true);
        printStats();
        cout << endl;

        cout << "Would you like to proceed? ";
    }
}

void fight(enemy &e) //fights the given enemy
{
    cout << "Entering fight with " << e.name << endl;
    while(true)
    {
        cout << "Do you want to attack or flee? ";
        string input = getInput();
        if(strEquals(input, "attack") || strEquals(input, "a"))
        {
            e.health -= player.damage;
            if(e.health <= 0)
            {
                cout << e.name << " has died" << endl;
                e.health = 0;
                e = emptyEnemy;
                return;
            }
            else
            {
                cout << "You hit for " << player.damage << " damage. " << e.name << " is at " << e.health << "/" << e.maxHealth << " health." << endl;
            }
        }
        else if(strEquals(input, "flee") || strEquals(input, "f"))
        {
            cout << "Select item to throw at enemy: ";
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
            if(actionDrop(getInput()))
            {
                cout << "You fled the battle" << endl;
                return;
            }
            else
            {
                cout << "You cannot drop that item" << endl;
            }
        }
        else
        {
            cout << "\"Ha! You can't even type correctly!\"" << endl;
        }

        int damageDealt = e.damage;
        damagePlayer(damageDealt);
        cout << e.name << " hit you for " << e.damage << " damage. You are at " << player.health << "/" << player.maxHealth << " health." << endl;
        if(player.health <= 0)
        {
            cout << "\"I warned you, but you wouldn't listen. Maybe next time...\"" << endl;
        }
    }
}

void actionFight(string e) //fights the enemy with the given name
{
    for(int i = 0; i < 16; i++)
    {
        if(strEquals(map[player.y][player.x].enemies[i].name, e))
        {
            cout << "\"Are you sure you want to battle me?\" ";
            if(getYesNo())
            {
                cout << "\"Don't say I didn't warn you!\"" << endl;
                fight(map[player.y][player.x].enemies[i]);
            }
            else
            {
                cout << "\"What a coward!\"" << endl;
            }
            return;
        }
    }
    cout << "No enemy with that name found" << endl;
}

void mainLoop() //called to start the player's turn
{
    cout << endl;
    string input = getAction();

    if(input.substr(0, 8) == "inspect ")
    {
        actionInspect(input.substr(8));
    }
    else if(input.substr(0, 6) == "fight ")
    {
        actionFight(input.substr(6));
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
    else if(input.substr(0, 9) == "interact ")
    {
        actionInteract(input.substr(9));
    }
    else if(input == "map")
    {
        actionMap();
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

void die()
{
    string skull[] = {
            "         _,.-------.,_",
            "     ,;~'             '~;,",
            "   ,;                     ;,",
            "  ;                         ;",
            " ,'                         ',",
            ",;                           ;,",
            "; ;      .           .      ; ;",
            "| ;   ______       ______   ; |",
            "|  `/~\"     ~\" . \"~     \"~\'  |",
            "|  ~  ,-~~~^~, | ,~^~~~-,  ~  |",
            " |   |        }:{        |   |",
            " |   l       / | \\       !   |",
            " .~  (__,.--\" .^. \"--.,__)  ~.",
            " |     ---;' / | \\ `;---     |",
            "  \\__.       \\/^\\/       .__/",
            "   V| \\                 / |V",
            "    | |T~\\___!___!___/~T| |",
            "    | |`IIII_I_I_I_IIII'| |",
            "    |  \\,III I I I III,/  |",
            "     \\   `~~~~~~~~~~'    /",
            "       \\   .       .   /",
            "         \\.    ^    ./",
            "           ^~~~^~~~^"
        };

    sleep_for(milliseconds(500));

    for(int i = 0; i < 23; i++)
    {
        cout << "        " << skull[i] << endl;
        sleep_for(milliseconds(200));
    }

    sleep_for(milliseconds(1000));

    cout << endl << endl <<
"         @@@@@@@      @@@     @@@@@@@ " << endl <<
"         @@@@@@@@     @@@     @@@@@@@@" << endl <<
"         @@!  @@@     @@!     @@!  @@@" << endl <<
"         !@!  @!@     !@!     !@!  @!@" << endl <<
"         @!@!!@!      !!@     @!@@!@! " << endl <<
"         !!@!@!       !!!     !!@!!!  " << endl <<
"         !!: :!!      !!:     !!:     " << endl <<
"         :!:  !:!     :!:     :!:     " << endl <<
"         ::   :::      ::      ::     " << endl <<
"          :   : :     :        :      " << endl;

    sleep_for(milliseconds(3000));
    cout << endl << endl << endl << endl << endl << endl << endl << endl << endl << endl << endl;
}

void endgame() //the function called when exiting to main menu
{
    if(player.isDead)
    {
        die();
    }
}


void mainMenu() //the function called when going to main menu
{
    int delay = 200;

    cout << endl; sleep_for(milliseconds(100));
    cout << "     @@@@@@   @@@  @@@  @@@@@@@    @@@@@@ " << endl; sleep_for(milliseconds(delay));
    cout << "    @@@@@@@   @@@  @@@  @@@@@@@@  @@@@@@@@" << endl; sleep_for(milliseconds(delay));
    cout << "    !@@       @@!  @@@  @@!  @@@  @@!  @@@" << endl; sleep_for(milliseconds(delay));
    cout << "    !@!       !@!  @!@  !@!  @!@  !@!  @!@" << endl; sleep_for(milliseconds(delay));
    cout << "    !!@@!!    @!@  !@!  @!@!!@!   @!@!@!@!" << endl; sleep_for(milliseconds(delay));
    cout << "     !!@!!!   !@!  !!!  !!@!@!    !!!@!!!!" << endl; sleep_for(milliseconds(delay));
    cout << "         !:!  !!:  !!!  !!: :!!   !!:  !!!" << endl; sleep_for(milliseconds(delay));
    cout << "        !:!   :!:  !:!  :!:  !:!  :!:  !:!" << endl; sleep_for(milliseconds(delay));
    cout << "    :::: ::   ::::: ::  ::   :::  ::   :::" << endl; sleep_for(milliseconds(delay));
    cout << "    :: : :     : :  :    :   : :   :   : :" << endl;
    cout << endl;

    sleep_for(milliseconds(delay * 3));

    cout << "                 ~Play  Game~             " << endl; sleep_for(milliseconds(delay * 3));
    cout << "                    ~Help~                " << endl; sleep_for(milliseconds(delay * 3));
    cout << "                  ~Credits~               " << endl; sleep_for(milliseconds(delay * 3));
    cout << "                 ~Quit  Sura~             " << endl << endl;

    string input = getInput();

    if(input == "play game" || input == "p")
    {
        setup();
        while(!shouldQuitGame)
        {
            mainLoop();
        }
        endgame();
    }
    else if(input == "help" || input == "h")
    {
        printHelp();
    }
    else if(input == "credits" || input == "c")
    {
        cout << "TGwTH: Coding" << endl;
        cout << "     : Coding" << endl;
        cout << "TK301: Coding" << endl;
        wait();
    }
    else if(input == "quit sura" || input == "q")
    {
        cout << "Are you sure you want to quit? ";
        shouldQuitSura = getYesNo();
        return;
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
