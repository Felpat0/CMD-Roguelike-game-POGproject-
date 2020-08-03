#include "Characters.h"

//----------------------------------------------------------------------------------------------------

//CHARACTER SECTION
Character::Character(unsigned int x, unsigned int y, float maxHP, float str, 
    float dex, float mnd, float wis, float res, 
    float movTime, float actTime, std::string ch){
        this->x = x;
        this->y = y;
        this->maxHp = maxHP;
        this->hp = maxHp;
        this->str = str;
        this->dex = dex;
        this->mnd = mnd;
        this->wis = wis;
        this->res = res;
        this->movTime = movTime;
        this->actTime = actTime;
        this->movementIntention = 0;
        this->ch = ch;
}

int Character::getMovementIntention() const {return this->movementIntention;}
void Character::resetMovementIntention(){this->movementIntention = 0;}

std::string Character::getLabel() const{return this->label;}
void Character::setCoordinates(const int x, const int y){
    this->x = x;
    this->y = y;
}
unsigned int Character::getX() const{return this->x;}
unsigned int Character::getY() const{return this->y;}
float Character::getStat(std::string stat) const{
    if(areStringsEqual(stat, "hp"))
        return this->hp;
    else if(areStringsEqual(stat, "maxHp")){
        return this->maxHp;
    }else if(areStringsEqual(stat, "str"))
        return this->str;
    else if(areStringsEqual(stat, "dex"))
        return this->dex;
    else if(areStringsEqual(stat, "mnd"))
        return this->mnd;
    else if(areStringsEqual(stat, "wis"))
        return this->wis;
    else if(areStringsEqual(stat, "res"))
        return this->res;
    else if(areStringsEqual(stat, "movT"))
        return this->movTime;
    else if(areStringsEqual(stat, "actT"))
        return this->actTime;
    return 0;
}
int Character::abilityCheck(std::string stat, unsigned int cd){
    int random = (rand() % 20) + 1;
    history += "\n" + this->label + " rolled " + std::to_string(random) + " (+" + std::to_string((int)getStat(stat)) + ").";
    return random + ((int)getStat(stat)) - cd;
}
float Character::getHP() const{return this->hp;};
float Character::getMaxHP() const{return this->maxHp;};
float Character::getStr() const{return this->str;}
float Character::getDex() const{return this->dex;}
float Character::getMnd() const{return this->mnd;}
float Character::getWis() const{return this->wis;}
float Character::getRes() const{return this->res;}
float Character::getMovTime() const{return this->movTime;}
float Character::getActTime() const{return this->actTime;}
std::string Character::getCh() const{return this->ch;}

bool Character::takeDamage(std::string attackerLabel, unsigned int damage){ //False = character died
    if(damage < 1)
        damage = 0;
    if(this->getHP() == 0){
        return false;
    }
    int damageTaken = (int)damage - (int)res;
    if(damageTaken < 0)
        damageTaken = 0;
    history += "\n" + attackerLabel + " damaged " + this->label + " for " + std::to_string(damageTaken) + " damage.";
    if(damageTaken > 0){
        if(damageTaken >= this->hp){
            this->hp = 0;
            history += "\n" + this->label + " has been defeated.";
            return false;
        }
        this->hp -= damageTaken;
    }
    return true;
}

void Character::healDamage(unsigned int heal){
    hp += heal;
    if(hp > maxHp){
        history += "\n" + this->label + " healed for " + std::to_string((int)(heal - (hp - maxHp))) + " hp";
        hp = maxHp;
    }else
        history += "\n" + this->label + " healed for " + std::to_string((int)(heal)) + " hp";
}

void Character::applySelfEffect(SelfEffect e){
    float value = ((this->getStat(e.getPotStat())) * e.getPot());
    if(areStringsEqual(e.getStat(), "hp")){
        if(value > 0)
            this->healDamage((int)value);
        else if(value < 0)
            this->takeDamage("Effect", abs((int)value));
    }else if(areStringsEqual(e.getStat(), "maxHp")){
        //If the player has full health, his current hp raise with the maxHp
        if(this->maxHp == this->hp)
            this->hp += (int)value;
        this->maxHp += (int)value;
        history += "\nMax hp of " + this->getLabel() + ": ";
        if(value > 0)
            history += "+";
        history += std::to_string((int)value);
    }else if(areStringsEqual(e.getStat(), "str")){
        this->str += value;
        history += "\nStrength (STR) of " + this->getLabel() + ": ";
        if(value > 0)
            history += "+";
        history += std::to_string(value);
    }else if(areStringsEqual(e.getStat(), "dex")){
        this->dex += value;
        history += "\nDexterity (DEX) of " + this->getLabel() + ": ";
        if(value > 0)
            history += "+";
        history += std::to_string(value);
    }else if(areStringsEqual(e.getStat(), "mnd")){
        this->mnd += value;
        history += "\nMind (MND) of " + this->getLabel() + ": ";
        if(value > 0)
            history += "+";
        history += std::to_string(value);
    }else if(areStringsEqual(e.getStat(), "wis")){
        this->wis += value;
        history += "\nWisdom (WIS) of " + this->getLabel() + ": ";
        if(value > 0)
            history += "+";
        history += std::to_string(value);
    }else if(areStringsEqual(e.getStat(), "res")){
        this->res += value;
        history += "\nResistance (RES) of " + this->getLabel() + ": ";
        if(value > 0)
            history += "+";
        history += std::to_string(value);
    }else if(areStringsEqual(e.getStat(), "movT")){
        this->movTime += value;
        history += "\nMove time (MovTime) of " + this->getLabel() + ": ";
        if(value > 0)
            history += "+";
        history += std::to_string(value);
    }else if(areStringsEqual(e.getStat(), "actT")){
        this->actTime += value;
        history += "\nAct time of " + this->getLabel() + ": ";
        if(value > 0)
            history += "+";
        history += std::to_string(value);
    }
}

void Character::applyEffect(Effect e){
    float value = e.getValue();
    if(areStringsEqual(e.getStat(), "hp")){
        if(value > 0)
            this->healDamage((int)value);
        else if(value < 0)
            this->takeDamage("Effect", abs((int)value));
    }else if(areStringsEqual(e.getStat(), "maxHp")){
        //If the player has full health, his current hp raise with the maxHp
        if(this->maxHp == this->hp)
            this->hp += (int)value;
        this->maxHp += (int)value;
        history += "\nMax hp of " + this->getLabel() + ": ";
        if(value > 0)
            history += "+";
        history += std::to_string((int)value);
    }else if(areStringsEqual(e.getStat(), "str")){
        this->str += value;
        history += "\nStrength (STR) of " + this->getLabel() + ": ";
        if(value > 0)
            history += "+";
        history += std::to_string(value);
    }else if(areStringsEqual(e.getStat(), "dex")){
        this->dex += value;
        history += "\nDexterity (DEX) of " + this->getLabel() + ": ";
        if(value > 0)
            history += "+";
        history += std::to_string(value);
    }else if(areStringsEqual(e.getStat(), "mnd")){
        this->mnd += value;
        history += "\nMind (MND) of " + this->getLabel() + ": ";
        if(value > 0)
            history += "+";
        history += std::to_string(value);
    }else if(areStringsEqual(e.getStat(), "wis")){
        this->wis += value;
        history += "\nWisdom (WIS) of " + this->getLabel() + ": ";
        if(value > 0)
            history += "+";
        history += std::to_string(value);
    }else if(areStringsEqual(e.getStat(), "res")){
        this->res += value;
        history += "\nResistance (RES) of " + this->getLabel() + ": ";
        if(value > 0)
            history += "+";
        history += std::to_string(value);
    }else if(areStringsEqual(e.getStat(), "movT")){
        this->movTime += value;
        history += "\nMove time (MovTime) of " + this->getLabel() + ": ";
        if(value > 0)
            history += "+";
        history += std::to_string(value);
    }else if(areStringsEqual(e.getStat(), "actT")){
        this->actTime += value;
        history += "\nAct time of " + this->getLabel() + ": ";
        if(value > 0)
            history += "+";
        history += std::to_string(value);
    }
}
//----------------------------------------------------------------------------------------------------

//PLAYER SECTION
Player::Player (std::string label, unsigned int x, unsigned int y, unsigned int maxMp, float maxHP, float str, 
    float dex, float mnd, float wis, float res, 
    float movTime, float actTime, std::string playerClass, std::string ch) :
    Character(x, y, maxHP, str, dex, mnd, wis, res, movTime, actTime, ch){
        this->lvl = 1;
        this->exp = 0;
        this->gp = 0;
        this->maxMp = maxMp;
        this->mp = maxMp;
        this->keys = 0;
        this->playerClass = playerClass;
        this->label = label;
}

int Player::getEquippedWeaponIndex(){
    std::vector<std::unique_ptr<InventoryElement>>::iterator it = inventoryElements.begin();
    std::vector<std::unique_ptr<InventoryElement>>::iterator end = inventoryElements.end();
    bool ok = true;
    for(it; it != end; it ++){
        if((**it).getIsEquipped() && areStringsEqual((**it).getType(), "weapon")){
            return it - inventoryElements.begin();
        }
    }
    return -1;
}

int Player::getEquippedItemIndex(){
    std::vector<std::unique_ptr<InventoryElement>>::iterator it = inventoryElements.begin();
    std::vector<std::unique_ptr<InventoryElement>>::iterator end = inventoryElements.end();
    bool ok = true;
    for(it; it != end; it ++){
        if((**it).getIsEquipped() && (areStringsEqual((**it).getType(), "herb") || areStringsEqual((**it).getType(), "potion"))){
            return it - inventoryElements.begin();
        }
    }
    return -1;
}

unsigned int Player::getLvl() const{return this->lvl;}
unsigned int Player::getMaxMP()const{return this->maxMp;}
unsigned int Player::getMP()const{return this->mp;}
unsigned int Player::getExp() const{return this->exp;}
unsigned int Player::getKeys() const{return this->keys;}
unsigned int Player::getGp() const{return this->gp;}
unsigned int Player::getInventorySize() const{return this->inventoryElements.size();}
std::string Player::getPlayerClass() const{return this->playerClass;}
InventoryElement Player::getInventoryElementAt(unsigned int n) const{return *(inventoryElements.at(n));}
Scroll Player::getScrollAt(unsigned int n){return *(static_cast<Scroll*>(inventoryElements.at(n).get()));}
Weapon Player::getWeaponAt(unsigned int n){return *(static_cast<Weapon*>(inventoryElements.at(n).get()));}

void Player::setUpHpMax(float upHpMax){this->upHpMax = upHpMax;}
void Player::setUpMpMax(unsigned int upMpMax){this->upMpMax = upMpMax;}
void Player::setUpStr(float upStr){this->upStr = upStr;}
void Player::setUpDex(float upDex){this->upDex = upDex;}
void Player::setUpMnd(float upMnd){this->upMnd = upMnd;}
void Player::setUpWis(float upWis){this->upWis = upWis;}
void Player::setUpRes(float upRes){this->upRes = upRes;}
void Player::setUpMovT(float upMovT){this->upMovT = upMovT;}
void Player::setUpActT(float upActT){this->upActT = upActT;}

int Player::input(std::string command){
    if(command == "w" || command == "a" || command == "s" || command == "d" || command == "W" || command == "A" || command == "S" || command == "D"){
        //Move
        setMovementIntention(command);
        return 1;
    }else if(command.length() == 7 && areStringsEqual(command.substr(0, 5), "equip") && isdigit(command[6])){
        //Equip
        return 2;
    }else if(command.length() == 5 && areStringsEqual(command.substr(0, 3), "atk") ){
        if(areStringsEqual("w", std::string(1, command[4])) || areStringsEqual("a", std::string(1, command[4]))
        || areStringsEqual("s", std::string(1, command[4])) || areStringsEqual("d", std::string(1, command[4]))){
            //Attack
            return 3;
        }
    }else if(command.length() == 3 && areStringsEqual(command.substr(0, 3), "use")){
        //Use item
        return 5;
    }else if(command.length() == 8 && areStringsEqual(command.substr(0, 4), "cast") && isdigit(command[5])){
        if(areStringsEqual("w", std::string(1, command[7])) || areStringsEqual("a", std::string(1, command[7]))
        || areStringsEqual("s", std::string(1, command[7])) || areStringsEqual("d", std::string(1, command[7]))){
            //Cast spell
            return 4;
        }
    }else if(command.length() == 4 && areStringsEqual(command.substr(0, 4), "take")){
        //Loot items
        return 6;
    }else if(command.length() == 9 && areStringsEqual(command.substr(0, 7), "discard") && isdigit(command[8])){
        //Discard item
        return 7;
    }else if(command.length() == 10 && areStringsEqual(command.substr(0, 8), "identify") && isdigit(command[9])){
        //Identify item
        return 8;
    }else if(command.length() == 6 && areStringsEqual(command.substr(0, 4), "open") ){
        if(areStringsEqual("w", std::string(1, command[5])) || areStringsEqual("a", std::string(1, command[5]))
        || areStringsEqual("s", std::string(1, command[5])) || areStringsEqual("d", std::string(1, command[5]))){
            //Open door/chest
            return 9;
        }
    }else if(command.length() == 9 && areStringsEqual(command.substr(0, 5), "range") && isdigit(command[6])){
        if(areStringsEqual("w", std::string(1, command[8])) || areStringsEqual("a", std::string(1, command[8]))
        || areStringsEqual("s", std::string(1, command[8])) || areStringsEqual("d", std::string(1, command[8]))){
            //Range objectIndex w/a/s/d
            return 10;
        }
    }
    return 0;
}

void Player::setMovementIntention(std::string direction){ 
    //0=no movement 1=up 2=down 3=left 4=right
    if(direction == "w" || direction == "W"){
            movementIntention = 1;
    }else if(direction == "a" || direction == "A"){
            movementIntention = 3;
    }else if(direction == "s" || direction == "S"){
            movementIntention = 2;
    }else if(direction == "d" || direction == "D"){
            movementIntention = 4;
    }
}

void Player::addInventoryElement(std::unique_ptr<InventoryElement>& element, bool viewInHistory){
    if(areStringsEqual(element->getType(), "gp")){
        if(viewInHistory)
            history += "\n" + this->getLabel() + " picked up " + element->getLabel() + " golds.";
        this->addGold(atoi(element->getLabel().c_str()));
    }else if(areStringsEqual(element->getType(), "key")){
        if(viewInHistory)
            history += "\n" + this->getLabel() + " picked up " + element->getLabel() + " keys.";
        this->addKeys(atoi(element->getLabel().c_str()));
    }else{
        bool identify = false;;
        element->setX(0);
        element->setY(0);
        if(areStringsEqual(element->getType(), "herb")){
            if(!element->getIsIdentified())
                identify = true;
            if(viewInHistory)
                history += "\n" + this->getLabel() + " picked up an herb.";
        }else{
            if(viewInHistory)
                history += "\n" + this->getLabel() + " picked up an item.";
        }   
        this->inventoryElements.push_back(std::move(element));  
        if(identify)
            identifyItem(inventoryElements.end() - inventoryElements.begin() - 1);   
    }
}

void Player::addInventoryWeapon(Weapon& element, bool viewInHistory){
    bool identify = false;
    element.setX(0);
    element.setY(0);
    if(areStringsEqual(element.getType(), "herb")){
        if(!element.getIsIdentified())
            identify = true;
        if(viewInHistory)
            history += "\n" + this->getLabel() + " picked up an herb.";
    }else{
        if(viewInHistory)
            history += "\n" + this->getLabel() + " picked up an item.";
    }
    this->inventoryElements.push_back(std::make_unique<Weapon>(element));  
}

void Player::addInventoryScroll(Scroll& element, bool viewInHistory){
    bool identify = false;
    element.setX(0);
    element.setY(0);
    if(viewInHistory)
        history += "\n" + this->getLabel() + " picked up an item.";
    std::unique_ptr<Scroll> t (std::make_unique<Scroll>(element));
    this->inventoryElements.push_back(std::move(t));  
}

void Player::reduceWeaponDurability(unsigned int index){
    int durability = static_cast<Weapon*>(inventoryElements.at(index).get())->getDurability();
    if(durability > 0){
        static_cast<Weapon*>(inventoryElements.at(index).get())->setDurability(durability - 1);
        if(static_cast<Weapon*>(inventoryElements.at(index).get())->getDurability() == 0){
            history += "\nThe weapon " +  inventoryElements.at(index)->getLabel() + " broke.";
            this->discardItem(index, false);
        }
    }
}

bool Player::disequipItem(unsigned int index){
    bool ok = true;
    if(this->inventoryElements.at(index)->getIsEquipped()){
        this->inventoryElements.at(index)->setEquipped(false);
        //Remove weapon/armor's effects
        if(areStringsEqual(this->inventoryElements.at(index)->getType(), "weapon") || areStringsEqual(this->inventoryElements.at(index)->getType(), "protection")){
            std::vector<Effect> temp = inventoryElements.at(index)->getEffects();
            std::vector<Effect>::iterator it2 = temp.begin();
            std::vector<Effect>::iterator end2 = temp.end();
            for(it2; it2 != end2; it2++){
                if(areStringsEqual(it2->getStat(), "hp"))
                    this->hp -= ((int)it2->getValue());
                else if(areStringsEqual(it2->getStat(), "maxHp")){
                    this->maxHp -= (int)it2->getValue();
                    if(this->hp > this->maxHp)
                        this->hp = this->maxHp;
                }else if(areStringsEqual(it2->getStat(), "mp"))
                    this->mp -= (int)it2->getValue();
                else if(areStringsEqual(it2->getStat(), "maxMp")){
                    this->maxMp -= (int)it2->getValue();
                    if(this->mp > this->maxMp)
                        this->mp = this->maxMp;
                }else if(areStringsEqual(it2->getStat(), "str"))
                    this->str -= it2->getValue();
                else if(areStringsEqual(it2->getStat(), "dex"))
                    this->dex -= it2->getValue();
                else if(areStringsEqual(it2->getStat(), "mnd"))
                    this->mnd -= it2->getValue();
                else if(areStringsEqual(it2->getStat(), "wis"))
                    this->wis -= it2->getValue();
                else if(areStringsEqual(it2->getStat(), "res"))
                    this->res -= it2->getValue();
                else if(areStringsEqual(it2->getStat(), "movT"))
                    this->movTime -= it2->getValue();
                else if(areStringsEqual(it2->getStat(), "actT"))
                    this->actTime -= it2->getValue();
            }
        }
        return true;
    }else
        return false;

}

bool Player::equipItem(unsigned int index){
    //If the index is not valid, return false
    if(index >= inventoryElements.size()){
        std::cout<<"\nThere is no item at this index. Press enter to continue.\n";
        fflush(stdin);
        system("pause");
        return false;
    }
    //If the item has not been identified, return false
    if(!this->inventoryElements.at(index)->getIsIdentified() && !areStringsEqual(this->inventoryElements.at(index)->getType(), "herb")){
        std::cout<<"\nThis item has to be identified first. Press enter to continue.\n";
        fflush(stdin);
        system("pause");
        return false;
    }
    //If the item is already equipped or can't be equipped, return false
    if(this->inventoryElements.at(index)->getIsEquipped() || areStringsEqual(this->inventoryElements.at(index)->getType(), "scroll")){
        std::cout<<"\nThe chosen item is already equipped or can't be equipped. Press enter to continue.\n";
        fflush(stdin);
        system("pause");
        return false;
    }
    if(areStringsEqual(this->inventoryElements.at(index)->getType(), "herb") && !this->inventoryElements.at(index)->getIsIdentified())
        history += "\n" + this->label + " equipped an unidentified herb.";
    else
        history += "\n" + this->label + " equipped " + this->inventoryElements.at(index)->getLabel() + ".";
    //Iterate over the inventory to check if an item of the same type is equipped
    //If it is, disequip it
    std::vector<std::unique_ptr<InventoryElement>>::iterator it = inventoryElements.begin();
    std::vector<std::unique_ptr<InventoryElement>>::iterator end = inventoryElements.end();
    for(it; it != end; it ++){
        if((**it).getType() == this->inventoryElements.at(index)->getType() || ((areStringsEqual((**it).getType(), "herb")|| areStringsEqual((**it).getType(), "potion")) && (areStringsEqual(this->inventoryElements.at(index)->getType(), "herb")|| areStringsEqual(this->inventoryElements.at(index)->getType(), "potion")))){
            disequipItem(it - inventoryElements.begin());
        }
    }
    //Equip item
    this->inventoryElements.at(index)->setEquipped(true);
    //Apply weapon/armor's effects
    if(areStringsEqual(inventoryElements.at(index)->getType(), "weapon") || areStringsEqual(inventoryElements.at(index)->getType(), "protection")){
        std::vector<Effect> temp = inventoryElements.at(index)->getEffects();
        std::vector<Effect>::iterator it2 = temp.begin();
        std::vector<Effect>::iterator end2 = temp.end();
        for(it2; it2 != end2; it2++){
            this->applyEffect(*it2);
        }
    }
    

    return true;
}

bool Player::discardItem(unsigned int index, bool check){
    if(inventoryElements.size() > index){
        if(inventoryElements.at(index)->getIsEquipped() && check){
            std::cout<<"\nYou can't discard an equipped item.\n";
            fflush(stdin);
            system("pause");
            return false;
        }
        inventoryElements.erase(inventoryElements.begin() + index);
        return true;
    }
    std::cout<<"\nThere is no item at this index.";
    fflush(stdin);
    system("pause");
    return false;
}

bool Player::identifyItem(unsigned int index){
    if(inventoryElements.size() <= index){
        std::cout<<"\nThe index is not valid. Press enter to continue.\n";
        fflush(stdin);
        system("pause");
        return false;
    }else if(inventoryElements.at(index)->getIsIdentified()){
        if(areStringsEqual(inventoryElements.at(index)->getType(), "weapon")){
            history += static_cast<Weapon*>(inventoryElements.at(index).get())->getDescription();
        }else if(areStringsEqual(inventoryElements.at(index)->getType(), "scroll")){
            history += static_cast<Scroll*>(inventoryElements.at(index).get())->getDescription();
        }else{
            history += this->inventoryElements.at(index)->getDescription();
        }
        return false;
    }else if(!inventoryElements.at(index)->getIsIdentifiable()){
        std::cout<<"\nThe item cannot be identified. Press enter to continue.\n";
        fflush(stdin);
        system("pause");
        return false;
    }
    int result = abilityCheck("wis", 15);
    if(result >= 0){
        //Succesfull ability check
        inventoryElements.at(index)->setIdentified(true);
        if(areStringsEqual(inventoryElements.at(index)->getType(), "weapon")){
            history += static_cast<Weapon*>(inventoryElements.at(index).get())->getDescription();
        }else if(areStringsEqual(inventoryElements.at(index)->getType(), "scroll")){
        history += static_cast<Scroll*>(inventoryElements.at(index).get())->getDescription();
        }else{
            history += this->inventoryElements.at(index)->getDescription();
        }
        
    }else if(result < -10){
        //Hard failure
        history += "\n" + this->label + " hard failed the identification, the item at index " + std::to_string(index) + " is no more identifiable.";
        inventoryElements.at(index)->setIsIdentifiable(false);
    }else{
        history += "\n" + this->label + " failed the identification of the item at index " + std::to_string(index) + ".";
    }
    return true;
}

void Player::addGold(unsigned int gold){this->gp += gold;}
void Player::addExp(unsigned int exp){
    this->exp += exp;
    //Level up
    while(this->exp >= this->lvl * 100){
        this->exp -= lvl*100;
        levelUp();
    }
}
void Player::addKeys(int keys){this->keys += keys;}
void Player::setMp(unsigned int mp){this->mp = mp;}
void Player::levelUp(){
    this->lvl += 1;
    this->hp += upHpMax;
    this->maxHp += this->upHpMax;
    this->mp += this->upMpMax;
    this->maxMp += this->upMpMax;
    this->str += this->upStr;
    this->dex += this->upDex;
    this->mnd += this->upMnd;
    this->wis += this->upWis;
    this->res += this->upRes;
    this->movTime += this->upMovT;
    this->actTime += this->upActT;
}

unsigned int Player::getAtkDamage(std::string stat, float pot) const{
    if(areStringsEqual(stat, "str"))
        return this->str * pot;
    if(areStringsEqual(stat, "dex"))
        return this->dex * pot;
    if(areStringsEqual(stat, "mnd"))
        return this->mnd * pot;
    else if(areStringsEqual(stat, "wis"))
        return this->wis * pot;
    else if(areStringsEqual(stat, "res"))
        return this->res * pot;
    else if(areStringsEqual(stat, "none"))
        return pot;
    std::cout<<"\nERROR: This stat doesn't exist";
    return 0;
}

std::vector<Effect> Player::getEquippedWeaponEffects(){
    std::vector<std::unique_ptr<InventoryElement>>::iterator it = inventoryElements.begin();
    std::vector<std::unique_ptr<InventoryElement>>::iterator end = inventoryElements.end();
    bool ok = true;
    for(it; it != end; it ++){
        if((**it).getIsEquipped() && areStringsEqual((**it).getType(), "weapon"))
            return (**it).getEffects();
    }
    std::vector<Effect> temp;
    return temp;
}

std::vector<Square> Player::getEquippedWeaponAOE(){
    std::vector<std::unique_ptr<InventoryElement>>::iterator it = inventoryElements.begin();
    std::vector<std::unique_ptr<InventoryElement>>::iterator end = inventoryElements.end();
    bool ok = true;
    for(it; it != end; it ++){
        if((**it).getIsEquipped() && areStringsEqual((**it).getType(), "weapon"))
            return (static_cast<Weapon*>((*it).get()))->getAreasOfEffect();
    }
    std::vector<Square> empty;
    return empty;
}

std::vector<Square> Player::getScrollAOEAt(unsigned int index){return (static_cast<Scroll*>(&(*inventoryElements.at(index)))->getAreasOfEffect());}

std::vector<SelfEffect> Player::getScrollSelfEffectsAt(unsigned int index){return (static_cast<Scroll*>(&(*inventoryElements.at(index)))->getSelfEffects());}

void Player::applyEffect(Effect e){
    if(areStringsEqual(e.getStat(), "mp") || areStringsEqual(e.getStat(), "maxMp")){
        int value = (int) e.getValue();
        if(areStringsEqual(e.getStat(), "mp")){
            this->mp += value;
            if(this->mp > this->maxMp)
                this->mp = this->maxMp;
            history += "\nMana points (MP) of " + this->getLabel() + ": ";
            if(value > 0)
                history += "+";
            history += std::to_string(value);
        }else if(areStringsEqual(e.getStat(), "maxMp")){
            //If the player has full mana, his current mana raise with the maxMp
            if(this->maxMp == this->mp)
                this->mp += value;
            this->maxMp += value;
            history += "\nMax mana points (MaxMP) of " + this->getLabel() + ": ";
            if(value > 0)
                history += "+";
            history += std::to_string(value);
        }
    }else{
        Character::applyEffect(e);
    }
}

void Player::applySelfEffect(SelfEffect e){
    if(areStringsEqual(e.getStat(), "mp") || areStringsEqual(e.getStat(), "maxMp")){
        int value = (int)((this->getStat(e.getPotStat())) * e.getPot());
        if(areStringsEqual(e.getStat(), "mp")){
            this->mp += value;
            if(this->mp > this->maxMp)
                this->mp = this->maxMp;
            history += "\nMana points (MP) of " + this->getLabel() + ": ";
            if(value > 0)
                history += "+";
            history += std::to_string(value);
        }else if(areStringsEqual(e.getStat(), "maxMp")){
            //If the player has full mana, his current mana raise with the maxMp
            if(this->maxMp == this->mp)
                this->mp += value;
            this->maxMp += value;
            history += "\nMax mana points (MaxMP) of " + this->getLabel() + ": ";
            if(value > 0)
                history += "+";
            history += std::to_string(value);
        }
    }else{
        Character::applySelfEffect(e);
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------

//ENEMY SECTION
Enemy::Enemy(unsigned int x, unsigned int y, unsigned int maxHP, float str, 
    float dex, float mnd, float wis, float res, 
    float movTime, float actTime, std::string attackStat, unsigned int attackRange, unsigned int sightRange, unsigned int exp, unsigned int gp, std::string label, std::string ch) : Character(x, y, maxHP, str, dex, mnd, wis, res, movTime, actTime, ch){
    this->nextActTime = 0;     
    this->attackStat = attackStat;
    this->attackRange = attackRange;
    this->sightRange = sightRange;
    this->exp = exp;
    this->gp = gp;
    this->label = label;
    this->hasAttackedLastTurn = false;
}

Enemy::Enemy(const Enemy& e, unsigned int x, unsigned int y) : Character(x, y, e.getMaxHP(), e.getStr(), e.getDex(), e.getMnd(), e.getWis(), e.getRes(), e.getMovTime(), e.getActTime(), e.getCh()){
    this->nextActTime = e.getNextActTime();     
    this->attackStat = e.getAttackStat();
    this->attackRange = e.getAttackRange();
    this->sightRange = e.getSightRange();
    this->exp = e.getExp();
    this->gp = e.getGp();
    this->label = e.getLabel();
    this->hasAttackedLastTurn = false;
}

void Enemy::setNextActTime(float nextActTime){this->nextActTime = nextActTime;}
void Enemy::setHasAttackedLastTurn(bool hasAttackedLastTurn){this->hasAttackedLastTurn= hasAttackedLastTurn;}

float Enemy::getNextActTime() const{return this->nextActTime;}
std::string Enemy::getAttackStat() const{return this->attackStat;}
unsigned int Enemy::getAtkDamage(){
    if(areStringsEqual(this->attackStat, "str"))
        return (int)this->str;
    else if(areStringsEqual(this->attackStat, "dex"))
        return (int)this->dex;
    else if(areStringsEqual(this->attackStat, "mnd"))
        return (int)this->mnd;
    else if(areStringsEqual(this->attackStat, "wis"))
        return (int)this->wis;
    else if(areStringsEqual(this->attackStat, "res"))
        return (int)this->res;
    return 0;
}
unsigned int Enemy::getAttackRange() const{return this->attackRange;}
unsigned int Enemy::getSightRange() const{return this->sightRange;}
unsigned int Enemy::getExp() const{return this->exp;}
unsigned int Enemy::getGp() const{return this->gp;}
bool Enemy::getHasAttackedLastTurn() const{return this->hasAttackedLastTurn;}