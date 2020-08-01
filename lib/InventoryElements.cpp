#include "InventoryElements.h"
std::string history = "";

//EFFECT SECTION
Effect::Effect(std::string stat, float value) : stat(stat), value(value){}

std::string Effect::getStat(){return stat;}
float Effect::getValue(){return value;}

//----------------------------------------------------------------------------------------------------

//SELFEFFECT SECTION
SelfEffect::SelfEffect(std::string stat, float pot, std::string potStat){
    this->stat = stat;
    this->pot = pot;
    this->potStat = potStat;
}

std::string SelfEffect::getStat(){return this->stat;}
float SelfEffect::getPot(){return this->pot;}
std::string SelfEffect::getPotStat(){return this->potStat;}

//----------------------------------------------------------------------------------------------------

//INVENTORYELEMENT SECTION
InventoryElement::InventoryElement(unsigned int x, unsigned int y, std::string label, std::string type, std::string ch, std::vector<Effect> effects){
    this->label = label;
    this->type = type;
    this->ch = ch;
    this->x = x;
    this->y = y;
    this->isChest = false;
    this->isEquipped = false;
    this->isIdentified = false;
    this->isIdentifiable = true;

    std::vector<Effect> temp = effects;
    std::vector<Effect>::iterator it = temp.begin();
    std::vector<Effect>::iterator end = temp.end();
    for(it; it != end; it++){
        this->effects.push_back(*it);
    }
}

InventoryElement::InventoryElement(unsigned int x, unsigned int y, std::string label, std::string type, std::string ch){
    this->label = label;
    this->type = type;
    this->ch = ch;
    this->x = x;
    this->y = y;
    this->isChest = false;
    this->isEquipped = false;
    this->isIdentified = false;
    this->isIdentifiable = true;
}

InventoryElement::InventoryElement(InventoryElement& element, unsigned int x, unsigned int y){
    this->label = element.getLabel();
    this->type = element.getType();
    this->ch = element.getCh();
    this->x = x;
    this->y = y;
    this->isChest = false;
    this->isEquipped = false;
    this->isIdentified = false;
    this->isIdentifiable = true;
    
    std::vector<Effect> temp = element.getEffects();
    std::vector<Effect>::iterator it = temp.begin();
    std::vector<Effect>::iterator end = temp.end();
    for(it; it != end; it++){
        this->effects.push_back(*it);
    }
}

void InventoryElement::setX(unsigned int x){this->x = x;}
void InventoryElement::setY(unsigned int y){this->y = y;}
void InventoryElement::setChest(bool input){this->isChest = input;}
void InventoryElement::setEquipped(bool input){this->isEquipped = input;}
void InventoryElement::setIdentified(bool input){this->isIdentified = input;}
void InventoryElement::setIsIdentifiable(bool input){this->isIdentifiable = input;}

bool InventoryElement::getIsChest() const{return this->isChest;}
bool InventoryElement::getIsEquipped() const{return this->isEquipped;}
bool InventoryElement::getIsIdentified() const{return this->isIdentified;}
bool InventoryElement::getIsIdentifiable() const{return this->isIdentifiable;}
unsigned int InventoryElement::getX() const{return this->x;}
unsigned int InventoryElement::getY() const{return this->y;}
std::string InventoryElement::getLabel() const{return this->label;}
std::string InventoryElement::getType() const{return this->type;}
std::string InventoryElement::getCh() const{return this->ch;}
std::vector<Effect> InventoryElement::getEffects() const{return effects;}

void InventoryElement::addEffect(Effect effect){effects.push_back(effect);}
std::string InventoryElement::getDescription(){
    std::string description = "";
    description += "\nThe item is a " +  this->type + ".";
    if(this->effects.size() > 0){
        std::vector<Effect>::iterator it = this->effects.begin();
        std::vector<Effect>::iterator end = this->effects.end();
        description += " It has has the following effects:";
        for(it; it != end; it++){
            description += "\nStat: " + (*it).getStat() + "   Bonus: " + std::to_string((int)((*it).getValue()));
        }
    }
    return description;
}

//----------------------------------------------------------------------------------------------------
//SQUARE SECTION
Square::Square(int fd, int rd, std::string stat, float pot) : fd(fd), rd(rd), pot(pot), stat(stat){}

int Square::getFd() const{return this->fd;}
int Square::getRd() const{return this->rd;}
float Square::getPot() const{return this->pot;}
std::string Square::getStat() const{return this->stat;}

//----------------------------------------------------------------------------------------------------
//WEAPON SECTION
Weapon::Weapon(unsigned int x, unsigned int y, std::string label, std::string type, std::string ch, unsigned int durability, unsigned int range) : InventoryElement(x, y, label, type, ch){
    this->durability = durability;
    this->range = range;
}

Weapon::Weapon(Weapon& w, unsigned int x, unsigned int y) : InventoryElement(x, y, w.getLabel(), w.getType(), w.getCh(), w.getEffects()){
    this->durability = w.getDurability();
    this->range = w.getRange();

    std::vector<Square> temp = w.getAreasOfEffect();
    std::vector<Square>::iterator it = temp.begin();
    std::vector<Square>::iterator end = temp.end();
    for(it; it != end; it++){
        this->areasOfEffect.push_back(*it);
    }
}

unsigned int Weapon::getDurability() const{return this->durability;}
unsigned int Weapon::getRange() const{return this->range;}
std::vector<Square> Weapon::getAreasOfEffect() const{return this->areasOfEffect;}

void Weapon::addAreasOfEffect(Square area){areasOfEffect.push_back(area);}
void Weapon::setDurability(unsigned int n){this->durability = n;}

std::string Weapon::getDescription(){
    std::string description = InventoryElement::getDescription();
    if(this->areasOfEffect.size() > 1)
        description += "\nThe weapon has AOE effects.";
    else if(this->areasOfEffect.size() == 1)
        description += "\nThe weapon hits only one enemy.";
    return description;
}

//----------------------------------------------------------------------------------------------------
//SCROLL SECTION

Scroll::Scroll(unsigned int x, unsigned int y, std::string label, std::string type, std::string ch, unsigned int range, float mpCost) : InventoryElement(x, y, label, type, ch){
    this->range = range;
    this->mpCost = mpCost;
}

Scroll::Scroll(Scroll& s, unsigned int x, unsigned int y) : InventoryElement(x, y, s.getLabel(), s.getType(), s.getCh()){
    this->range = s.getRange();
    this->mpCost = s.getMpCost();

    //Copy AOEs
    std::vector<Square> temp = s.getAreasOfEffect();
    std::vector<Square>::iterator it = temp.begin();
    std::vector<Square>::iterator end = temp.end();
    for(it; it != end; it++){
        this->areasOfEffect.push_back(*it);
    }
    //Copy self effects
    std::vector<SelfEffect> temp2 = s.getSelfEffects();
    std::vector<SelfEffect>::iterator it2 = temp2.begin();
    std::vector<SelfEffect>::iterator end2 = temp2.end();
    for(it2; it2 != end2; it2++){
        this->selfEffects.push_back(*it2);
    }
}

float Scroll::getMpCost() const{return this->mpCost;}
unsigned int Scroll::getRange() const{return this->range;}
std::vector<SelfEffect> Scroll::getSelfEffects() const{return this->selfEffects;}
std::vector<Square> Scroll::getAreasOfEffect() const{return this->areasOfEffect;}

void Scroll::addSelfEffect(SelfEffect s){this->selfEffects.push_back(s);}
void Scroll::addAreasOfEffect(Square s){this->areasOfEffect.push_back(s);}

std::string Scroll::getDescription(){
    std::string description = InventoryElement::getDescription();
    if(this->areasOfEffect.size() > 1)
        description += "\nThe spell has AOE effects.";
    else if(this->areasOfEffect.size() == 1)
        description += "\nThe spell hits only one enemy.";

    if(this->selfEffects.size() > 0){
        std::vector<SelfEffect>::iterator it = this->selfEffects.begin();
        std::vector<SelfEffect>::iterator end = this->selfEffects.end();
        description += "\nThe scroll has the following self effects:";
        for(it; it != end; it++){
            description += "\nStat: " + (*it).getStat() + "   Bonus/malus: " + std::to_string((int)((*it).getPot())) + " (+" + (*it).getPotStat() + ")";
        }
    }

    return description;
}

//----------------------------------------------------------------------------------------------------

bool areStringsEqual(std::string a, std::string b){
    if(a.length() != b.length())
        return false;
    else{
        for(int i = 0; i != a.length(); i++){
            if(toupper(a[i]) != toupper(b[i])){
                return false;
            }
        }
    }
    return true;
}