#ifndef _CHARACTERS_H
#define _CHARACTERS_H
#include "InventoryElements.h"

class Character{
    protected:
    std::string label;
    float hp;
    float maxHp;
    float str;
    float dex;
    float mnd;
    float wis;
    float res;
    float movTime;
    float actTime;
    unsigned int x;
    unsigned int y;
    std::string ch;

    protected:
    unsigned int movementIntention; //0=no movement 1=up 2=down 3=left 4=right
    
    public:
    
    Character(unsigned int x, unsigned int y, float maxHP, float str, 
    float dex, float mnd, float wis, float res, 
    float movTime, float actTime, std::string ch);

    int getMovementIntention() const;
    void resetMovementIntention();
    void setCoordinates(const int x, const int y);

    std::string getLabel() const;
    unsigned int getX() const;
    unsigned int getY() const;
    float getStat(std::string stat) const;
    int abilityCheck(std::string stat, unsigned int cd);
    float getHP() const;
    float getMaxHP() const;
    float getStr() const;
    float getDex() const;
    float getMnd() const;
    float getWis() const;
    float getRes() const;
    float getMovTime() const;
    float getActTime() const;
    std::string getCh() const;

    bool abilityCheck(std::string );

    bool takeDamage(std::string attackerLabel, unsigned int damage); //False = character died
    void healDamage(unsigned int heal);
    void applySelfEffect(SelfEffect e);
    void applyEffect(Effect e);
};

class Player : public Character{
    private:
    unsigned int lvl;
    unsigned int mp;
    unsigned int maxMp;
    unsigned int exp;
    unsigned int keys;
    unsigned int gp;
    float upHpMax;
    unsigned int upMpMax;
    float upStr;
    float upDex;
    float upMnd;
    float upWis;
    float upRes;
    float upMovT;
    float upActT;
    std::string playerClass;
    std::vector<std::unique_ptr<InventoryElement>> inventoryElements;

    public:
    Player(std::string label, unsigned int x, unsigned int y, unsigned int maxMp, float maxHP, float str, 
    float dex, float mnd, float wis, float res, 
    float movTime, float actTime, std::string playerClass, std::string ch);
    
    int getEquippedWeaponIndex(); //-1 = none
    int getEquippedItemIndex(); //-1 = none
    unsigned int getLvl() const;
    unsigned int getMP() const;
    unsigned int getMaxMP() const;
    unsigned int getExp() const;
    unsigned int getKeys() const;
    unsigned int getGp() const;
    unsigned int getInventorySize() const;
    std::string getPlayerClass() const;
    InventoryElement getInventoryElementAt(unsigned int n) const;
    Scroll getScrollAt(unsigned int n);
    Weapon getWeaponAt(unsigned int n);

    void setUpHpMax(float upHpMax);
    void setUpMpMax(unsigned int upMpMax);
    void setUpStr(float upStr);
    void setUpDex(float upDex);
    void setUpMnd(float upMnd);
    void setUpWis(float upWis);
    void setUpRes(float upRes);
    void setUpMovT(float upMovT);
    void setUpActT(float upActT);
    
    int input(const std::string command);
    void setMovementIntention(const std::string direction);
    void addInventoryElement(std::unique_ptr<InventoryElement>& element, bool viewInHistory);
    void addInventoryWeapon(Weapon& element, bool viewInHistory);
    void addInventoryScroll(Scroll& element, bool viewInHistory);
    void reduceWeaponDurability(unsigned int index);
    bool disequipItem(unsigned int index);
    bool equipItem(unsigned int index);
    bool discardItem(unsigned int index, bool check);
    bool identifyItem(unsigned int index);
    void addGold(unsigned int gold);
    void addExp(unsigned int exp);
    void addKeys(int keys);
    void setMp(unsigned int mp);
    void levelUp();

    unsigned int getAtkDamage(std::string stat, float pot) const;
    std::vector<Effect> getEquippedWeaponEffects();
    std::vector<Square> getEquippedWeaponAOE();
    
    std::vector<Square> getScrollAOEAt(unsigned int index);
    std::vector<SelfEffect> getScrollSelfEffectsAt(unsigned int index);
    
    void applySelfEffect(SelfEffect e);
    void applyEffect(Effect e);
};


class Enemy : public Character{
    private:
    float nextActTime;
    unsigned int attackRange;
    unsigned int sightRange;
    unsigned int exp;
    unsigned int gp;
    std::string attackStat;
    bool hasAttackedLastTurn;
    
    public:
    Enemy(unsigned int x, unsigned int y, unsigned int maxHP, float str, 
    float dex, float mnd, float wis, float res, 
    float movTime, float actTime, std::string attackStat, unsigned int attackRange, 
    unsigned int sightRange, unsigned int exp, unsigned int gp, std::string label, std::string ch);
    Enemy(const Enemy& e, unsigned int x, unsigned int y);

    void setNextActTime(float nextActTime);
    void setHasAttackedLastTurn(bool hasAttackedLastTurn);

    float getNextActTime() const;
    std::string getAttackStat() const;
    unsigned int getAttackRange() const;
    unsigned int getSightRange() const;
    unsigned int getExp() const;
    unsigned int getGp() const;
    unsigned int getAtkDamage();
    bool getHasAttackedLastTurn() const;
};

#endif