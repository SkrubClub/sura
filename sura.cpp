#include <algorithm>
#include <cstdio>
#include <iostream>
#include <math.h>
#include <sstream>
#include <stdio.h>
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
    int power;
    void (*interact)(object &);
};

struct playercharacter //this represents the character; there is only ever one instance of it
{
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
    for(int i = 0; i < 101; i++)
    {
        if(i >= 10)
        {
            loopLen = 2;
        }
        if(i == 100)
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
    cout << endl;
    cout << "Objects: ";
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

void actionInteract(string name)
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

void actionNothing()
{
    cout << "You did nothing" << endl;
}

void actionQuit()
{
    cout << "Are you sure you want to quit? ";
    shouldQuitGame = getYesNo();
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
}

void setup() //the function called when starting a new game
{
    srand(time(NULL));
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
        printStats();
        cout << endl;

        cout << "Would you like to proceed? ";
    }
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
    else if(input.substr(0, 9) == "interact ")
    {
        actionInteract(input.substr(9));
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
    cout <<
                                                        endl <<
        "     @@@@@@   @@@  @@@  @@@@@@@    @@@@@@ " << endl <<
        "    @@@@@@@   @@@  @@@  @@@@@@@@  @@@@@@@@" << endl <<
        "    !@@       @@!  @@@  @@!  @@@  @@!  @@@" << endl <<
        "    !@!       !@!  @!@  !@!  @!@  !@!  @!@" << endl <<
        "    !!@@!!    @!@  !@!  @!@!!@!   @!@!@!@!" << endl <<
        "     !!@!!!   !@!  !!!  !!@!@!    !!!@!!!!" << endl <<
        "         !:!  !!:  !!!  !!: :!!   !!:  !!!" << endl <<
        "        !:!   :!:  !:!  :!:  !:!  :!:  !:!" << endl <<
        "    :::: ::   ::::: ::  ::   :::  ::   :::" << endl <<
        "    :: : :     : :  :    :   : :   :   : :" << endl <<
                                                        endl <<
        "                 ~Play  Game~             " << endl <<
        "                    ~Help~                " << endl <<
        "                  ~Credits~               " << endl <<
        "                 ~Quit  Sura~             " << endl <<
                                                        endl;

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
