#ifndef _INVENTORY_H
#define _INVENTORY_H
#include <iostream>
#include <string>
#include <cstdlib>
#include <vector>
#include <ctime>
#include <memory>
#include <fstream>
#include <ctype.h>

#include "rapidxml/rapidxml.hpp"
#include "rapidxml/rapidxml_print.hpp"
#include "Termcolor/termcolor.hpp"

#define MIN_PLAYER_EXIT_DISTANCE 50
#define ROOMS_NUMBER 20
#define MAX_ROOM_HEIGHT 15
#define MAX_ROOM_WIDTH 15
#define MIN_ROOM_WIDTH 5
#define MIN_ROOM_HEIGHT 5
#define MAX_X_VALUE 84 //MAX_X_VALUE = MAX_MATRIX_WIDTH - MAX_ROOM_WIDTH -1
#define MAX_Y_VALUE 84
#define MIN_X_VALUE 1
#define MIN_Y_VALUE 1
#define MAX_MATRIX_WIDTH 100
#define MAX_MATRIX_HEIGHT 100
#define MAP_WIDTH 81
#define MAP_HEIGHT 21
#define PLAYER_SIGHT 10
#define MIN_ENEMIES_NUMBER 1
#define MAX_ENEMIES_NUMBER 2
#define MIN_ITEMS_NUMBER 1
#define MAX_ITEMS_NUMBER 4
#define MIN_CHESTS_NUMBER 1
#define MAX_CHESTS_NUMBER 1
#define MIN_GP_LOOT_NUMBER 1
#define MAX_GP_LOOT_NUMBER 15
#define MIN_KEYS_LOOT_NUMBER 1
#define MAX_KEYS_LOOT_NUMBER 2
#define CHESTS_CHANCE 3 // 1/CHESTs_CHANCE that there is a chest in the room
#define MIN_CHEST_GOLD 50
#define MAX_CHEST_GOLD 300
#define MIN_CHEST_KEYS 2
#define MAX_CHEST_KEYS 3

extern std::string history;

class Effect{
    private:
    std::string stat;
    float value;

    public:
    Effect(std::string stat, float value);
    std::string getStat();
    float getValue();
};

class SelfEffect{
    private:
    std::string stat;
    float pot;
    std::string potStat;

    public:
    SelfEffect(std::string stat, float pot, std::string potStat);
    std::string getStat();
    float getPot();
    std::string getPotStat();
};

class InventoryElement{
    protected:
    int x;
    int y;
    bool isChest;
    bool isEquipped;
    bool isIdentified;
    bool isIdentifiable;
    std::string label;
    std::string type;
    std::string ch;
    std::vector<Effect> effects;

    public:
    InventoryElement(unsigned int x, unsigned int y, std::string label, std::string type, std::string ch, std::vector<Effect> effects);
    InventoryElement(unsigned int x, unsigned int y, std::string label, std::string type, std::string ch);
    InventoryElement(InventoryElement& element, unsigned int, unsigned int y);

    void setX(unsigned int x);
    void setY(unsigned int y);
    void setChest(bool input); //Set if the item is a chest
    void setEquipped(bool input);
    void setIdentified(bool input);
    void setIsIdentifiable(bool input);

    unsigned int getX() const;
    unsigned int getY() const;
    bool getIsChest() const;
    bool getIsEquipped() const;
    bool getIsIdentified() const;
    bool getIsIdentifiable() const;
    std::string getLabel() const;
    std::string getType() const;
    std::string getCh() const;
    std::vector<Effect> getEffects() const;

    void addEffect(Effect effect);
    std::string getDescription();
};

class Square{
    private:
    int fd; //Forward distance (where the player directed the attack)
    int rd; //Right distance (next direction clockwise)
    float pot;
    std::string stat;

    public:
    Square(int fd, int rd, std::string stat, float pot);

    int getFd() const;
    int getRd() const;
    float getPot() const;
    std::string getStat() const;
};

class Weapon : public InventoryElement{
    private:
    unsigned int durability;
    unsigned int range;
    std::vector<Square> areasOfEffect;

    public:
    Weapon(unsigned int x, unsigned int y, std::string label, std::string type, std::string ch, unsigned int durability, unsigned int range);
    Weapon(Weapon& w, unsigned int x, unsigned int y);

    unsigned int getDurability() const;
    unsigned int getRange() const;
    std::vector<Square> getAreasOfEffect() const;

    void addAreasOfEffect(Square area);
    void setDurability(unsigned int n);

    std::string getDescription();
};

class Scroll : public InventoryElement{
    private:
    unsigned int range;
    float mpCost;
    std::vector<Square> areasOfEffect;
    std::vector<SelfEffect> selfEffects;

    public:
    Scroll(unsigned int x, unsigned int y, std::string label, std::string type, std::string ch, unsigned int range, float mpCost);
    Scroll(Scroll& s, unsigned int x, unsigned int y);
    unsigned int getRange() const;
    float getMpCost() const;
    std::vector<SelfEffect> getSelfEffects() const;
    std::vector<Square> getAreasOfEffect() const;

    void addSelfEffect(SelfEffect s);
    void addAreasOfEffect(Square s);

    std::string getDescription();
};
bool areStringsEqual(std::string a, std::string b);

#endif