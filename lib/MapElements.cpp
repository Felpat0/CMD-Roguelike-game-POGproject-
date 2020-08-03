#include "MapElements.h"

//DOOR SECTION
//Door constructor
Door::Door(int id, int x, int y, int connectedDoorId, int connectedRoomId, bool locked, std::string chUnlocked, std::string chLocked){
    this->id = id;
    this->x = x;
    this->y = y;
    this->connectedDoorId = connectedDoorId;
    this->connectedRoomId = connectedRoomId;
    this->locked = locked;
    this->chUnlocked = chUnlocked;
    this->chLocked = chLocked;
}

Door::Door(int id, int x, int y, bool locked, std::string chUnlocked, std::string chLocked){
    this->id = id;
    this->x = x;
    this->y = y;
    this->locked = locked;
    this->chUnlocked = chUnlocked;
    this->chLocked = chLocked;
    this->connectedDoorId = 0;
    this->connectedRoomId = 0;
}


unsigned int Door::getId()const{return this->id;}
unsigned int Door::getX()const{return this->x;}
unsigned int Door::getY()const{return this->y;}
unsigned int Door::getConnectedDoorId() const{return this->connectedDoorId;}
unsigned int Door::getConnectedRoomId() const{return this->connectedRoomId;}
bool Door::isLocked() const{return this->locked;}
std::string Door::getChUnlocked() const{return this->chUnlocked;}
std::string Door::getChLocked() const{return this->chLocked;}
void Door::setConnectedDoorId(unsigned int id){this->connectedDoorId = id;}
void Door::setConnectedRoomId(unsigned int id){this->connectedRoomId = id;}
void Door::setLocked(bool input){this->locked = input;}

//----------------------------------------------------------------------------------------------------

//ROOM SECTION
//Room constructor
Room::Room(int id, std::string chWall, std::string chFloor){
    w = MIN_ROOM_WIDTH + rand() % (MAX_ROOM_WIDTH - MIN_ROOM_WIDTH + 1);
    h = MIN_ROOM_HEIGHT + rand() % (MAX_ROOM_HEIGHT - MIN_ROOM_HEIGHT + 1);
    x = MIN_X_VALUE + rand() % (MAX_X_VALUE - MIN_X_VALUE + 1);
    y = MIN_Y_VALUE + rand() % (MAX_Y_VALUE - MIN_Y_VALUE + 1);
    this->id = id;
    this->chWall = chWall;
    this->chFloor = chFloor;
}

Room::Room(int id, int newX, int newY, int width, int height, std::string chWall, std::string chFloor) : id(id), w(width), h(height), x(newX), y(newY), chWall(chWall), chFloor(chFloor){}
bool Room::addDoor(std::unique_ptr<Door>& door){
    this->doors.push_back(std::move(door));
    this->chWall = chWall;
    this->chFloor = chFloor;
    return true;
}

unsigned int Room::getId() const{return id;}
std::string Room::getLabel() const{return label;}
unsigned int Room::getX() const{return x;}
unsigned int Room::getY() const{return y;}
unsigned int Room::getWidth() const{return w;}
unsigned int Room::getHeight() const{return h;}
std::string Room::getChWall() const{return chWall;}
std::string Room::getChFloor() const{return chFloor;}

//----------------------------------------------------------------------------------------------------

//GAME SECTION
//Game constructor
Game::Game(){
    lapsedTime = 0;
    //Init matrix
    for (int i = 0; i != MAX_MATRIX_HEIGHT; i++) {
		for (int j = 0; j != MAX_MATRIX_WIDTH; j++) {
            m[i][j] = 0;
            fogMatrix[i][j] = false;
		}
	}

}

//Game functions
unsigned int Game::getExitX() const{return this->exitX;}
unsigned int Game::getExitY() const{return this->exitY;}

void Game::generateMap(){
    this->chExit = "0x00BB";
    this->chPath = "0x0023";
    this->chFog = "0x003F";
    std::cout<<"\nCreating rooms...";
    for(int counter = 0; counter != ROOMS_NUMBER; counter ++){
        addRoom(counter + 4, "0x2593", "0x00B7");
    }
    if(rooms.size() > 1){
        std::cout<<"\nDone!\nLinking rooms...";
        linkRooms();
    }
    std::cout<<"\nDone!\nExporting map...";
    exportMap();
    std::cout<<"\nDone!";
}

void Game::initMap(){
    using namespace rapidxml;
    using namespace std;
    xml_document<> doc;
	xml_node<> * root_node;
    ifstream xmlFile ("xml/map.xml"); 
    
    if(!xmlFile.is_open()){
        std::cout<<"\nError opening xml/map.xml, you have to generate a map first";
        std::exit(EXIT_FAILURE);
    }

    //Indicate start and end of the stream
    vector<char> buffer((istreambuf_iterator<char>(xmlFile)), istreambuf_iterator<char>());
    //Add the "end file character" at the end of the file
	buffer.push_back('\0');
	//Parse the buffer using the xml file parsing library into doc, now the xml_document is ready
	doc.parse<0>(&buffer[0]);
	//Assign the root node to root_node (the method returns and address)
	root_node = doc.first_node("Game");
    exitX = atoi(root_node->first_attribute("exitX")->value());
    exitY = atoi(root_node->first_attribute("exitY")->value());
    this->chExit = root_node->first_attribute("chExit")->value();
    this->chPath = root_node->first_attribute("chPath")->value();
    this->chFog = root_node->first_attribute("chFog")->value();
    root_node = root_node->first_node("Rooms");
    
    for (xml_node<> * roomNode = root_node->first_node("Room"); roomNode; roomNode = roomNode->next_sibling()){
	    std::unique_ptr<Room> temp = std::unique_ptr<Room>(new Room(atoi(roomNode->first_attribute("id")->value()), atoi(roomNode->first_attribute("x")->value()), atoi(roomNode->first_attribute("y")->value()), atoi(roomNode->first_attribute("width")->value()),
                                                                    atoi(roomNode->first_attribute("height")->value()), (roomNode->first_attribute("chWall")->value()), roomNode->first_attribute("chFloor")->value()));
        std::vector<std::unique_ptr<Room>>::iterator end = rooms.end();
        bool ok = true;
        for (std::vector<std::unique_ptr<Room>>::iterator it = rooms.begin(); it != end; it++ ){
            if(isColliding(*temp, **it) == 1)
                ok = false;
        }
        //If temp does not collide wrongly with other rooms
        if(ok){
            addRoom(temp);
            //Add doors
            for(xml_node<> * doorNode = roomNode->first_node("Doors")->first_node("Door"); doorNode; doorNode = doorNode->next_sibling()){
                if(std::string(doorNode->name()) == "Door"){
                    std::unique_ptr<Door> tempDoor = std::unique_ptr<Door>(new Door(atoi(doorNode->first_attribute("id")->value()), atoi(doorNode->first_attribute("x")->value()) + rooms.back()->getX(), atoi(doorNode->first_attribute("y")->value()) + rooms.back()->getY(), atoi(doorNode->first_attribute("connectedDoorId")->value()), atoi(doorNode->first_attribute("connectedRoomId")->value()), (bool)(atoi(doorNode->first_attribute("locked")->value())), doorNode->first_attribute("chOpen")->value(), doorNode->first_attribute("chLocked")->value()));
                    //If this wall has not been eliminated because of room fusions
                    if(m[tempDoor->getY()][tempDoor->getX()] > 3){
                        m[tempDoor->getY()][tempDoor->getX()] = -1*(rooms.back()->getId());
                        rooms.back()->addDoor(tempDoor);
                    }
                }
            }

            //Add Enemies
            for(xml_node<> * enemyNode = roomNode->first_node("Enemies")->first_node("Enemy"); enemyNode; enemyNode = enemyNode->next_sibling()){
                //Iterate over the bestiary to find the right enemy
                std::vector<std::unique_ptr<Enemy>>::iterator enemyEnd = bestiaryEnemies.end();
                for (std::vector<std::unique_ptr<Enemy>>::iterator enemyIt = bestiaryEnemies.begin(); enemyIt != enemyEnd; enemyIt++){
                    if(areStringsEqual((**enemyIt).getLabel(), enemyNode->first_attribute("label")->value())){
                        std::unique_ptr<Enemy> enemy;
                        enemy = std::unique_ptr<Enemy>(new Enemy(**enemyIt, atoi(enemyNode->first_attribute("x")->value()) + rooms.back()->getX(), atoi(enemyNode->first_attribute("y")->value()) + rooms.back()->getY()));
                        enemies.push_back(std::move(enemy));
                    }
                }
            }
            //Add items
            for(xml_node<> * itemNode = roomNode->first_node("Loots")->first_node("Loot"); itemNode; itemNode = itemNode->next_sibling()){
                //Check if it's a weapon or not
                if(areStringsEqual(itemNode->first_attribute("type")->value(), "weapon")){
                    //It's a weapon
                    std::vector<Weapon>::iterator wEnd = inventoryWeapons.end();
                    for (std::vector<Weapon>::iterator wIt = inventoryWeapons.begin(); wIt != wEnd; wIt++){
                        if(areStringsEqual((*wIt).getLabel(), itemNode->first_attribute("label")->value())){
                            std::unique_ptr<Weapon> weapon;
                            weapon = std::unique_ptr<Weapon>(new Weapon(*wIt, atoi(itemNode->first_attribute("x")->value()) + rooms.back()->getX(), atoi(itemNode->first_attribute("y")->value()) + rooms.back()->getY()));
                            weapons.push_back(std::move(weapon));
                        }
                    }
                }else if(areStringsEqual(itemNode->first_attribute("type")->value(), "scroll")){
                    //It's a spell
                    std::vector<Scroll>::iterator wEnd = inventoryScrolls.end();
                    for (std::vector<Scroll>::iterator wIt = inventoryScrolls.begin(); wIt != wEnd; wIt++){
                        if(areStringsEqual((*wIt).getLabel(), itemNode->first_attribute("label")->value())){
                            std::unique_ptr<Scroll> scroll;
                            scroll = std::unique_ptr<Scroll>(new Scroll(*wIt, atoi(itemNode->first_attribute("x")->value()) + rooms.back()->getX(), atoi(itemNode->first_attribute("y")->value()) + rooms.back()->getY()));
                            scrolls.push_back(std::move(scroll));
                        }
                    }
                }else{
                    //It's an item
                    if(areStringsEqual("key", itemNode->first_attribute("type")->value())){
                        //Key
                        std::unique_ptr<InventoryElement> item = std::unique_ptr<InventoryElement>(new InventoryElement(atoi(itemNode->first_attribute("x")->value()) + rooms.back()->getX(), atoi(itemNode->first_attribute("y")->value()) + rooms.back()->getY(), itemNode->first_attribute("label")->value(), "key", "0x00BF"));
                        items.push_back(std::move(item));
                    }else if(areStringsEqual("gp", itemNode->first_attribute("type")->value())){
                        //GP
                        std::unique_ptr<InventoryElement> item = std::unique_ptr<InventoryElement>(new InventoryElement(atoi(itemNode->first_attribute("x")->value()) + rooms.back()->getX(), atoi(itemNode->first_attribute("y")->value()) + rooms.back()->getY(), itemNode->first_attribute("label")->value(), "gp", "0x00BF"));
                        items.push_back(std::move(item));
                    }else{
                        std::vector<InventoryElement>::iterator iEnd = inventoryItems.end();
                        for (std::vector<InventoryElement>::iterator iIt = inventoryItems.begin(); iIt != iEnd; iIt++){
                            if(areStringsEqual((*iIt).getLabel(), itemNode->first_attribute("label")->value())){
                                std::unique_ptr<InventoryElement> item = std::unique_ptr<InventoryElement>(new InventoryElement(*iIt, atoi(itemNode->first_attribute("x")->value()) + rooms.back()->getX(), atoi(itemNode->first_attribute("y")->value()) + rooms.back()->getY()));
                                items.push_back(std::move(item));
                            }
                        }
                    }
                }
            }

            //Add chests
            //Check if the room has chests
            if(roomNode->first_node("treasureChests") != nullptr){
                for(xml_node<> * itemNode = roomNode->first_node("treasureChests")->first_node(); itemNode; itemNode = itemNode->next_sibling()){
                    std::unique_ptr<InventoryElement> item;
                    item = std::unique_ptr<InventoryElement>(new InventoryElement(atoi(itemNode->first_attribute("x")->value()) + rooms.back()->getX(), atoi(itemNode->first_attribute("y")->value()) + rooms.back()->getY(), itemNode->first_attribute("label")->value(), itemNode->first_attribute("type")->value(), "0x00A9"));
                    item->setChest(true);
                    items.push_back(std::move(item));
                }
            }
        }
	}

    xmlFile.close();
}

void Game::exportMap(){ 
    using namespace rapidxml;
    using namespace std;
    xml_document<> wdoc;
	
	xml_node<> *declaration = wdoc.allocate_node( node_declaration);
	declaration->append_attribute(wdoc.allocate_attribute("version","1.0"));
	declaration->append_attribute(wdoc.allocate_attribute("encoding","utf-8"));
	wdoc.append_node(declaration);	
	
	string node_name = wdoc.allocate_string("Game");
	xml_node<> *wroot = wdoc.allocate_node(node_element, node_name.c_str());
	wdoc.append_node( wroot );
	
    xml_attribute<> *attr = wdoc.allocate_attribute("w", to_string(MAX_MATRIX_WIDTH).c_str());
	wroot->append_attribute(attr);
    attr = wdoc.allocate_attribute("h", to_string(MAX_MATRIX_HEIGHT).c_str());
	wroot->append_attribute(attr);
    attr = wdoc.allocate_attribute("exitX", to_string(this->exitX).c_str());
	wroot->append_attribute(attr);
    attr = wdoc.allocate_attribute("exitY", to_string(this->exitY).c_str());
	wroot->append_attribute(attr);
    attr = wdoc.allocate_attribute("chExit", (this->chExit).c_str());
	wroot->append_attribute(attr);
    attr = wdoc.allocate_attribute("chPath", (this->chPath).c_str());
	wroot->append_attribute(attr);
    attr = wdoc.allocate_attribute("chFog", (this->chFog).c_str());
	wroot->append_attribute(attr);
	xml_node<> *roomsNode = wdoc.allocate_node( node_element,  wdoc.allocate_string("Rooms") );
    wroot->append_node(roomsNode);
    //Add rooms
	xml_node<> *room;
    std::vector<std::unique_ptr<Room>>::iterator end = rooms.end();
    std::vector<std::unique_ptr<Room>>::iterator it;
    for(it = rooms.begin(); it != end; it++){
        room = wdoc.allocate_node( node_element,  wdoc.allocate_string("Room") );
        roomsNode->append_node(room);

        attr = wdoc.allocate_attribute("id", wdoc.allocate_string(to_string((**it).getId()).c_str()));
        room->append_attribute(attr);
        attr = wdoc.allocate_attribute("label", wdoc.allocate_string((**it).getLabel().c_str()));
        room->append_attribute(attr);
        attr = wdoc.allocate_attribute("x", wdoc.allocate_string(to_string((**it).getX()).c_str()));
        room->append_attribute(attr);
        attr = wdoc.allocate_attribute("y", wdoc.allocate_string(wdoc.allocate_string(to_string((**it).getY()).c_str())));
        room->append_attribute(attr);
        attr = wdoc.allocate_attribute("width", wdoc.allocate_string(to_string((**it).getWidth()).c_str()));
        room->append_attribute(attr);
        attr = wdoc.allocate_attribute("height", wdoc.allocate_string(to_string((**it).getHeight()).c_str()));
        room->append_attribute(attr);
        attr = wdoc.allocate_attribute("chWall", wdoc.allocate_string((**it).getChWall().c_str()));
        room->append_attribute(attr);
        attr = wdoc.allocate_attribute("chFloor", wdoc.allocate_string((**it).getChFloor().c_str()));
        room->append_attribute(attr);

        //Add doors
        xml_node<> *doors = wdoc.allocate_node( node_element,  wdoc.allocate_string("Doors") );
        room->append_node(doors);
        xml_node<> *door;
        std::vector<std::unique_ptr<Door>>::iterator doorsEnd = (**it).doors.end();
        std::vector<std::unique_ptr<Door>>::iterator doorsIt = (**it).doors.begin();
    
        for(doorsIt; doorsIt != doorsEnd; doorsIt++){
            door = wdoc.allocate_node(node_element, wdoc.allocate_string("Door") );
            attr = wdoc.allocate_attribute("id", wdoc.allocate_string(to_string((**doorsIt).getId()).c_str()));
            door->append_attribute(attr);
            attr = wdoc.allocate_attribute("connectedDoorId", wdoc.allocate_string(to_string((**doorsIt).getConnectedDoorId()).c_str()));
            door->append_attribute(attr);
            attr = wdoc.allocate_attribute("connectedRoomId", wdoc.allocate_string(to_string((**doorsIt).getConnectedRoomId()).c_str()));
            door->append_attribute(attr);
            attr = wdoc.allocate_attribute("x", wdoc.allocate_string(to_string((**doorsIt).getX() - (**it).getX()).c_str()));
            door->append_attribute(attr);
            attr = wdoc.allocate_attribute("y", wdoc.allocate_string(to_string((**doorsIt).getY() - (**it).getY()).c_str()));
            door->append_attribute(attr);
            attr = wdoc.allocate_attribute("room", wdoc.allocate_string((**it).getLabel().c_str()));
            door->append_attribute(attr);
            attr = wdoc.allocate_attribute("locked", wdoc.allocate_string(to_string(int((**doorsIt).isLocked())).c_str()));
            door->append_attribute(attr);
            attr = wdoc.allocate_attribute("chOpen", wdoc.allocate_string((**doorsIt).getChUnlocked().c_str()));
            door->append_attribute(attr);
            attr = wdoc.allocate_attribute("chLocked", wdoc.allocate_string((**doorsIt).getChLocked().c_str()));
            door->append_attribute(attr);
            doors->append_node(door);
        }
        int tempX;
        int tempY;
        //Generate and add enemies
        xml_node<> *enemiesNode = wdoc.allocate_node( node_element,  wdoc.allocate_string("Enemies") );
        room->append_node(enemiesNode);
        unsigned int random1 = (rand() % (MAX_ENEMIES_NUMBER + 1 - MIN_ENEMIES_NUMBER)) + MIN_ENEMIES_NUMBER;
        for(int i = 0; i != random1; i++){
            //Decide what type of enemy will it be (basing on the bestiary)
            unsigned int enemyIndex = rand() % bestiaryEnemies.size();
            do{
                tempX = rand()%((**it).getX() + (**it).getWidth() - (**it).getX() - 2) + (**it).getX() + 1;
                tempY = rand()%((**it).getY() + (**it).getHeight() - (**it).getY() - 2) + (**it).getY() + 1;
            }while(getElementType(tempY, tempX) != 2);
            m[tempY][tempX] = 3;
            xml_node<> *enemy;
            enemy = wdoc.allocate_node(node_element, wdoc.allocate_string("Enemy") );
            attr = wdoc.allocate_attribute("label", wdoc.allocate_string(bestiaryEnemies.at(enemyIndex)->getLabel().c_str()));
            enemy->append_attribute(attr);
            attr = wdoc.allocate_attribute("x", wdoc.allocate_string(to_string(tempX - (**it).getX()).c_str()));
            enemy->append_attribute(attr);
            attr = wdoc.allocate_attribute("y", wdoc.allocate_string(to_string(tempY - (**it).getY()).c_str()));
            enemy->append_attribute(attr);

            enemiesNode->append_node(enemy);
        }

        //Generate and add items
        xml_node<> *loots = wdoc.allocate_node( node_element,  wdoc.allocate_string("Loots") );
        room->append_node(loots);
        random1 = (rand() % (MAX_ITEMS_NUMBER + 1 - MIN_ITEMS_NUMBER)) + MIN_ITEMS_NUMBER;
        for(int i = 0; i != random1; i++){
            do{
                tempX = rand()%((**it).getX() + (**it).getWidth() - (**it).getX() - 2) + (**it).getX() + 1;
                tempY = rand()%((**it).getY() + (**it).getHeight() - (**it).getY() - 2) + (**it).getY() + 1;
            }while(getElementType(tempY, tempX) != 2);
            m[tempY][tempX] = 3;
            xml_node<> *item;
            item = wdoc.allocate_node(node_element, wdoc.allocate_string("Loot") );
            attr = wdoc.allocate_attribute("x", wdoc.allocate_string(to_string(tempX - (**it).getX()).c_str()));
            item->append_attribute(attr);
            attr = wdoc.allocate_attribute("y", wdoc.allocate_string(to_string(tempY - (**it).getY()).c_str()));
            item->append_attribute(attr);

            //Decide randomly if the items is a weapon, an item, GP or a key
            int random = rand()%5;
            if(random == 0){
                //It's a weapon
                if(inventoryWeapons.size() > 0){
                    attr = wdoc.allocate_attribute("type", wdoc.allocate_string("weapon"));
                    item->append_attribute(attr);
                    //Decide randomly what weapon the item is
                    int itemIndex = rand() % inventoryWeapons.size();
                    attr = wdoc.allocate_attribute("label", wdoc.allocate_string(inventoryWeapons.at(itemIndex).getLabel().c_str()));
                    item->append_attribute(attr);
                }
            }else if(random == 1){
                //It's a scroll
                if(inventoryScrolls.size() > 0){
                    attr = wdoc.allocate_attribute("type", wdoc.allocate_string("scroll"));
                    item->append_attribute(attr);
                    //Decide randomly what weapon the item is
                    int itemIndex = rand() % inventoryScrolls.size();
                    attr = wdoc.allocate_attribute("label", wdoc.allocate_string(inventoryScrolls.at(itemIndex).getLabel().c_str()));
                    item->append_attribute(attr);
                }
            }else if(random == 2){
                //It's an item
                if(inventoryItems.size() > 0){
                    //Decide randomly what item it is
                    int itemIndex = rand() % inventoryItems.size();
                    attr = wdoc.allocate_attribute("type", wdoc.allocate_string(inventoryItems.at(itemIndex).getType().c_str()));
                    item->append_attribute(attr);
                    attr = wdoc.allocate_attribute("label", wdoc.allocate_string(inventoryItems.at(itemIndex).getLabel().c_str()));
                    item->append_attribute(attr);
                }
            }else if(random == 3){
                //It's GP
                attr = wdoc.allocate_attribute("type", wdoc.allocate_string("gp"));
                item->append_attribute(attr);
                attr = wdoc.allocate_attribute("label", wdoc.allocate_string(to_string((rand()%MAX_GP_LOOT_NUMBER - MIN_GP_LOOT_NUMBER + 1) + MIN_GP_LOOT_NUMBER).c_str()));
                item->append_attribute(attr);
            }else if(random == 4){
                //It's a key
                attr = wdoc.allocate_attribute("type", wdoc.allocate_string("key"));
                item->append_attribute(attr);
                attr = wdoc.allocate_attribute("label", wdoc.allocate_string(to_string((rand()%MAX_KEYS_LOOT_NUMBER - MIN_KEYS_LOOT_NUMBER + 1) + MIN_KEYS_LOOT_NUMBER).c_str()));
                item->append_attribute(attr);
            }
            loots->append_node(item);
        }
        //Generate and add chests
        if(rand()%CHESTS_CHANCE == 0){
            xml_node<> *chests = wdoc.allocate_node( node_element,  wdoc.allocate_string("treasureChests") );
            room->append_node(chests);
            int random = (rand() % (MAX_CHESTS_NUMBER + 1 - MIN_CHESTS_NUMBER)) + MIN_CHESTS_NUMBER;
            for(int i = 0; i != random; i++){
                do{
                    tempX = rand()%((**it).getX() + (**it).getWidth() - (**it).getX() - 2) + (**it).getX() + 1;
                    tempY = rand()%((**it).getY() + (**it).getHeight() - (**it).getY() - 2) + (**it).getY() + 1;
                }while(getElementType(tempY, tempX) != 2);
                m[tempY][tempX] = 3;

                xml_node<> *chest;
                chest = wdoc.allocate_node(node_element, wdoc.allocate_string("Chest") );
                attr = wdoc.allocate_attribute("x", wdoc.allocate_string(to_string(tempX - (**it).getX()).c_str()));
                chest->append_attribute(attr);
                attr = wdoc.allocate_attribute("y", wdoc.allocate_string(to_string(tempY - (**it).getY()).c_str()));
                chest->append_attribute(attr);
                //Decide what will the chest contain
                switch (rand() % 5)
                {
                case 0:
                    //GP
                    attr = wdoc.allocate_attribute("type", wdoc.allocate_string("gp"));
                    chest->append_attribute(attr);
                    attr = wdoc.allocate_attribute("label", wdoc.allocate_string(to_string((rand()%MAX_CHEST_GOLD - MIN_CHEST_GOLD + 1) + MIN_CHEST_GOLD).c_str()));
                    chest->append_attribute(attr);
                    break;
                case 1:
                    //Keys
                    attr = wdoc.allocate_attribute("type", wdoc.allocate_string("key"));
                    chest->append_attribute(attr);
                    attr = wdoc.allocate_attribute("label", wdoc.allocate_string(to_string((rand()%MAX_CHEST_KEYS - MIN_CHEST_KEYS + 1) + MIN_CHEST_KEYS).c_str()));
                    chest->append_attribute(attr);
                    break;
                case 2:
                    //It's a weapon
                    if(inventoryWeapons.size() > 0){
                        attr = wdoc.allocate_attribute("type", wdoc.allocate_string("weapon"));
                        chest->append_attribute(attr);
                        //Decide randomly what weapon the item is
                        int itemIndex = rand() % inventoryWeapons.size();
                        attr = wdoc.allocate_attribute("label", wdoc.allocate_string(inventoryWeapons.at(itemIndex).getLabel().c_str()));
                        chest->append_attribute(attr);
                    }
                    break;
                case 3:
                    //It's a scroll
                    if(inventoryScrolls.size() > 0){
                        attr = wdoc.allocate_attribute("type", wdoc.allocate_string("scroll"));
                        chest->append_attribute(attr);
                        //Decide randomly what weapon the item is
                        int itemIndex = rand() % inventoryScrolls.size();
                        attr = wdoc.allocate_attribute("label", wdoc.allocate_string(inventoryScrolls.at(itemIndex).getLabel().c_str()));
                        chest->append_attribute(attr);
                    }
                    break;
                case 4:
                    //It's an item
                    if(inventoryItems.size() > 0){
                        //Decide randomly what item it is
                        int itemIndex = rand() % inventoryItems.size();
                        attr = wdoc.allocate_attribute("type", wdoc.allocate_string(inventoryItems.at(itemIndex).getType().c_str()));
                        chest->append_attribute(attr);
                        attr = wdoc.allocate_attribute("label", wdoc.allocate_string(inventoryItems.at(itemIndex).getLabel().c_str()));
                        chest->append_attribute(attr);
                    }
                    break;
                default:
                    break;
                }
                chests->append_node(chest);
            }
        }
    }	 
	ofstream outfile; 
	outfile.open("xml/map.xml"); 
	outfile << wdoc;
	outfile.close();
    
}

void Game::printInterface(){
    int y = 0;
    for (int i = player->getY() - (MAP_HEIGHT/2); i != player->getY() + (MAP_HEIGHT/2) +1; i++) {
        y++;
		for (int j = player->getX() - (MAP_WIDTH/2); j != player->getX() + (MAP_WIDTH/2) +1; j++) {
            if(j < 0 || i < 0 || j >= MAX_MATRIX_WIDTH || i >= MAX_MATRIX_HEIGHT){
                std::cout<<" ";
            }else{
                //Check if Player is in this position
                if(player->getY() == i && player->getX() == j)
                    printUnicode(player->getCh(), 1);
                else if(fogMatrix[i][j] == true){
                    //Check if an enemy is in this position
                    std::vector<std::unique_ptr<Enemy>>::iterator end = enemies.end();
                    bool written = false;
                    for (std::vector<std::unique_ptr<Enemy>>::iterator it = enemies.begin(); it != end; it++ ){
                        if((**it).getY() == i && (**it).getX() == j){
                            if((**it).getHasAttackedLastTurn() || getDistance(**it, *player) < PLAYER_SIGHT){
                                written = true;
                                if(((*it)->getHP() * 100)/((*it)->getMaxHP()) >= 75)
                                    printUnicode((*it)->getCh(), 2);
                                else if (((*it)->getHP() * 100)/((*it)->getMaxHP()) >= 50)
                                    printUnicode((*it)->getCh(), 3);
                                else if (((*it)->getHP() * 100)/((*it)->getMaxHP()) >= 25)
                                    printUnicode((*it)->getCh(), 4);
                                else
                                    printUnicode((*it)->getCh(), 5);
                            }else{
                                //If not, print the map element under the enemy
                                unsigned int t = getElementTypeNoEnemy(i, j);
                                if(t == 2){
                                    written = true;
                                    printUnicode(rooms.at(abs(m[i][j]) - 4)->getChFloor(), 0);
                                }else if(t == 3){
                                    written = true;
                                    printUnicode(chPath, 0);
                                }else if(t == 8){
                                    written = true;
                                    printUnicode(this->chExit, 0);
                                }
                                
                            }
                        }
                    }
                    
                    //Check if an item is in this position
                    std::vector<std::unique_ptr<InventoryElement>>::iterator end2 = items.end();
                    for (std::vector<std::unique_ptr<InventoryElement>>::iterator it = items.begin(); it != end2; it++ ){
                        if((**it).getY() == i && (**it).getX() == j){
                            written = true;
                            printUnicode((*it)->getCh(), 0);
                        }
                    }
                    //Check if a weapon is in this position
                    std::vector<std::unique_ptr<Weapon>>::iterator end3 = weapons.end();
                    for (std::vector<std::unique_ptr<Weapon>>::iterator it = weapons.begin(); it != end3; it++){
                        if((**it).getY() == i && (**it).getX() == j){
                            written = true;
                            printUnicode((*it)->getCh(), 0);
                        }
                    }
                    //Check if a scroll is in this position
                    std::vector<std::unique_ptr<Scroll>>::iterator end4 = scrolls.end();
                    for (std::vector<std::unique_ptr<Scroll>>::iterator it = scrolls.begin(); it != end4; it++){
                        if((**it).getY() == i && (**it).getX() == j){
                            written = true;
                            printUnicode((*it)->getCh(), 0);
                        }
                    }
                    //Check if it's a map element
                    if(!written){
                        switch (getElementType(i, j)){
                            case 0:
                                //Nothing
                                written = true;
                                std::cout<<" ";
                                break;
                            case 1:
                                //Door
                                {
                                    written = true;
                                    std::vector<std::unique_ptr<Room>>::iterator roomsEnd = rooms.end();
                                    for(std::vector<std::unique_ptr<Room>>::iterator it = rooms.begin(); it != roomsEnd; it ++){
                                        std::vector<std::unique_ptr<Door>>::iterator end = (**it).doors.end();
                                        for (std::vector<std::unique_ptr<Door>>::iterator it2 = (**it).doors.begin(); it2 != end; it2++ ){
                                            if((**it2).getY() == i && (**it2).getX() == j){
                                                if((**it2).isLocked()){
                                                    printUnicode((**it2).getChLocked(), 0);
                                                }else{
                                                    printUnicode((**it2).getChUnlocked(), 0);
                                                }
                                                break;
                                            }
                                        }
                                    }
                                }
                                break;
                            case 2:
                                //Floor
                                written = true;
                                printUnicode(rooms.at(abs(m[i][j]) - 4)->getChFloor(), 0);
                                break;
                            case 3:
                                //Path
                                written = true;
                                printUnicode(chPath, 0);
                                break;
                            case 4:
                                //Wall
                                written = true;
                                printUnicode(rooms.at(abs(m[i][j]) - 4)->getChWall(), 0);
                                break;
                            case 8:
                                //Exit
                                written = true;
                                printUnicode(this->chExit, 0);
                                break;
                            default:
                                break;
                        }
                    }
                    if(!written)
                        printUnicode(rooms.at(abs(m[i][j]) - 4)->getChFloor(), 0);
                }else{
                    //It's int the fog
                    printUnicode(chFog, 0);
                }
            }
                          
		}
        std::cout<<"   ";
        switch (y){
                case 1:
                    std::cout<<"Name: "<<player->getLabel();
                    break;
                case 2:
                    std::cout<<"Class: "<<player->getPlayerClass();
                    break;
                case 3:
                    std::cout<<"Level: "<<player->getLvl();
                    break;
                case 4:
                    if((player->getHP() * 100)/(player->getMaxHP()) >= 75)
                        std::cout<<"HP: "<<termcolor::green<<player->getHP()<<"/"<<(int)player->getMaxHP();
                    else if((player->getHP() * 100)/(player->getMaxHP()) >= 50)
                        std::cout<<"HP: "<<termcolor::yellow<<player->getHP()<<"/"<<(int)player->getMaxHP();
                    else if((player->getHP() * 100)/(player->getMaxHP()) >= 25)
                        std::cout<<"HP: "<<termcolor::magenta<<player->getHP()<<"/"<<(int)player->getMaxHP();
                    else
                        std::cout<<"HP: "<<termcolor::red<<player->getHP()<<"/"<<(int)player->getMaxHP();
                    
                    std::cout<<"    ";
                    std::cout<<termcolor::reset;
                    if(player->getMaxMP()){
                        if((player->getMP() * 100)/(player->getMaxMP()) >= 75)
                            std::cout<<"MP: "<<termcolor::blue<<player->getMP()<<"/"<<(int)player->getMaxMP();
                        else if((player->getMP() * 100)/(player->getMaxMP()) >= 50)
                            std::cout<<"MP: "<<termcolor::yellow<<player->getMP()<<"/"<<(int)player->getMaxMP();
                        else if((player->getMP() * 100)/(player->getMaxMP()) >= 25)
                            std::cout<<"MP: "<<termcolor::magenta<<player->getMP()<<"/"<<(int)player->getMaxMP();
                        else
                            std::cout<<"MP: "<<termcolor::red<<player->getMP()<<"/"<<(int)player->getMaxMP();
                    }else{
                        std::cout<<"MP: "<<termcolor::red<<"0/0";
                    }
                    std::cout<<termcolor::reset;
                    break;
                case 5:
                    std::cout<<"STR: "<<(int)player->getStr();
                    std::cout<<"    ";
                    std::cout<<"DEX: "<<(int)player->getDex();
                    std::cout<<"    ";
                    std::cout<<"MND: "<<(int)player->getMnd();
                    std::cout<<"    ";
                    std::cout<<"WIS: "<<(int)player->getWis();
                    break;
                case 6:
                    std::cout<<"ActTime: "<<player->getActTime();
                    std::cout<<"    ";
                    std::cout<<"MovTime: "<<player->getMovTime();
                    std::cout<<"    ";
                    std::cout<<"Res: "<<(int)player->getRes();
                    break;
                case 7:
                    std::cout<<"Exp: "<<player->getExp()<<"/"<<player->getLvl()*100;
                    break;
                case 8:
                    std::cout<<"GP: "<<player->getGp();
                    std::cout<<"    ";
                    std::cout<<"Keys: "<<player->getKeys();
                    std::cout<<"    ";
                    std::cout<<"LapsedTime: "<<lapsedTime;
                    break;
                case 10:
                    std::cout<<"Inventory";
                    break;
            default:
                if(y >= 11 && y <= 20){
                    std::cout<<y-11;
                    if(y-11 < player->getInventorySize()){
                        if(player->getInventoryElementAt(y-11).getIsEquipped()){
                            std::cout<<termcolor::yellow<<" E : ";
                        }else
                            std::cout<<"   : ";
                        if(player->getInventoryElementAt(y-11).getIsIdentified()){
                            if(areStringsEqual(player->getInventoryElementAt(y-11).getType(), "herb"))
                                std::cout<<termcolor::green<<player->getInventoryElementAt(y-11).getLabel();
                            else if(areStringsEqual(player->getInventoryElementAt(y-11).getType(), "potion"))
                                std::cout<<termcolor::blue<<player->getInventoryElementAt(y-11).getLabel();
                            else if(areStringsEqual(player->getInventoryElementAt(y-11).getType(), "weapon")){
                                std::cout<<termcolor::yellow<<player->getInventoryElementAt(y-11).getLabel();
                                if(player->getWeaponAt(y-11).getDurability() > 0)
                                    std::cout<<" ("<<player->getWeaponAt(y-11).getDurability()<<")";
                            }else if(areStringsEqual(player->getInventoryElementAt(y-11).getType(), "protection"))
                                std::cout<<termcolor::white<<player->getInventoryElementAt(y-11).getLabel();
                            else if(areStringsEqual(player->getInventoryElementAt(y-11).getType(), "scroll"))
                                std::cout<<termcolor::magenta<<player->getInventoryElementAt(y-11).getLabel();
                        }else if(areStringsEqual(player->getInventoryElementAt(y-11).getType(), "herb"))
                            std::cout<<termcolor::green<<"Unidentified herb";
                        else
                            std::cout<<"???";
                    }else
                        std::cout<<"   : ";
                    std::cout<<termcolor::reset;
                }
                break;
            }
		std::cout << std::endl;
	}
    for(int t = 0; t != 120; t++)
        std::cout<<"-";
    std::cout<<history<<"\n";
    for(int t = 0; t != 120; t++)
        std::cout<<"-";
    std::cout<<"\n";
    
    history = "";
    if(this->player->getHP() > 0){
        std::cout<<std::endl<<"What would you like to do?\nW = move north\nS = move south\nA = move ovest\nD = move east\nEquip objectIndex = equip an object, a weapon or an armor";
        std::cout<<std::endl<<"Atk w/a/s/d = attack in the chosen direction\nUse = use the equipped object\nCast spellIndex w/a/s/d = cast the spell at the index spellIndex";
        std::cout<<std::endl<<"Open w/a/s/d = open the door in the chosen direction\nTake = loot the surrounding items\nDiscard objectIndex = discard the object at the index objectIndex";
        std::cout<<std::endl<<"Identify objectIndex = show the description of the object at the index objectIndex\nRange objectIndex w/a/s/d = show the range in the choosen direction of the object at the index objectIndex\n";
    }
}

void Game::printMatrix(){
    for(int i = 0; i != MAX_MATRIX_HEIGHT; i++){
        std::cout<<i<<" ";
        for(int j = 0; j != MAX_MATRIX_WIDTH; j++){
            if(i == exitY && j == exitX){
                printUnicode(chExit, 4);
            }else{
                switch (getElementType(i, j)){
                            case 0:
                            //Nothing
                                std::cout<<" ";
                                break;
                            case 1:
                                //Door
                                {
                                    std::vector<std::unique_ptr<Room>>::iterator roomsEnd = rooms.end();
                                    for(std::vector<std::unique_ptr<Room>>::iterator it = rooms.begin(); it != roomsEnd; it ++){
                                        std::vector<std::unique_ptr<Door>>::iterator end = (**it).doors.end();
                                        for (std::vector<std::unique_ptr<Door>>::iterator it2 = (**it).doors.begin(); it2 != end; it2++ ){
                                            if((**it2).getY() == i && (**it2).getX() == j){
                                                if((**it2).isLocked()){
                                                    printUnicode((**it2).getChLocked(), 0);
                                                }else{
                                                    printUnicode((**it2).getChUnlocked(), 0);
                                                }
                                                break;
                                            }
                                        }
                                    }
                                }
                                break;
                            case 2:
                                //Floor
                                printUnicode(rooms.at(abs(m[i][j]) - 4)->getChFloor(), 0);
                                break;
                            case 3:
                                //Path
                                printUnicode(chPath, 0);
                                break;
                            case 4:
                                //Wall
                                printUnicode(rooms.at(abs(m[i][j]) - 4)->getChWall(), 0);
                                break;
                            default:
                                std::cout<<".";
                                break;
                        }
                    }
        }
        std::cout<<"\n";
    }
}

bool Game::isWalkable(int x, int y){
    //If it's a wall or a player, return false
    if((m[y][x] >= 0) || (y == player->getY() && x == player->getX()))
        return false;
    //If it's an enemy, return false
    std::vector<std::unique_ptr<Enemy>>::iterator end = enemies.end();
    bool written = false;
    for (std::vector<std::unique_ptr<Enemy>>::iterator it = enemies.begin(); it != end; it++ ){
        if((**it).getX() == x && (**it).getY() == y)
            return false;
    }
    //If it's an item, return false
    std::vector<std::unique_ptr<InventoryElement>>::iterator end2 = items.end();
    for (std::vector<std::unique_ptr<InventoryElement>>::iterator it = items.begin(); it != end2; it++ ){
        if((**it).getX() == x && (**it).getY() == y)
            return false;
    }
    //If it's a weapon, return false
    std::vector<std::unique_ptr<Weapon>>::iterator end3 = weapons.end();
    for (std::vector<std::unique_ptr<Weapon>>::iterator it = weapons.begin(); it != end3; it++ ){
        if((**it).getX() == x && (**it).getY() == y)
            return false;
    }

    //If it's a scroll, return false
    std::vector<std::unique_ptr<Scroll>>::iterator end4 = scrolls.end();
    for (std::vector<std::unique_ptr<Scroll>>::iterator it = scrolls.begin(); it != end4; it++ ){
        if((**it).getX() == x && (**it).getY() == y)
            return false;
    }

    //If it's a closed door, return false
    std::vector<std::unique_ptr<Room>>::iterator roomsEnd = rooms.end();
    for(std::vector<std::unique_ptr<Room>>::iterator roomsIt = rooms.begin(); roomsIt != roomsEnd; roomsIt ++){
        std::vector<std::unique_ptr<Door>>::iterator end = (**roomsIt).doors.end();
        for (std::vector<std::unique_ptr<Door>>::iterator doorsIt = (**roomsIt).doors.begin(); doorsIt != end; doorsIt++ ){
            if((**doorsIt).getY() == y && (**doorsIt).getX() == x){
                if((**doorsIt).isLocked())
                    return false;
            }
        }
    }
    return true;
}

bool Game::isWalkableForPlayer(int x, int y){
    //If it's a wall or nothing, return false
    if(m[y][x] >= 0){
        return false;
    }
    //If there is an enemy, return false
    std::vector<std::unique_ptr<Enemy>>::iterator enemiesEnd = enemies.end();
    for (std::vector<std::unique_ptr<Enemy>>::iterator enemiesIt = enemies.begin(); enemiesIt != enemiesEnd; enemiesIt++){
        if((**enemiesIt).getX() == x && (**enemiesIt).getY() == y){
            return false;
        }
    }
    //If it's a closed door, return false
    std::vector<std::unique_ptr<Room>>::iterator roomsEnd = rooms.end();
    for(std::vector<std::unique_ptr<Room>>::iterator roomsIt = rooms.begin(); roomsIt != roomsEnd; roomsIt ++){
        std::vector<std::unique_ptr<Door>>::iterator end = (**roomsIt).doors.end();
        for (std::vector<std::unique_ptr<Door>>::iterator doorsIt = (**roomsIt).doors.begin(); doorsIt != end; doorsIt++ ){
            if((**doorsIt).getY() == y && (**doorsIt).getX() == x){
                if((**doorsIt).isLocked())
                    return false;
            }
        }
    }
    return true;
}

unsigned int Game::getElementType(int i, int j){
    //0 = nothing
    //1 = door
    //2 = floor
    //3 = path
    //4 = wall
    //5 = enemy
    //6 = loot
    //7 = chest
    //8 = exit
    //std::cout<<"\n\n"<<i<<","<<j<<"   "<<exitY<<","<<exitX<<"\n";
    if(m[i][j] == 0)
        return 0;
    else if(this->exitY == i && this->exitX == j)
        return 8;
    else if(m[i][j] < -3){
        //Check if enemy
        std::vector<std::unique_ptr<Enemy>>::iterator enemiesEnd = enemies.end();
        for (std::vector<std::unique_ptr<Enemy>>::iterator it = enemies.begin(); it != enemiesEnd;  it++ ){
            if((**it).getX() == j && (**it).getY() == i)
                return 5;
        }
        //Check if loot or chest
        std::vector<std::unique_ptr<InventoryElement>>::iterator itemsEnd = items.end();
        for (std::vector<std::unique_ptr<InventoryElement>>::iterator it = items.begin(); it != itemsEnd;  it++ ){
            if((**it).getX() == j && (**it).getY() == i){
                if((**it).getIsChest())
                    return 7;
                else
                    return 6;
            }
        }

        std::vector<std::unique_ptr<Room>>::iterator roomsEnd = rooms.end();
        //Check if floor/path/door
        for(std::vector<std::unique_ptr<Room>>::iterator it = rooms.begin(); it != roomsEnd; it ++){
            std::vector<std::unique_ptr<Door>>::iterator end = (**it).doors.end();
            for (std::vector<std::unique_ptr<Door>>::iterator it2 = (**it).doors.begin(); it2 != end; it2++ ){
                if((**it2).getY() == i && (**it2).getX() == j){
                    //It's door
                    return 1;
                }
            }
            
            if(i >= (**it).getY() && i <= (**it).getY() + (**it).getHeight() -1
             && j >= (**it).getX() && j <= (**it).getX() + (**it).getWidth() -1){
                //It's floor
                return 2;
            }
            
        }
        //It's path
        return 3;
    }
    else if(m[i][j] > 3){ 
        //Wall
        return 4;
    }
    return 0;
}

unsigned int Game::getElementTypeNoEnemy(int i, int j){
    //0 = nothing
    //1 = door
    //2 = floor
    //3 = path
    //4 = wall
    //6 = loot
    //7 = chest
    //8 = exit
    //std::cout<<"\n\n"<<i<<","<<j<<"   "<<exitY<<","<<exitX<<"\n";
    if(m[i][j] == 0)
        return 0;
    else if(this->exitY == i && this->exitX == j)
        return 8;
    else if(m[i][j] < -3){
        //Check if loot or chest
        std::vector<std::unique_ptr<InventoryElement>>::iterator itemsEnd = items.end();
        for (std::vector<std::unique_ptr<InventoryElement>>::iterator it = items.begin(); it != itemsEnd;  it++ ){
            if((**it).getX() == j && (**it).getY() == i){
                if((**it).getIsChest())
                    return 7;
                else
                    return 6;
            }
        }

        std::vector<std::unique_ptr<Room>>::iterator roomsEnd = rooms.end();
        //Check if floor/path/door
        for(std::vector<std::unique_ptr<Room>>::iterator it = rooms.begin(); it != roomsEnd; it ++){
            std::vector<std::unique_ptr<Door>>::iterator end = (**it).doors.end();
            for (std::vector<std::unique_ptr<Door>>::iterator it2 = (**it).doors.begin(); it2 != end; it2++ ){
                if((**it2).getY() == i && (**it2).getX() == j){
                    //It's door
                    return 1;
                }
            }
            
            if(i >= (**it).getY() && i <= (**it).getY() + (**it).getHeight() -1
             && j >= (**it).getX() && j <= (**it).getX() + (**it).getWidth() -1){
                //It's floor
                return 2;
            }
            
        }
        //It's path
        return 3;
    }
    else if(m[i][j] > 3){ 
        //Wall
        return 4;
    }
    return 0;
}

void Game::addRoom(std::unique_ptr<Room>& room){
    bool isCorner = false;
    std::vector<std::unique_ptr<Room>>::iterator end = rooms.end();
    //Write room
    for(int i = room->getY(); i != room->getY() + room->getHeight(); i++){
        for(int j = room->getX(); j != room->getX() + room->getWidth(); j++){
            isCorner = false;
            for (std::vector<std::unique_ptr<Room>>::iterator it = rooms.begin(); it != end; it++ ){
                if((i == (**it).getY() && j == (**it).getX()) || (i == (**it).getY() + (**it).getHeight() -1 && j == (**it).getX()) || (i == (**it).getY() && j == (**it).getX() + (**it).getWidth() -1) || (i == (**it).getY() + (**it).getHeight() -1 && j == (**it).getX() + (**it).getWidth() -1)){
                    isCorner = true;
                }
            }
            if((i == room->getY() && j == room->getX()) || (i == room->getY() + room->getHeight() -1 && j == room->getX()) || (i == room->getY() && j == room->getX() + room->getWidth() -1) || (i == room->getY() + room->getHeight() -1 && j == room->getX() + room->getWidth() -1)){
                isCorner = true;
            }
            if((j != room->getX() && i != room->getY()) && (j != room->getX() + room->getWidth() -1 && i != room->getY() + room->getHeight() -1))
                m[i][j] = -1*room->getId();
            else if((m[i][j] >= 4 || m[i][j] <= -4) && !isCorner){ //There is another room, fuse them
                //If there is a door, delete it
                std::vector<std::unique_ptr<Door>>::iterator doorIt = (*rooms.at(abs(m[i][j]) - 4)).doors.begin();
                std::vector<std::unique_ptr<Door>>::iterator doorEnd = (*rooms.at(abs(m[i][j]) - 4)).doors.end();
                for(doorIt; doorIt != doorEnd; doorIt ++){
                    if((**doorIt).getX() == j && (**doorIt).getY() == i){
                        doorIt = (*rooms.at(abs(m[i][j]) - 4)).doors.erase(doorIt);
                        break;
                    }
                }
                m[i][j] = -1*room->getId();
            }
            else
                m[i][j] = room->getId();
        }
    }
    rooms.push_back(std::move(room));
}

void Game::addRoom(int id, std::string chWall, std::string chFloor){
    bool ok = false;
    std::unique_ptr<Room> room;
    unsigned int counter = 0;
    while(!ok){
        counter ++;
        ok = true;
        room = std::unique_ptr<Room>(new Room(id, chWall, chFloor));
        std::vector<std::unique_ptr<Room>>::iterator end = rooms.end();
        for (std::vector<std::unique_ptr<Room>>::iterator it = rooms.begin(); it != end; it++ ){
            if(isColliding(*room, **it) == 1)
                ok = false;
        }

        if(counter > MAX_MATRIX_WIDTH*10){
            std::cout<<"\nRooms generation failed, please try decreasing ROOMS_NUMBER";
            exit (EXIT_FAILURE);
        }

    }
    bool isCorner = false;
    std::vector<std::unique_ptr<Room>>::iterator end = rooms.end();
    //Write room
    for(int i = room->getY(); i != room->getY() + room->getHeight(); i++){
        for(int j = room->getX(); j != room->getX() + room->getWidth(); j++){
            isCorner = false;
            for (std::vector<std::unique_ptr<Room>>::iterator it = rooms.begin(); it != end; it++ ){
                if((i == (**it).getY() && j == (**it).getX()) || (i == (**it).getY() + (**it).getHeight() -1 && j == (**it).getX()) || (i == (**it).getY() && j == (**it).getX() + (**it).getWidth() -1) || (i == (**it).getY() + (**it).getHeight() -1 && j == (**it).getX() + (**it).getWidth() -1)){
                    isCorner = true;
                }
            }
            if((i == room->getY() && j == room->getX()) || (i == room->getY() + room->getHeight() -1 && j == room->getX()) || (i == room->getY() && j == room->getX() + room->getWidth() -1) || (i == room->getY() + room->getHeight() -1 && j == room->getX() + room->getWidth() -1)){
                isCorner = true;
            }
            if((j != room->getX() && i != room->getY()) && (j != room->getX() + room->getWidth() -1 && i != room->getY() + room->getHeight() -1))
                m[i][j] = -1*room->getId();
            else if((m[i][j] >= 4 || m[i][j] <= -4) && !isCorner){
                m[i][j] = -1*room->getId();
            }
            else
                m[i][j] = room->getId();
        }
    }

    //If there is no game exit, generate it
    if(this->exitX == 0 && this->exitY == 0){
        this->exitX = (rand() % (room->getWidth() - 2)) + room->getX() + 1;
        this->exitY = (rand() % (room->getHeight() - 2)) + room->getY() + 1;
        m[exitY][exitX] = 3;
    }
    
    rooms.push_back(std::move(room));
}

int Game::isColliding(Room& room1, Room& room2){
    int collision = 0;
    if((room1.getX() < room2.getX() + room2.getWidth() - 2 && room1.getX() > room2.getX() + 2 || room1.getX() + room1.getWidth() < room2.getX() + room2.getWidth() - 2 && room1.getX() + room1.getWidth() > room2.getX() + 2) || (room2.getX() < room1.getX() + room1.getWidth() - 2 && room2.getX() > room1.getX() + 2 || room2.getX() + room2.getWidth() < room1.getX() + room1.getWidth() - 2 && room2.getX() + room2.getWidth() > room1.getX() + 2)){
		//Room1's upper wall colliding
        if(room1.getY() == room2.getY() + room2.getHeight() - 1){
            collision = 2;
        }
        //Room1's lower wall colliding
        if(room1.getY() + room1.getHeight() -1 == room2.getY()){
            collision = 3;
        }
	}

    if((room1.getY() < room2.getY() + room2.getHeight() - 2 && room1.getY() > room2.getY() + 2 || room1.getY() + room1.getHeight() < room2.getY() + room2.getHeight() - 2 && room1.getY() + room1.getHeight() > room2.getY() + 2) || (room2.getY() < room1.getY() + room1.getHeight() -2 && room2.getY() > room1.getY() + 2 || room2.getY() + room2.getHeight() < room1.getY() + room1.getHeight() - 2 && room2.getY() + room2.getHeight() > room1.getY() + 2)){
        //Room1's left wall colliding
        if(room1.getX() == room2.getX() + room2.getWidth() - 1){
            if(collision == 0)
                collision = 4;
            else
                collision = 1;
        }
        //Room1's right wall colliding
        if(room1.getX() + room1.getWidth() - 1 == room2.getX()){
            if(collision == 0)
                collision = 5;
            else
                collision = 1;
        }
	}
    if(collision == 0){
        if((room1.getX() <= room2.getX() + room2.getWidth() && room1.getX() >= room2.getX() || room1.getX() + room1.getWidth() <= room2.getX() + room2.getWidth() && room1.getX() + room1.getWidth() >= room2.getX()) || (room2.getX() <= room1.getX() + room1.getWidth() && room2.getX() >= room1.getX() || room2.getX() + room2.getWidth() <= room1.getX() + room1.getWidth() && room2.getX() + room2.getWidth() >= room1.getX())){
                //Then if y coordinates match
                if((room1.getY() <= room2.getY() + room2.getHeight() && room1.getY() >= room2.getY() || room1.getY() + room1.getHeight() <= room2.getY() + room2.getHeight() && room1.getY() + room1.getHeight() >= room2.getY()) || (room2.getY() <= room1.getY() + room1.getHeight() && room2.getY() >= room1.getY() || room2.getY() + room2.getHeight() <= room1.getY() + room1.getHeight() && room2.getY() + room2.getHeight() >= room1.getY())){
                    collision = 1;
                }
        }
    }
    
    return collision;
}

void Game::linkRooms(){
    std::vector<std::unique_ptr<Room>>::iterator end = rooms.end();
    std::vector<std::unique_ptr<Room>>::iterator it = rooms.begin();
    int doorsCounter = 0;
    //Check if every room is fused
    int counter = 0;
    it++;
    for (it; it != end; it++){
        if(isColliding(**it, *(rooms.at(0))) != 0)
            counter ++;
    }
    //If not every room is fused, create paths
    if(counter < rooms.size()-1){
        it = rooms.begin();
        for (it; it != end; it++){
            int otherIndex = rand() % rooms.size();
            while(otherIndex == it - rooms.begin() || isColliding(**it, *(rooms.at(otherIndex))) != 0){
                otherIndex = rand() % rooms.size();
            }
            createPath(**it, *(rooms.at(otherIndex)), doorsCounter);
        }
    }

    //Check that there are no rooms with 0 doors
    it = rooms.begin();
    for (it; it != end; it++){
        while((**it).doors.size() == 0){
            int otherIndex = rand() % rooms.size();
            while(otherIndex == it - rooms.begin() || isColliding(**it, *(rooms.at(otherIndex))) != 0){
                otherIndex = rand() % rooms.size();
            }
            createPath(**it, *(rooms.at(otherIndex)), doorsCounter);
        }
    }
}

void Game::linkRoomsByDoors(){
    std::vector<std::unique_ptr<Room>>::iterator end = rooms.end();
    std::vector<std::unique_ptr<Room>>::iterator it = rooms.begin();
    //Iterate over every room
    for (it; it != end; it++){
        std::vector<std::unique_ptr<Door>>::iterator end2 = (**it).doors.end();
        std::vector<std::unique_ptr<Door>>::iterator it2 = (**it).doors.begin();
        
        //Iterate over every room's door
        for (it2; it2 != end2; it2++){
            //If the door is connected to another door (and the path starts from it)...
            if((**it2).getConnectedDoorId() != 0){
                std::vector<std::unique_ptr<Door>>::iterator end3 = rooms.at((**it2).getConnectedRoomId() - 4)->doors.end();
                std::vector<std::unique_ptr<Door>>::iterator it3 = rooms.at((**it2).getConnectedRoomId() - 4)->doors.begin();
                //Iterate over every door of the connected room to check if it is the right one
                for (it3; it3 != end3; it3++){
                    if((**it3).getId() == (**it2).getConnectedDoorId()){ 
                        unsigned int temp = it2 - (**it).doors.begin();
                        createPathByDoors(**it2, **it3, (**it).getId()); 
                        it2 = (**it).doors.begin() + temp;
                        end2 = (**it).doors.end();
                        break;
                    }
                }
            }
        }
    }

}

bool Game::createPath(Room& room1, Room& room2, int& doorsCounter){
    //check if there is a possible straight line
    int straightX = 0;
    int straightY = 0;

    std::vector<unsigned int> addedDoorsIds;
    std::vector<unsigned int> addedRoomsIds;

    int x;
    int y;
    int oldY;
    int oldX;
    int inc;
    int previousRoomId = 0;
    int cycleEnd;
    int doors = 0; //Doors added by this function call
    //check if is vertical straight
    //Iterate over the room1's X
    for(x = room1.getX() + 1; x != room1.getX() + room1.getWidth() -1; x ++){
        bool ok = true;
        std::vector<std::unique_ptr<Room>>::iterator end = rooms.end();
        std::vector<std::unique_ptr<Room>>::iterator it = rooms.begin();
        //Iterate over rooms to do a check
        for(it = rooms.begin(); it != end; it++){
            //Check if the path would go on a wall
            if(x == (**it).getX() || x == (**it).getX() + (**it).getWidth() - 1){
                ok = false;
                break;
            }
        }
        if(ok){
            //Check if between this possibile door and room2 the would be another door
            //Find the Y coordinate
            if(room1.getY() > room2.getY() + room2.getHeight() - 1){
                //Room 1 is under room2
                y = room1.getY();
                inc = -1;
                cycleEnd = room2.getY() + room2.getHeight() - 1;
            }else if(room1.getY() + room1.getHeight() - 1 < room2.getY()){
                //Room 1 is over room2
                y = room1.getY() + room1.getHeight() - 1;
                inc = 1;
                cycleEnd = room2.getY();
            }else{
                ok = false;
            }
            
            if(ok){
                //Check if the door is found
                oldY = y;
                for(y; y != cycleEnd + inc; y += inc){
                    if(getElementType(y, x) == 1){
                        ok = false;
                        break;
                    }
                }
            }
        }
        y = oldY;
        if(x > room2.getX() && x < room2.getX() + room2.getWidth() -1 && ok){
            straightX = x;
            //std::cout<<"VERTICAL STRAIGHT\n\n";
            //Write the path
            for(y; y != cycleEnd + inc; y += inc){
                if(getElementType(y, straightX) == 4){
                    doorsCounter ++;
                    doors ++;
                    bool locked = rand()%2;
                    std::unique_ptr<Door> tempDoor = std::unique_ptr<Door>(new Door(doorsCounter, straightX, y, locked, "0x004F", "0x00D8"));
                    rooms.at(abs(m[y][straightX]) - 4)->addDoor(tempDoor);
                    if(doors > 1 && previousRoomId != abs(m[y][straightX]) - 4){
                            rooms.at(previousRoomId)->doors.back()->setConnectedDoorId(doorsCounter);
                            rooms.at(previousRoomId)->doors.back()->setConnectedRoomId(abs(m[y][straightX]));
                        }
                    m[y][straightX] = m[y][straightX]*-1;
                    previousRoomId = abs(m[y][straightX]) - 4;
                }else if(getElementType(y, straightX) == 0)
                    m[y][straightX] = room1.getId()*-1;
            }
            return true;
        }
    }
    //check if horizontal straight
    if(straightX == 0){
        for(y = room1.getY() + 1; y != room1.getY() + room1.getHeight() -1; y ++){
            bool ok = true;
            std::vector<std::unique_ptr<Room>>::iterator end = rooms.end();
            std::vector<std::unique_ptr<Room>>::iterator it = rooms.begin();
            for(it = rooms.begin(); it != end; it++){
                if(y == (**it).getY() || y == (**it).getY() + (**it).getHeight() - 1){
                    ok = false;
                    break;
                }
            }
            if(ok){
                //Check if between this possibile door and room2 the would be another door
                //Find the X coordinate
                if(room1.getX() > room2.getX() + room2.getWidth() - 1){
                    //Room 1 is under room2
                    x = room1.getX();
                    inc = -1;
                    cycleEnd = room2.getX() + room2.getWidth() - 1;
                }else if(room1.getX() + room1.getWidth() -1 < room2.getX()){
                    //Room 1 is over room2
                    x = room1.getX() + room1.getWidth() - 1;
                    inc = 1;
                    cycleEnd = room2.getX(); 
                }else{
                    ok = false;
                }

                if(ok){
                    //Check if the door is found
                    oldX = x;
                    for(x; x != cycleEnd + inc; x += inc){
                        if(getElementType(y, x) == 1){
                            ok = false;
                            break;
                        }
                    }
                }
            }
            x = oldX;
            if(y > room2.getY() && y < room2.getY() + room2.getHeight() -1 && ok){
                straightY = y;
                //std::cout<<"HORIZONTAL STRAIGHT\n\n";
                //If there is already a door, change path
                if(getElementType(straightY, x) == 1)
                    return false;
                //Write the path
                for(x; x != cycleEnd + inc; x += inc){
                    //std::cout<<getElementType(straightY, x);
                    //If it's a room's wall, create a door
                    if(getElementType(straightY, x) == 4){
                        doorsCounter ++;
                        doors ++;
                        bool locked = rand()%2;
                        std::unique_ptr<Door> tempDoor = std::unique_ptr<Door>(new Door(doorsCounter, x, straightY, locked, "0x004F", "0x00D8"));
                        rooms.at(abs(m[straightY][x]) - 4)->addDoor(tempDoor);
                        //Prendere la scorsa porta e darle come connected id quello della nuova
                        if(doors > 1 && previousRoomId != abs(m[straightY][x]) - 4){
                            rooms.at(previousRoomId)->doors.back()->setConnectedDoorId(doorsCounter);
                            rooms.at(previousRoomId)->doors.back()->setConnectedRoomId(abs(m[straightY][x]));
                        }
                        m[straightY][x] = m[straightY][x] * -1;
                        previousRoomId = abs(m[straightY][x]) - 4;
                    }else if(getElementType(straightY, x) == 0)
                        m[straightY][x] = room1.getId()*-1;
                }
                return true;;
            }
        }
    }
    //If the path is not straight
    if(straightX == 0 && straightY == 0){
        //First write an horizontal path, then a vertical
        int x = room1.getX() - 1;
        int inc = 1;
        
        if(room2.getX() + (room2.getWidth()/2) > room1.getX() + (room1.getWidth()/2)){
            x += room1.getWidth() + 1;
        }
        
        //Decide increment direction based on initial wall
        if(x <= room2.getX())
            inc = 1;
        else if(x >= room2.getX() + room2.getWidth() -1)
            inc = -1;
        bool ok;
        unsigned int counter = 0;
        do{
            counter ++;
            y = (room1.getY() + room1.getHeight() -2) - rand() % (room1.getHeight() -2);
            std::vector<std::unique_ptr<Room>>::iterator end = rooms.end();
            std::vector<std::unique_ptr<Room>>::iterator it = rooms.begin();
            ok = true;
            for(it; it != end; it++){
                if((y == (**it).getY() || y == (**it).getY() + (**it).getHeight() - 1)){
                    ok = false;
                }
            }

            //Check if the path would go on a door
            for(int k = x; (k <= room2.getX() || k >= room2.getX() + room2.getWidth() -1); k += inc){
                if(getElementType(y, k) == 1){
                    ok = false;
                    break;
                }
            }
            if(counter > ROOMS_NUMBER*100)
                return false;
        }while(!ok);

        int startingPoint[] = {y, x};
        //Add door at the start of the path
        doorsCounter ++;
        doors ++;
        if(x == room1.getX() - 1){
            m[y][x + 1] = room1.getId() * -1; //Door
            bool locked = rand()%2;
            std::unique_ptr<Door> tempDoor = std::unique_ptr<Door>(new Door(doorsCounter, x + 1, y, locked, "0x004F", "0x00D8"));
            addedDoorsIds.push_back(tempDoor->getId());
            addedRoomsIds.push_back(previousRoomId);
            room1.addDoor(tempDoor);
            previousRoomId = room1.getId() - 4;
        }else{
            m[y][x - 1] = room1.getId() * -1; //Door
            bool locked = rand()%2;
            std::unique_ptr<Door> tempDoor = std::unique_ptr<Door>(new Door(doorsCounter, x - 1, y, locked, "0x004F", "0x00D8"));
            addedDoorsIds.push_back(tempDoor->getId());
            addedRoomsIds.push_back(previousRoomId);
            room1.addDoor(tempDoor);
            previousRoomId = room1.getId() - 4;
        }
        int k;

        //Horizontal writing
        ok = true;
        for(k = startingPoint[1]; (k <= room2.getX() || k >= room2.getX() + room2.getWidth() -1); k += inc){
            if(m[y][k] == 0)
                m[y][k] = room1.getId() * -1; //Path
            else if(getElementType(y, k) == 4){
                doorsCounter ++;
                doors ++;
                bool locked = rand()%2;
                std::unique_ptr<Door> tempDoor = std::unique_ptr<Door>(new Door(doorsCounter, k, y, locked, "0x004F", "0x00D8"));
                addedDoorsIds.push_back(tempDoor->getId());
                addedRoomsIds.push_back(abs(m[y][k]) - 4);
                rooms.at(abs(m[y][k]) - 4)->addDoor(tempDoor);
                if(previousRoomId != abs(m[y][k]) - 4){
                    rooms.at(previousRoomId)->doors.back()->setConnectedDoorId(doorsCounter);
                    rooms.at(previousRoomId)->doors.back()->setConnectedRoomId(abs(m[y][k]));
                }
                previousRoomId = abs(m[y][k]) - 4;
                m[y][k] = m[y][k] * -1; //Door
            }
            //printInterface();
        }
        if(k == startingPoint[1]){
            if(m[y][k] == 0)
                m[y][k] = rooms.at(previousRoomId)->getId() * -1; //Path
            k = startingPoint[1] - inc;
        }
        //std::cout<<"\n\nfine\n\n";


        //Vertical
            m[y][k] = rooms.at(previousRoomId)->getId() * -1; //Path
            int oldInc = inc;
            inc = -1;
            if(startingPoint[0] <= room2.getY())
                inc = 1;
            int wallsCounter = 0;
            int c;
            int t = 0;
            //Count how many walls in a row would the path collide (to understand if it is going on a room edge)
            do{
                c = 0;
                //std::cout<<std::endl;
                do{
                    //std::cout<<(m[k + t*oldInc][x + c*inc]);
                    if(getElementType(y + c*inc, k + t*oldInc) ==  4 || getElementType(y + c*inc, k + t*oldInc) ==  1) //Door
                        wallsCounter ++;
                    else
                        wallsCounter = 0;
                    if(wallsCounter > 1)
                        break;
                    c++;
                }while((y + c*inc <= MAX_MATRIX_HEIGHT -1 && y + c*inc > -1) && (y + c*inc <= room2.getY() || y + c*inc >= room2.getY() + room2.getHeight() -1));
                //std::cout<<std::endl<<"Riga "<<k + t*oldInc<<"   Counter "<<wallsCounter<<"\n";
                if(wallsCounter < 2)
                    break;
                t++;
                wallsCounter = 0;
            }while(true);
            
            x = k + t*oldInc;

            //Vertical writing
            for(k = startingPoint[0]; (k <= room2.getY() || k >= room2.getY() + room2.getHeight() -1); k += inc){
                if(m[k][x] == 0)
                    m[k][x] = rooms.at(previousRoomId)->getId() * -1; //Path
                else if(getElementType(k, x) == 4){
                    doorsCounter ++;
                    bool locked = rand()%2;
                    std::unique_ptr<Door> tempDoor = std::unique_ptr<Door>(new Door(doorsCounter, x, k, locked, "0x004F", "0x00D8"));
                    rooms.at(abs(m[k][x]) - 4)->addDoor(tempDoor);
                    if(previousRoomId != abs(m[k][x]) - 4){
                        rooms.at(previousRoomId)->doors.back()->setConnectedDoorId(doorsCounter);
                        //std::cout<<"\nDoor "<<rooms.at(previousRoomId)->doors.back()->getId()<<" connected to door "<<doorsCounter<<"\n";
                        rooms.at(previousRoomId)->doors.back()->setConnectedRoomId(abs(m[k][x]));
                        //std::cout<<"\n"<<previousRoomId<<"  "<<abs(m[k][x]) - 4;
                    }
                    previousRoomId = abs(m[k][x]) - 4;
                    m[k][x] = rooms.at(previousRoomId)->getId() * -1; //Door
                }
            }
    }
    return true;
    
}

void Game::createPathByDoors(Door& door1, Door& door2, int roomId){
    //Check if there is a possible straight line
    int straightX = 0;
    int straightY = 0;
    int inc = 1;

    //Check if is vertical straight
    if(door1.getX() == door2.getX()){
        straightX = door1.getX();
        if(door1.getY() > door2.getY())
            inc = -1;
        for(int i = door1.getY() + inc; i != door2.getY(); i += inc){
            m[i][straightX] = -1*roomId;
        }
    }
    //Check if horizontal straight
    else if(straightX == 0 && door1.getY() == door2.getY()){
        straightY = door1.getY();
        if(door1.getX() > door2.getX())
            inc = -1;
        for(int j = door1.getX() + inc; j != door2.getX(); j += inc){
            m[straightY][j] = -1*roomId;
        }
    }else{
        int y;
        int j;
        //If the door to be reached is on the left, then Increment is negative
        if(door1.getX() > door2.getX())
            inc = -1;

        //Horizontal path writing
        for(j = door1.getX() + inc; j != door2.getX(); j += inc){
            //If there is a wall, add a door
            if(m[door1.getY()][j] > 3){
                bool locked = rand() % 2;
                std::unique_ptr<Door> tempDoor = std::unique_ptr<Door>(new Door(rooms.size()*10, j, door1.getY(), locked, "0x004F", "0x00D8"));
                rooms.at(abs(m[door1.getY()][j]) - 4)->addDoor(tempDoor);
            }
            m[door1.getY()][j] = -1*roomId;
        }

        //If the door to be reached is on the upper side, then Increment is negative
        if(door1.getY() > door2.getY())
            inc = -1;
        else
            inc = 1;
        //Vertical path writing
        for(int i = door1.getY(); i != door2.getY(); i += inc){
            //If there is a wall, add a door
            if(m[i][j] > 3){
                bool locked = rand() % 2;
                std::unique_ptr<Door> tempDoor = std::unique_ptr<Door>(new Door(rooms.size()*10, j, i, locked, "0x004F", "0x00D8"));
                rooms.at(abs(m[i][j]) - 4)->addDoor(tempDoor);
            }
            m[i][j] = -1*roomId;
        }
        
    }
}

void Game::clearUnusedDoors(){
    using namespace std;
    //Clear unused doors (doors with connectedDoorId = 0 and nessun'altra porta le collega)
    vector<unique_ptr<Room>>::iterator roomsIt = rooms.begin();
    vector<unique_ptr<Room>>::iterator roomsEnd = rooms.end();
    for(roomsIt; roomsIt != roomsEnd; roomsIt ++){
        vector<unique_ptr<Door>>::iterator doorIt = (**roomsIt).doors.begin();
        vector<unique_ptr<Door>>::iterator doorEnd = (**roomsIt).doors.end();
        int counter = 0;
        if((**roomsIt).doors.size() > 0){
            for(int i = 0; i < (**roomsIt).doors.size() - counter; i++){
                if((**roomsIt).doors.at(i)->getConnectedDoorId() == 0){
                    bool toBeDeleted = true;
                    //Chech if the door has a near path
                    if(getElementType((**roomsIt).doors.at(i)->getY() - 1,(**roomsIt).doors.at(i)->getX()) == 3){
                        toBeDeleted = false;
                    }else if(getElementType((**roomsIt).doors.at(i)->getY() + 1, (**roomsIt).doors.at(i)->getX()) == 3){
                        toBeDeleted = false;
                    }else if(getElementType((**roomsIt).doors.at(i)->getY(), (**roomsIt).doors.at(i)->getX() - 1) == 3){
                        toBeDeleted = false;
                    }else if(getElementType((**roomsIt).doors.at(i)->getY(), (**roomsIt).doors.at(i)->getX() + 1) == 3){
                        toBeDeleted = false;
                    }

                    if(toBeDeleted){
                        m[(**roomsIt).doors.at(i)->getY()][(**roomsIt).doors.at(i)->getX()] = (**roomsIt).getId();
                        (**roomsIt).doors.erase((**roomsIt).doors.begin() + i);
                        i--;
                        counter ++;
                        if((**roomsIt).doors.size() == 0){
                            break;
                        }
                    }
                }
            }
        }
    }
}

void Game::chooseClass(){
    std::cout<<"\nChoose your class:";
    //Import classes and tell the user what they do
    using namespace rapidxml;
    using namespace std;
    xml_document<> doc;
	xml_node<> * root_node;
    ifstream xmlFile ("xml/classes.xml"); 

    if(!xmlFile.is_open()){
        std::cout<<"\nError opening xml/classes.xml";
        std::exit(EXIT_FAILURE);
    }

    //Indicate start and end of the stream
    vector<char> buffer((istreambuf_iterator<char>(xmlFile)), istreambuf_iterator<char>());
    //Add the "end file character" at the end of the file
	buffer.push_back('\0');
	//Parse the buffer using the xml file parsing library into doc, now the xml_document is ready
	doc.parse<0>(&buffer[0]);
	//Assign the root node to root_node (the method returns and address)
	root_node = doc.first_node("classes");
    for (xml_node<> * classNode = root_node->first_node("heroClass"); classNode; classNode = classNode->next_sibling()){
        std::cout<<"\n\n- "<<classNode->first_attribute("label")->value();
	    std::cout<<"\nHP: "<<classNode->first_node("baseStats")->first_attribute("hpMax")->value();
        std::cout<<"  MP:"<<classNode->first_node("baseStats")->first_attribute("mpMax")->value();
        std::cout<<"\nSTR: "<<classNode->first_node("baseStats")->first_attribute("str")->value();
        std::cout<<"  DEX:"<<classNode->first_node("baseStats")->first_attribute("dex")->value();
        std::cout<<"  MND:"<<classNode->first_node("baseStats")->first_attribute("mnd")->value();
        std::cout<<"  WIS:"<<classNode->first_node("baseStats")->first_attribute("wis")->value();
        std::cout<<"  RES:"<<classNode->first_node("baseStats")->first_attribute("res")->value();
        
        std::cout<<"\nStarting items:";
        xml_node<> *equipNode = classNode->first_node("startingEquipment");
        for (equipNode = equipNode->first_node(); equipNode; equipNode = equipNode->next_sibling()){
            std::cout<<"\n"<<equipNode->first_attribute("label")->value()<<" - "<<equipNode->name();
        }
	}
    std::string temp;
    bool ok = false;
    while(!ok){
        std::cout<<"\n\nWhat class do you choose? ";
        fflush(stdin);
        getline(std::cin, temp);
        for (xml_node<> * classNode = root_node->first_node("heroClass"); classNode; classNode = classNode->next_sibling()){
            if(areStringsEqual(temp, classNode->first_attribute("label")->value())){
                ok = true;
                temp = classNode->first_attribute("label")->value();
            }
        }

        if(!ok){
            std::cout<<"\nThe input doesn't correspond to any class. Try Again.\n";
        }
    }

    for (xml_node<> * classNode = root_node->first_node("heroClass"); classNode; classNode = classNode->next_sibling()){
        if(areStringsEqual(temp, classNode->first_attribute("label")->value())){
            std::string name;
            fflush(stdin);
            std::cout<<"\nYou are a "<<temp<<", what is your name? ";
            getline(std::cin, name);
            
            //Spawn player
            unsigned int tempX;
            unsigned int tempY;
            do{
                unsigned int tempRoomIndex = rand() % ROOMS_NUMBER;
                tempX = rooms[tempRoomIndex]->getX() + rooms[tempRoomIndex]->getWidth() -2 - rand() % (rooms[tempRoomIndex]->getWidth() -2);
                tempY = rooms[tempRoomIndex]->getY() + rooms[tempRoomIndex]->getHeight() -2 - rand() % (rooms[tempRoomIndex]->getHeight() -2);
            }while(getElementType(tempY, tempX) != 2 || getDistance(tempX, tempY, exitX, exitY) < MIN_PLAYER_EXIT_DISTANCE);
            //Cycle goes on if the player is too close to the exit
            player = std::make_unique<Player>(Player(name, tempX, tempY, atoi(classNode->first_node("baseStats")->first_attribute("mpMax")->value()), 
            stof(classNode->first_node("baseStats")->first_attribute("hpMax")->value()), stof(classNode->first_node("baseStats")->first_attribute("str")->value()), 
            stof(classNode->first_node("baseStats")->first_attribute("dex")->value()), stof(classNode->first_node("baseStats")->first_attribute("mnd")->value()), 
            stof(classNode->first_node("baseStats")->first_attribute("wis")->value()), stof(classNode->first_node("baseStats")->first_attribute("res")->value()), 
            stof(classNode->first_node("baseStats")->first_attribute("movT")->value()), stof(classNode->first_node("baseStats")->first_attribute("actT")->value()), 
            temp, "0x0050"));
            //Set level up stats
            player->setUpHpMax(stof(classNode->first_node("levelUpStats")->first_attribute("hpMax")->value()));
            player->setUpMpMax(stof(classNode->first_node("levelUpStats")->first_attribute("mpMax")->value()));
            player->setUpStr(stof(classNode->first_node("levelUpStats")->first_attribute("str")->value()));
            player->setUpDex(stof(classNode->first_node("levelUpStats")->first_attribute("dex")->value()));
            player->setUpMnd(stof(classNode->first_node("levelUpStats")->first_attribute("mnd")->value()));
            player->setUpWis(stof(classNode->first_node("levelUpStats")->first_attribute("wis")->value()));
            player->setUpRes(stof(classNode->first_node("levelUpStats")->first_attribute("res")->value()));
            player->setUpMovT(stof(classNode->first_node("levelUpStats")->first_attribute("movT")->value()));
            player->setUpActT(stof(classNode->first_node("levelUpStats")->first_attribute("actT")->value()));
            
            for(xml_node<>* equipNode = classNode->first_node("startingEquipment")->first_node(); equipNode; equipNode = equipNode->next_sibling()){
                //Give player his starting weapons
                std::vector<Weapon>::iterator it = inventoryWeapons.begin();
                std::vector<Weapon>::iterator end = inventoryWeapons.end();
                for(it; it != end; it++){
                    if(areStringsEqual(equipNode->first_attribute("label")->value(), it->getLabel())){
                        unique_ptr<InventoryElement> a (new Weapon(*it, 0, 0));
                        a->setIdentified(true);
                        player->addInventoryElement(a, false);
                    }
                }
                //Give player his starting scrolls
                std::vector<Scroll>::iterator it2 = inventoryScrolls.begin();
                std::vector<Scroll>::iterator end2 = inventoryScrolls.end();
                for(it2; it2 != end2; it2++){
                    if(areStringsEqual(equipNode->first_attribute("label")->value(), it2->getLabel())){
                        unique_ptr<InventoryElement> a (new Scroll(*it2, 0, 0));
                        a->setIdentified(true);
                        player->addInventoryElement(a, false);
                    }
                }
                //Give player his starting items
                std::vector<InventoryElement>::iterator it3 = inventoryItems.begin();
                std::vector<InventoryElement>::iterator end3 = inventoryItems.end();
                for(it3; it3 != end3; it3++){
                    if(areStringsEqual(equipNode->first_attribute("label")->value(), it3->getLabel())){
                        unique_ptr<InventoryElement> a (new InventoryElement(*it3));
                        a->setIdentified(true);
                        player->addInventoryElement(a, false);
                    }
                }
            }
            
            std::cout<<"\n\n"<<name<<", press enter to begin your adventure!\n";
            system("pause");
        }
    }

    xmlFile.close();
}

void Game::clearFog(){
    for(int i = player->getY() - PLAYER_SIGHT; i != player->getY() + PLAYER_SIGHT +1; i++){
        for(int j = player->getX() - PLAYER_SIGHT; j != player->getX() + PLAYER_SIGHT +1; j++){
            if(j >= 0 && i >= 0 && j < MAX_MATRIX_WIDTH && i < MAX_MATRIX_HEIGHT)
                fogMatrix[i][j] = true;
        }
    }
}

void Game::moveEnemies(){
    int direction;
    int tempX;
    int tempY;
    std::vector<std::unique_ptr<Enemy>>::iterator end = enemies.end();
    for (std::vector<std::unique_ptr<Enemy>>::iterator it = enemies.begin(); it != end; it++ ){
        if((**it).getNextActTime() <= lapsedTime){
            //If player is in attack range, attack it
            if(getDistance((**it), *player) <= (**it).getAttackRange()){
                this->player->takeDamage((**it).getLabel(), (**it).getAtkDamage());
                (**it).setNextActTime(this->lapsedTime + (**it).getActTime());
                (**it).setHasAttackedLastTurn(true);
            }
            //If player is in sight range, move towards him
            else if(getDistance((**it), *player) <= (**it).getSightRange()){
                walkShortestPath(**it, this->player->getX(), this->player->getY());
                (**it).setNextActTime(this->lapsedTime + (**it).getMovTime());
            }
            //If none, just move randomly
            else{
                //Check available directions
                std::vector<unsigned int> directions;
                if(isWalkable((**it).getX(), (**it).getY() -1)){
                    directions.push_back(1);
                }
                if(isWalkable((**it).getX(), (**it).getY() +1)){
                    directions.push_back(2);
                }
                if(isWalkable((**it).getX() -1, (**it).getY())){
                    directions.push_back(3);
                }
                if(isWalkable((**it).getX() +1, (**it).getY())){
                    directions.push_back(4);
                }
                if(directions.size() > 0){
                    direction = directions.at(rand() % directions.size());
                    switch (direction){
                        case 1: //Up
                            tempX = (**it).getX();
                            tempY = (**it).getY() - 1;
                            break;
                        case 2: //Down
                            tempX = (**it).getX();
                            tempY = (**it).getY() + 1;
                            break;
                        case 3: //Left
                            tempX = (**it).getX() - 1;
                            tempY = (**it).getY();
                            break;
                        case 4: //Right
                            tempX = (**it).getX() + 1;
                            tempY = (**it).getY();
                            break;
                    }
                    (**it).setCoordinates(tempX, tempY);
                    (**it).setNextActTime(this->lapsedTime + (**it).getMovTime());
                }
                
            }
        }
    }
}

unsigned int Game::getDistance(Character& a, Character& b) const{
    return (abs((int)(a.getX() - b.getX())) + abs(((int)(a.getY() - b.getY()))));
}

unsigned int Game::getDistance(int x1, int y1, int x2, int y2) const{
    return (abs(x1 - x2) + abs(y1 - y2));
}

int Game::getEnemyIndexAtPosition(unsigned int i, unsigned int j){
    std::vector<std::unique_ptr<Enemy>>::iterator enemyEnd = enemies.end();
    for (std::vector<std::unique_ptr<Enemy>>::iterator enemyIt = enemies.begin(); enemyIt != enemyEnd; enemyIt++){
        if((**enemyIt).getY() == i && (**enemyIt).getX() == j)
            return enemyIt - enemies.begin();
    }
    return -1;
}

int Game::getItemIndexAtPosition(unsigned int i, unsigned int j){
    //Check if it's a consumable
    std::vector<std::unique_ptr<InventoryElement>>::iterator end = items.end();
    for (std::vector<std::unique_ptr<InventoryElement>>::iterator it = items.begin(); it != end; it++){
        if((**it).getY() == i && (**it).getX() == j){
            return it - items.begin();
        }
    }
    return -1;
}

int Game::getWeaponIndexAtPosition(unsigned int i, unsigned int j){
    //Check if it's a weapon
    std::vector<std::unique_ptr<Weapon>>::iterator end3 = weapons.end();
    for (std::vector<std::unique_ptr<Weapon>>::iterator it = weapons.begin(); it != end3; it++){
        if((**it).getY() == i && (**it).getX() == j){
            return it - weapons.begin();
        }
    }
    return -1;
}

int Game::getScrollIndexAtPosition(unsigned int i, unsigned int j){
    //Check if it's a scroll
    std::vector<std::unique_ptr<Scroll>>::iterator end4 = scrolls.end();
    for (std::vector<std::unique_ptr<Scroll>>::iterator it = scrolls.begin(); it != end4; it++){
        if((**it).getY() == i && (**it).getX() == j){
            return it - scrolls.begin();
        }
    }
    return -1;
}

void Game::playerLoot(){
    for(int i = -1; i != 2; i++){
        for(int j = -1; j != 2; j++){
            if(player->getInventorySize() < 10){
                if(i != 0 || j != 0){
                    unsigned int tempX = player->getX() + j;
                    unsigned int tempY = player->getY() + i;
                    if(tempX >= 0 && tempX <= MAX_MATRIX_WIDTH - 1 && tempY >= 0 && tempY <= MAX_MATRIX_HEIGHT - 1){
                        int temp = getItemIndexAtPosition(tempY, tempX);
                        if(temp != -1 && !items.at(temp)->getIsChest()){
                            //It's a consumable
                            player->addInventoryElement(items.at(temp), true);
                            items.erase(items.begin() + temp);
                        }else{
                            temp = getWeaponIndexAtPosition(tempY, tempX);
                            if(temp != -1){
                                //It's a weapon
                                std::unique_ptr<InventoryElement> t (std::move(weapons.at(temp)));
                                weapons.erase(weapons.begin() + temp);
                                player->addInventoryElement(t, true);
                        }else{
                                temp = getScrollIndexAtPosition(tempY, tempX);
                                if(temp != -1){
                                    //It's a scroll
                                    std::unique_ptr<InventoryElement> t (std::move(scrolls.at(temp)));
                                    scrolls.erase(scrolls.begin() + temp);
                                    player->addInventoryElement(t, true);
                                }
                            }
                        }
                    }
                }
            }else{
                std::cout<<"\nYour inventory is full, you can't loot more items.";
                system("pause");
                break;
            }
        }
        if(player->getInventorySize() >= 10)
            break;
    }
}

bool Game::playerCastSpell(char direction, unsigned int index){
    if(index >= this->player->getInventorySize()){
        std::cout<<"\nThe index is not valid. Press enter to continue\n";
        fflush(stdin);
        system("pause");
        return false;
    }else if(!this->player->getInventoryElementAt(index).getIsIdentified()){
        //Check if the item has been identified
        std::cout<<"\nThis item has to be identified first. Press enter to continue.\n";
        system("pause");
        return false;
    }else if(!areStringsEqual(this->player->getInventoryElementAt(index).getType(), "scroll")){
        //Check if the item is a scroll
        std::cout<<"\nThis item is not a scroll. Press enter to continue.\n";
        system("pause");
        return false;
    }else{
        //Check if player has enough mana
        if(this->player->getMP() - this->player->getScrollAt(index).getMpCost() < 0){
            std::cout<<"\nYOU HAVE NO MANA!!!";
            system("pause");
            return false;
        }
        //Decrease player's mana
        this->player->setMp(this->player->getMP() - this->player->getScrollAt(index).getMpCost());
        history += "\n" + this->player->getLabel() + " used the scroll " + this->player->getInventoryElementAt(index).getLabel() + ".";
        unsigned int tempI = this->player->getY();
        unsigned int tempJ = this->player->getX();
        int iInc = 0;
        int jInc = 0;
        if(tolower(direction) == 'w')
            iInc = -1;
        else if(tolower(direction) == 's')
            iInc = 1;
        else if(tolower(direction) == 'a')
            jInc = -1;
        else
            jInc = 1;
            
        //Apply AOEs
        std::vector<Square> t = this->player->getScrollAOEAt(index); 

        std::vector<Square>::iterator it = t.begin();
        std::vector<Square>::iterator end = t.end();
        //Iterate over weapon's AOEs
        for(it; it != end; it ++){
            playerAOE(*(it), this->player->getAtkDamage(it->getStat(), it->getPot()), this->player->getScrollAt(index).getRange(), iInc, jInc);
        }

        //Apply self effects
        std::vector<SelfEffect> t2 = this->player->getScrollSelfEffectsAt(index); 
        std::vector<SelfEffect>::iterator it2 = t2.begin();
        std::vector<SelfEffect>::iterator end2 = t2.end();
        //Iterate over weapon's AOEs
        for(it2; it2 != end2; it2 ++){
            this->player->applySelfEffect(*it2);
        }
        
    }
    return true;
}

void Game::playerAttack(char direction){
    int weaponIndex = this->player->getEquippedWeaponIndex();
    unsigned int tempI = this->player->getY();
    unsigned int tempJ = this->player->getX();
    int iInc = 0;
    int jInc = 0;

    if(tolower(direction) == 'w')
        iInc = -1;
    else if(tolower(direction) == 's')
        iInc = 1;
    else if(tolower(direction) == 'a')
        jInc = -1;
    else
        jInc = 1;

    std::vector<Square> t = this->player->getEquippedWeaponAOE();
    if(t.size() == 0){
        //If the weapon has no AOEs/player has no weapon equipped, just attack forward with pot=1 and scaling on STR
        Square temp = Square(1, 0, "str", 1);
        playerAOE(temp, this->player->getAtkDamage("str", 1), 0, iInc, jInc);
    }else{
        std::vector<Square>::iterator it = t.begin();
        std::vector<Square>::iterator end = t.end();
        //Iterate over weapon's AOEs
        for(it; it != end; it ++){
            playerAOE((*(it)), this->player->getAtkDamage(it->getStat(), it->getPot()), this->player->getWeaponAt(weaponIndex).getRange(), iInc, jInc);
        }
        //Reduce weapon's durability
        this->player->reduceWeaponDurability(weaponIndex);
    }

}

void Game::playerAOE(Square effect, unsigned int damage, unsigned int range, int iInc, int jInc){
    unsigned int tempI = this->player->getY();
    unsigned int tempJ = this->player->getX();
    //Go to the origin point
    for(int k = 0; k != range; k++){
        tempI += iInc;
        tempJ += jInc;
        if(getEnemyIndexAtPosition(tempI, tempJ) != -1)
            break;
    }
    int i = tempI;
    int j = tempJ;
    int fd;
    int rd;
    fd = effect.getFd();
    rd = effect.getRd();
    int counter = 0;
    if(iInc != 0){
        //std::cout<<"\n\n\nRange "<<i + (iInc * fd)<<"  "<< j + rd<<"\n\n\n";
        int tempIndex = getEnemyIndexAtPosition(i + (iInc * fd), j + rd);
        if(tempIndex > -1){
            if(!enemies.at(tempIndex)->takeDamage(this->player->getLabel(), damage)){
                //Enemy died
                history += "\n" + this->player->getLabel() + " obtained " + std::to_string(enemies.at(tempIndex)->getGp()) + " gp.";
                this->player->addGold(enemies.at(tempIndex)->getGp());
                history += "\n" + this->player->getLabel() + " obtained " + std::to_string(enemies.at(tempIndex)->getExp()) + " exp.";
                this->player->addExp(enemies.at(tempIndex)->getExp());
                enemies.erase(enemies.begin() + tempIndex);
            }
        }
    }else{
        //std::cout<<"\n\n\nRange "<<i + rd<<"  "<< j + (jInc * fd)<<"\n\n\n";
        int tempIndex = getEnemyIndexAtPosition(i + rd, j + (jInc * fd));
        if(tempIndex > -1){
            if(!enemies.at(tempIndex)->takeDamage(this->player->getLabel(), damage)){
                //Enemy died
                history += "\n" + this->player->getLabel() + " obtained " + std::to_string(enemies.at(tempIndex)->getGp()) + " gp.";
                this->player->addGold(enemies.at(tempIndex)->getGp());
                history += "\n" + this->player->getLabel() + " obtained " + std::to_string(enemies.at(tempIndex)->getExp()) + " exp.";
                this->player->addExp(enemies.at(tempIndex)->getExp());
                enemies.erase(enemies.begin() + tempIndex);
            }

        }
    }
}

bool Game::playerOpen(char direction){
    int iInc = 0;
    int jInc = 0;

    if(tolower(direction) == 'w')
        iInc = -1;
    else if(tolower(direction) == 's')
        iInc = 1;
    else if(tolower(direction) == 'a')
        jInc = -1;
    else
        jInc = 1;
    unsigned int tempI = this->player->getY() + iInc;
    unsigned int tempJ = this->player->getX() + jInc;

    if(getElementType(tempI, tempJ) == 1){
        //It's a door
        std::vector<std::unique_ptr<Room>>::iterator roomsEnd = rooms.end();
        for(std::vector<std::unique_ptr<Room>>::iterator roomsIt = rooms.begin(); roomsIt != roomsEnd; roomsIt ++){
            std::vector<std::unique_ptr<Door>>::iterator end = (**roomsIt).doors.end();
            for (std::vector<std::unique_ptr<Door>>::iterator doorsIt = (**roomsIt).doors.begin(); doorsIt != end; doorsIt++ ){
                if((**doorsIt).getY() == tempI && (**doorsIt).getX() == tempJ){
                    if(!(**doorsIt).isLocked()){
                        std::cout<<"\nThe door is already open.";
                        system("pause");
                        return false;
                    }else{
                        std::string choice;
                        while(true){
                            bool ac = false;
                            if(this->player->getKeys() > 0){
                                std::cout<<"\nDo you want to use a key? y/n\n";
                                getline(std::cin, choice);

                                if(areStringsEqual(choice, "y")){
                                    if(this->player->getKeys() > 0){
                                        history += "\n" + this->player->getLabel() + " opened a door using a key.";
                                        this->player->addKeys(-1);
                                        (**doorsIt).setLocked(false);
                                        return true;
                                    }
                                }else if(areStringsEqual(choice, "n")){
                                ac = true;
                            }else
                                std::cout<<"\nInsert a valid input.";
                            }else{
                                ac = true;
                            }

                            if(ac){
                                std::cout<<"\nDo you want to perform an ability check on STR? y/n\n";
                                getline(std::cin, choice);

                                if(areStringsEqual(choice, "y")){
                                    int result = this->player->abilityCheck("str", 15);
                                    if(result >= 0){
                                        history += "\n" + this->player->getLabel() + " opened a door.";
                                        (**doorsIt).setLocked(false);
                                    }else{
                                        history += "\n" + this->player->getLabel() + " failed to open a door.";
                                        this->player->takeDamage("Door", abs((int)(result)/2));
                                    }
                                    return true;
                                }else if(areStringsEqual(choice, "n")){
                                    return false;
                                }else
                                    std::cout<<"\nInsert a valid input.";
                            }
                        }
                    }
                }
            }
        }
    }else if(getElementType(this->player->getY() + iInc, this->player->getX() + jInc) == 7){
        //It's a chest
        std::vector<std::unique_ptr<InventoryElement>>::iterator end = items.end();
        for (std::vector<std::unique_ptr<InventoryElement>>::iterator it = items.begin(); it != end; it++ ){
            if((**it).getY() == tempI && (**it).getX() == tempJ){
                std::string choice;
                    while(true){
                        bool ac = false;
                        if(this->player->getKeys() > 0){
                            std::cout<<"\nDo you want to use a key? y/n\n";
                            getline(std::cin, choice);

                            if(areStringsEqual(choice, "y")){
                                history += "\n" + this->player->getLabel() + " opened a chest using a key.";
                                this->player->addInventoryElement(*it, true);
                                this->player->addKeys(-1);
                                items.erase(it);
                                return true;
                            }else if(areStringsEqual(choice, "n"))
                                ac = true;
                            else
                                std::cout<<"\nInsert a valid input.";
                        }else 
                            ac = true;
                        if(ac){
                            std::cout<<"\nDo you want to perform an ability check on DEX? y/n\n";
                            getline(std::cin, choice);

                            if(areStringsEqual(choice, "y")){
                                int result = this->player->abilityCheck("dex", 15);
                                if(result >= 0){
                                    history += "\n" + this->player->getLabel() + " opened a chest.";
                                    this->player->addInventoryElement(*it, true);
                                    items.erase(it);
                                }else{
                                    history += "\n" + this->player->getLabel() + " opened a chest but also got injured.";
                                    this->player->takeDamage("Chest", abs(result)/3);
                                    this->player->addInventoryElement(*it, true);
                                    items.erase(it);
                                }
                                items.erase(it);
                                return true;
                            }else if(areStringsEqual(choice, "n")){
                                return false;
                            }else
                                std::cout<<"\nInsert a valid input.";
                        }
                    
                }
            }
            
        }
    }else{
        std::cout<<"\nThere is nothing to open in this direction.";
        system("pause");
        return false;
    }
    return false;
}

bool Game::playerUse(){
    bool found = false;
    unsigned int index;
    for(int i = 0; i != player->getInventorySize(); i++){
        if(player->getInventoryElementAt(i).getIsEquipped() && (areStringsEqual(this->player->getInventoryElementAt(i).getType(), "herb")
        || areStringsEqual(this->player->getInventoryElementAt(i).getType(), "potion"))){
            found = true;
            index = i;
        }
    }
    if(!found){
        std::cout<<"\nYou don't have an equipped item.\n";
        fflush(stdin);
        system("pause");
        return false;
    }else{
        std::vector<Effect> t = this->player->getInventoryElementAt(index).getEffects();
        std::vector<Effect>::iterator it = t.begin();
        std::vector<Effect>::iterator end = t.end();
        history += "\n" + this->player->getLabel() + " used " + this->player->getInventoryElementAt(index).getLabel() + ".";
        for(it; it != end; it++){
            this->player->applyEffect(*it);
        }
        this->player->discardItem(index, false);
        return true;
    }

    return false;
    
}

void Game::getBestiary(){
    using namespace rapidxml;
    using namespace std;
    xml_document<> doc;
	xml_node<> * root_node;
    ifstream xmlFile ("xml/enemies.xml"); 

    if(!xmlFile.is_open()){
        std::cout<<"\nError opening xml/enemies.xml";
        std::exit(EXIT_FAILURE);
    }

    //Indicate start and end of the stream
    vector<char> buffer((istreambuf_iterator<char>(xmlFile)), istreambuf_iterator<char>());
    //Add the "end file character" at the end of the file
	buffer.push_back('\0');
	//Parse the buffer using the xml file parsing library into doc, now the xml_document is ready
	doc.parse<0>(&buffer[0]);
	//Assign the root node to root_node (the method returns and address)
	root_node = doc.first_node("enemies");
    std::unique_ptr<Enemy> enemy;
    for (xml_node<> * enemyNode = root_node->first_node("enemy"); enemyNode; enemyNode = enemyNode->next_sibling()){
        enemy = std::unique_ptr<Enemy>(new Enemy(0, 0, stof(enemyNode->first_node("baseStats")->first_attribute("hpMax")->value()), stof(enemyNode->first_node("baseStats")->first_attribute("str")->value()),
        stof(enemyNode->first_node("baseStats")->first_attribute("dex")->value()), stof(enemyNode->first_node("baseStats")->first_attribute("mnd")->value()),
        stof(enemyNode->first_node("baseStats")->first_attribute("wis")->value()), stof(enemyNode->first_node("baseStats")->first_attribute("res")->value()),
        stof(enemyNode->first_node("baseStats")->first_attribute("movT")->value()), stof(enemyNode->first_node("baseStats")->first_attribute("actT")->value()),
        (enemyNode->first_node("enemyStats")->first_attribute("atkStat")->value()), atoi(enemyNode->first_node("enemyStats")->first_attribute("atkRange")->value()),
        atoi(enemyNode->first_node("enemyStats")->first_attribute("sight")->value()), atoi(enemyNode->first_node("enemyStats")->first_attribute("exp")->value()),
        atoi(enemyNode->first_node("enemyStats")->first_attribute("gp")->value()), enemyNode->first_attribute("label")->value(), enemyNode->first_attribute("ch")->value()));

        bestiaryEnemies.push_back(std::move(enemy));
    }
    xmlFile.close();
}

void Game::getItems(){
    using namespace rapidxml;
    using namespace std;
    xml_document<> doc;
	xml_node<> * root_node;
    ifstream xmlFile ("xml/items.xml"); 

    if(!xmlFile.is_open()){
        std::cout<<"\nError opening xml/items.xml";
        std::exit(EXIT_FAILURE);
    }

    //Indicate start and end of the stream
    vector<char> buffer((istreambuf_iterator<char>(xmlFile)), istreambuf_iterator<char>());
    //Add the "end file character" at the end of the file
	buffer.push_back('\0');
	//Parse the buffer using the xml file parsing library into doc, now the xml_document is ready
	doc.parse<0>(&buffer[0]);
	//Assign the root node to root_node (the method returns and address)
	root_node = doc.first_node("items");
    for (xml_node<> * itemNode = root_node->first_node(); itemNode; itemNode = itemNode->next_sibling()){
        if(areStringsEqual(itemNode->name(), "weapon")){
            //It's a weapon
            inventoryWeapons.push_back(Weapon(0, 0, itemNode->first_attribute("label")->value(), itemNode->name(), itemNode->first_attribute("ch")->value(), atoi(itemNode->first_attribute("durability")->value()), atoi(itemNode->first_attribute("range")->value())));
            //Add effects to the weapon
            xml_node<> * tempNode = itemNode->first_node("bonusStats");
            if(tempNode != nullptr){
                for (xml_node<> * effectNode = tempNode->first_node("effect"); effectNode; effectNode = effectNode->next_sibling()){
                    inventoryWeapons.back().addEffect(Effect(effectNode->first_attribute("stat")->value(), stof(effectNode->first_attribute("value")->value())));
                }
            }
            //AreasOfEffect to the weapon
            tempNode = itemNode->first_node("areaOfEffect");
            if(tempNode != nullptr){
                for (xml_node<> * effectNode = tempNode->first_node("square"); effectNode; effectNode = effectNode->next_sibling()){
                    inventoryWeapons.back().addAreasOfEffect(Square(atoi(effectNode->first_attribute("fd")->value()), atoi(effectNode->first_attribute("rd")->value()), effectNode->first_attribute("stat")->value(), stof(effectNode->first_attribute("pot")->value())));
                }
            }
        }else if(areStringsEqual(itemNode->name(), "scroll")){
            //It's a scroll
            inventoryScrolls.push_back(Scroll(0, 0, itemNode->first_attribute("label")->value(), itemNode->name(), itemNode->first_attribute("ch")->value(), atoi(itemNode->first_attribute("range")->value()), stof(itemNode->first_attribute("mpCost")->value())));
            //Add self effects to the scroll
            xml_node<> * tempNode = itemNode->first_node("selfEffects");
            if(tempNode != nullptr){
                for (xml_node<> * effectNode = tempNode->first_node("effect"); effectNode; effectNode = effectNode->next_sibling()){
                    inventoryScrolls.back().addSelfEffect(SelfEffect(effectNode->first_attribute("stat")->value(), stof(effectNode->first_attribute("pot")->value()), effectNode->first_attribute("potStat")->value()));
                }
            }
            //Add AreasOfEffect to the scroll
            tempNode = itemNode->first_node("areaOfEffect");
            if(tempNode != nullptr){
                for (xml_node<> * effectNode = tempNode->first_node("square"); effectNode; effectNode = effectNode->next_sibling()){
                    inventoryScrolls.back().addAreasOfEffect(Square(atoi(effectNode->first_attribute("fd")->value()), atoi(effectNode->first_attribute("rd")->value()), effectNode->first_attribute("stat")->value(), stof(effectNode->first_attribute("pot")->value())));
                }
            }
        }else{
            //It's a generic item
            inventoryItems.push_back(InventoryElement(0, 0, itemNode->first_attribute("label")->value(), itemNode->name(), itemNode->first_attribute("ch")->value()));
            //Add effects to the item
            for (xml_node<> * effectNode = itemNode->first_node("effect"); effectNode; effectNode = effectNode->next_sibling()){
                inventoryItems.back().addEffect(Effect(effectNode->first_attribute("stat")->value(), stof(effectNode->first_attribute("value")->value())));
            }
        }
    }
    xmlFile.close();
}

void Game::printUnicode(std::string character, unsigned int color) const{
    switch (color)
    {
    case 1:
        //Player green
        std::cout<<termcolor::green;
        break;
    case 2:
        //High health enemy
        std::cout <<termcolor::blue;
        break;
    case 3:
        //Mid high health enemy
        std::cout<<termcolor::magenta;
        break;
    case 4:
        //Mid low health enemy
        std::cout<<termcolor::yellow;
        break;
    case 5:
        //Low health enemy
        std::cout<<termcolor::red;
        break;
    default:
        break;
    }
    int dec = (int)strtol(character.c_str(), NULL, 16);
    if(dec < 128){
        //1 byte
        printf("%c", dec);
    }else{
        std::string binary = "";
        int started = false;
        int n;
        //Convert to binary
        for(int i = 2; i != character.length(); i++){
            switch(toupper(character[i]))
            {
                case '0':
                    binary += "0000";
                    break;
                case '1':
                    binary += "0001";
                    break;
                case '2':
                    binary += "0010";
                    break;
                case '3':
                    binary += "0011";
                    break;
                case '4':
                    binary += "0100";
                    break;
                case '5':
                    binary += "0101";
                    break;
                case '6':
                    binary += "0110";
                    break;
                case '7':
                    binary += "0111";
                    break;
                case '8':
                    binary += "1000";
                    break;
                case '9':
                    binary += "1001";
                    break;
                case 'A':
                    binary += "1010";
                    break;
                case 'B':
                    binary += "1011";
                    break;
                case 'C':
                    binary += "1100";
                    break;
                case 'D':
                    binary += "1101";
                    break;
                case 'E':
                    binary += "1110";
                    break;
                case 'F':
                    binary += "1111";
                    break;
            }
        }
        if(dec >= 67108864){
            //6 bytes
            n = 6;            
        }else if(dec >= 2097152){
            //5 bytes
            n = 5;
        }else if(dec >= 65536){
            //2 bytes
            n = 4;
        }else if(dec >= 2048){
            //3 bytes
            n = 3;
        }else if(dec >= 128){
            //2 bytes
            n = 2;
        }
        std::string bytes[n];
        int i;
        for(i = 0; i != n; i++){
            bytes[0] += '1';
            if(i + 1 < n)
                bytes[i + 1] += "10";
        }
        bytes[0] += '0';
        //Delete the initial bytes
        for(i = 0; binary.size() > (8*n) - (n+1) - 2*(n-1); i++){
            binary.erase(binary.begin());
        }

        //Add the binary numbers to every byte
        int j = 0;
        //First byte
        for(j; j != 8 - (n+1); j++){
            bytes[0] += binary[j];
        }
        //std::cout<<"\n"<<bytes[0];
        //Other bytes
        int c;
        for(i = 1; i != n; i++){
            for(c = 0; c != 6; c++){
                bytes[i] += binary[j + c];
            } 
            j += c;
        }

        switch (n)
        {
        case 2:
            printf("%c%c", std::stoi(bytes[0], nullptr, 2), std::stoi(bytes[1], nullptr, 2));
            break;
        case 3:
            printf("%c%c%c", std::stoi(bytes[0], nullptr, 2), std::stoi(bytes[1], nullptr, 2), std::stoi(bytes[2], nullptr, 2));
            break;
        case 4:
            printf("%c%c%c%c", std::stoi(bytes[0], nullptr, 2), std::stoi(bytes[1], nullptr, 2), std::stoi(bytes[2], nullptr, 2), std::stoi(bytes[3], nullptr, 2));
            break;
        case 5:
            printf("%c%c%c%c%c", std::stoi(bytes[0], nullptr, 2), std::stoi(bytes[1], nullptr, 2), std::stoi(bytes[2], nullptr, 2), std::stoi(bytes[3], nullptr, 2), std::stoi(bytes[4], nullptr, 2));
            break;
        case 6:
            printf("%c%c%c%c%c", std::stoi(bytes[0], nullptr, 2), std::stoi(bytes[1], nullptr, 2), std::stoi(bytes[2], nullptr, 2), std::stoi(bytes[3], nullptr, 2), std::stoi(bytes[4], nullptr, 2), std::stoi(bytes[5], nullptr, 2));
            break;
        default:
            break;
        }
    }

    std::cout << termcolor::reset;
}

void Game::printRange(std::vector<Square> areasOfEffect, unsigned int range, char direction){
    std::vector<unsigned int> iCoordinates;
    std::vector<unsigned int> jCoordinates;
    //Fill the map with the necessary coordinates
    int iInc = 0;
    int jInc = 0;

    if(tolower(direction) == 'w')
        iInc = -1;
    else if(tolower(direction) == 's')
        iInc = 1;
    else if(tolower(direction) == 'a')
        jInc = -1;
    else
        jInc = 1;

    unsigned int tempI = this->player->getY();
    unsigned int tempJ = this->player->getX();
    //Go to the origin point
    for(int k = 0; k != range; k++){
        tempI += iInc;
        tempJ += jInc;
        iCoordinates.push_back(tempI);
        jCoordinates.push_back(tempJ);
        if(getEnemyIndexAtPosition(tempI, tempJ) != -1)
            break;
    }
    int i = tempI;
    int j = tempJ;
    int fd;
    int rd;
    std::vector<Square>::iterator it = areasOfEffect.begin();
    std::vector<Square>::iterator end = areasOfEffect.end();
    int counter = 0;
    for(it; it != end; it ++){
        fd = it->getFd();
        rd = it->getRd();
        if(iInc != 0){
            //std::cout<<"\nCoordinate I "<<i + (iInc * fd)<<"  "<< j + rd;
            iCoordinates.push_back(i + (iInc * fd));
            jCoordinates.push_back(j + rd);
            //coordinates.insert(std::pair<unsigned int, unsigned int>(i + (iInc * fd), j + rd));
        }else{
            //std::cout<<"\nCoordinate J "<<i + rd<<"   "<<j + (jInc * fd);
            iCoordinates.push_back(i + rd);
            jCoordinates.push_back(j + (jInc * fd));
            //coordinates.insert(std::pair<unsigned int, unsigned int>(i + rd, j + (jInc * fd)));
        }
    }
    //std::cout<<"\n\nSize: "<<coordinates.size()<<"Counter: "<<counter<<"\n\n";
    //Print map
    int y = 0;
    for (int i = player->getY() - (MAP_HEIGHT/2); i != player->getY() + (MAP_HEIGHT/2) +1; i++) {
        y++;
		for (int j = player->getX() - (MAP_WIDTH/2); j != player->getX() + (MAP_WIDTH/2) +1; j++) {
            std::vector<unsigned int>::iterator itI = iCoordinates.begin();
            std::vector<unsigned int>::iterator itJ = jCoordinates.begin();
            std::vector<unsigned int>::iterator endI = iCoordinates.end();
            
            for(itI; itI != endI; itI ++, itJ ++){
                if(*itI == i && *itJ == j){
                    std::cout<<termcolor::on_cyan;
                }
            }

            if(j < 0 || i < 0 || j >= MAX_MATRIX_WIDTH || i >= MAX_MATRIX_HEIGHT){
                std::cout<<" ";
            }else{
                //Check if Player is in this position
                if(player->getY() == i && player->getX() == j)
                    printUnicode(player->getCh(), 1);
                else if(fogMatrix[i][j] == true){
                    //Check if an enemy is in this position
                    std::vector<std::unique_ptr<Enemy>>::iterator end = enemies.end();
                    bool written = false;
                    for (std::vector<std::unique_ptr<Enemy>>::iterator it = enemies.begin(); it != end; it++ ){
                        if((**it).getY() == i && (**it).getX() == j){
                            written = true;
                            if(((*it)->getHP() * 100)/((*it)->getMaxHP()) >= 75)
                                printUnicode((*it)->getCh(), 2);
                            else if (((*it)->getHP() * 100)/((*it)->getMaxHP()) >= 50)
                                printUnicode((*it)->getCh(), 3);
                            else if (((*it)->getHP() * 100)/((*it)->getMaxHP()) >= 25)
                                printUnicode((*it)->getCh(), 4);
                            else
                                printUnicode((*it)->getCh(), 5);
                        }
                    }
                    //Check if an item is in this position
                    std::vector<std::unique_ptr<InventoryElement>>::iterator end2 = items.end();
                    for (std::vector<std::unique_ptr<InventoryElement>>::iterator it = items.begin(); it != end2; it++ ){
                        if((**it).getY() == i && (**it).getX() == j){
                            written = true;
                            printUnicode((*it)->getCh(), 0);
                        }
                    }
                    //Check if a weapon is in this position
                    std::vector<std::unique_ptr<Weapon>>::iterator end3 = weapons.end();
                    for (std::vector<std::unique_ptr<Weapon>>::iterator it = weapons.begin(); it != end3; it++){
                        if((**it).getY() == i && (**it).getX() == j){
                            written = true;
                            printUnicode((*it)->getCh(), 0);
                        }
                    }

                    //Check if a scroll is in this position
                    std::vector<std::unique_ptr<Scroll>>::iterator end4 = scrolls.end();
                    for (std::vector<std::unique_ptr<Scroll>>::iterator it = scrolls.begin(); it != end4; it++){
                        if((**it).getY() == i && (**it).getX() == j){
                            written = true;
                            printUnicode((*it)->getCh(), 0);
                        }
                    }

                    //Check if it's a map element
                    if(!written){
                        switch (getElementType(i, j)){
                            case 0:
                            //Nothing
                                std::cout<<" ";
                                break;
                            case 1:
                                //Door
                                {
                                    std::vector<std::unique_ptr<Door>>::iterator end = rooms.at(abs(m[i][j]) - 4)->doors.end();
                                    std::vector<std::unique_ptr<Door>>::iterator it = rooms.at(abs(m[i][j]) - 4)->doors.begin();
                                    //Iterate over every door of the connected room to check if it is the right one
                                    for (it; it != end; it++){
                                        if((**it).getY() == i && (**it).getX() == j){
                                            if((**it).isLocked())
                                                printUnicode((**it).getChLocked(), 0);
                                            else
                                                printUnicode((**it).getChUnlocked(), 0);
                                            break;
                                        }
                                    }
                                }
                                break;
                            case 2:
                                //Floor
                                printUnicode(rooms.at(abs(m[i][j]) - 4)->getChFloor(), 0);
                                break;
                            case 3:
                                //Path
                                printUnicode(chPath, 0);
                                break;
                            case 4:
                                //Wall
                                printUnicode(rooms.at(abs(m[i][j]) - 4)->getChWall(), 0);
                                break;
                            case 8:
                                //Exit
                                printUnicode(this->chExit, 0);
                                break;
                            default:
                                break;
                        }
                    }
                }else{
                    //It's int the fog
                    printUnicode(chFog, 0);
                }
            }
            std::cout<<termcolor::reset;
        }
		std::cout << std::endl;
	}
    system("pause");
}

void Game::checkBox(std::shared_ptr<Box> current, std::shared_ptr<Box> temp, std::vector<std::shared_ptr<Box>>& openList, std::vector<std::shared_ptr<Box>>& closedList, unsigned int targetX, unsigned int targetY){
    //Check if temp is in the closedList
    bool closed = false;
    std::vector<std::shared_ptr<Box>>::iterator it = closedList.begin();
    std::vector<std::shared_ptr<Box>>::iterator end = closedList.end();
    for(it; it != end; it ++){
        //If the square is in the closedList...
        if((**it).getX() == temp->getX() && (**it).getY() == temp->getY()){
             //Don't add it to the openList
            closed = true;
        }
    }

    if(!closed){
        //Check if temp is in the openList
        bool open = false;
        it = openList.begin();
        end = openList.end();
        for(it; it != end; it ++){
            //If the square is in the openList...
            if((**it).getX() == temp->getX() && (**it).getY() == temp->getY()){
                open = true;
                unsigned int newGScore = current->getG() + 1;
                unsigned int newHScore = getDistance(temp->getX(), temp->getY(), targetX, targetY);
                if(newGScore + newHScore < (**it).getG() + (**it).getH()){
                    (**it).setG(newGScore);
                    (**it).setH(newHScore);
                    std::shared_ptr<Box> t = (current->getPreviousBox()); 
                    (**it).setPreviousBox(t); 
                }
            }
        }
        if(!open){
            openList.push_back(temp);
        }
    }
}

void Game::walkShortestPath(Character& c, unsigned int targetX, unsigned int targetY){
    bool found = false;
    unsigned int counter = 0;
    //Create open list
    std::vector<std::shared_ptr<Box>> openList;
    //Create closed list
    std::vector<std::shared_ptr<Box>> closedList;
    //Set the current square
    std::shared_ptr<Box> current = std::make_shared<Box> (Box(c.getX(), c.getY(), 0, 0));
    while(true){/*
        //TEMP INIZIO
        for (int i = player->getY() - (MAP_HEIGHT/2); i != player->getY() + (MAP_HEIGHT/2) +1; i++) {
            for (int j = player->getX() - (MAP_WIDTH/2); j != player->getX() + (MAP_WIDTH/2) +1; j++) {
                if(j < 0 || i < 0 || j >= MAX_MATRIX_WIDTH || i >= MAX_MATRIX_HEIGHT){
                    std::cout<<" ";
                }else{
                    //Check if Player is in this position
                    if(player->getY() == i && player->getX() == j)
                        printUnicode(player->getCh(), 1);
                    else if(!isWalkable(j, i))
                        std::cout<<"X";
                    else{
                        std::vector<std::shared_ptr<Box>>::iterator it = openList.begin();
                        std::vector<std::shared_ptr<Box>>::iterator end = openList.end();
                        bool written = false;
                        for(it; it != end; it ++){
                            if((**it).getX() == j && (**it).getY() == i){
                                written = true;
                                std::cout<<termcolor::green<<"."<<termcolor::reset;
                            }
                        }
                        it = closedList.begin();
                        end = closedList.end();
                        for(it; it != end; it ++){
                            if((**it).getX() == j && (**it).getY() == i){
                                written = true;
                                std::cout<<termcolor::red<<"."<<termcolor::reset;
                            }
                        }
                        
                        
                        if(!written)
                            std::cout<<".";
                    }
                }
            }
            std::cout<<std::endl;
        }*/
        
        //TEMP FINE
        //Add the walkable squares to the open list if necessary
        //Upper square
        //Check if the target is in the open list
        counter ++;
        if(current->getX() == targetX && current->getY() - 1 == targetY){
            found = true;
            break;
        }
        if(isWalkable(current->getX(), current->getY() - 1)){
            std::shared_ptr<Box> temp = std::make_shared<Box>(Box(current->getX(), current->getY() - 1, current->getG() + 1, getDistance(current->getX(), current->getY() - 1, targetX, targetY)));
            //Set previous box
            temp->setPreviousBox(current);
            
            checkBox(current, temp, openList, closedList, targetX, targetY);
        }
        //Lower square
        //Check if the target is in the open list
        if(current->getX() == targetX && current->getY() + 1 == targetY){
            found = true;
            break;
        }
        if(isWalkable(current->getX(), current->getY() + 1)){
            std::shared_ptr<Box> temp = std::make_shared<Box>(Box(current->getX(), current->getY() + 1, current->getG() + 1, getDistance(current->getX(), current->getY() + 1, targetX, targetY)));
            //Set previous box
            temp->setPreviousBox(current);
            //Check if the target is in the open list
            checkBox(current, temp, openList, closedList, targetX, targetY);
        }
        //Right square
        //Check if the target is in the open list
        if(current->getX() + 1 == targetX && current->getY() == targetY){
            found = true;
            break;
        }
        if(isWalkable(current->getX() + 1, current->getY())){
            std::shared_ptr<Box> temp = std::make_shared<Box>(Box(current->getX() + 1, current->getY(), current->getG() + 1, getDistance(current->getX() + 1, current->getY(), targetX, targetY)));
            //Set previous box
            temp->setPreviousBox(current);
            //Check if the target is in the open list
            checkBox(current, temp, openList, closedList, targetX, targetY);
        }
        //Left square
        //Check if the target is in the open list
        if(current->getX() - 1 == targetX && current->getY() == targetY){
            found = true;
            break;
        }
        //std::cout<<"\nStar 8";
        if(isWalkable(current->getX() - 1, current->getY())){
            std::shared_ptr<Box> temp = std::make_shared<Box>(Box(current->getX() - 1, current->getY(), current->getG() + 1, getDistance(current->getX() - 1, current->getY(), targetX, targetY)));
            //Set previous box
            temp->setPreviousBox(current);
            //Check if the target is in the open list
            checkBox(current, temp, openList, closedList, targetX, targetY);
        }
        //Find the box with the lower F score
        std::vector<std::shared_ptr<Box>>::iterator it = openList.begin();
        std::vector<std::shared_ptr<Box>>::iterator end = openList.end();
        unsigned int minG = (**it).getG();
        unsigned int minH = (**it).getH();
        unsigned int minIndex = 0;
        for(it; it != end; it ++){
            //If the F score of the current element is lower...
            if((**it).getG() + (**it).getH() < minG + minH){
                minG = (**it).getG();
                minH = (**it).getH();
                minIndex = it - openList.begin();
            }else if((**it).getG() + (**it).getH() == minG + minH){
                if((**it).getH() < minH){
                    minG = (**it).getG();
                    minH = (**it).getH();
                    minIndex = it - openList.begin();
                }
            }
        }
        if(openList.size() == 0)
            break;
        //Add the box with the lower F score to the closed list
        closedList.push_back(std::make_shared<Box>(Box(openList.at(minIndex)->getX(), openList.at(minIndex)->getY(), openList.at(minIndex)->getG(), openList.at(minIndex)->getH())));
        closedList.back()->setPreviousBox(openList.at(minIndex)->getPreviousBox());
        //Set the current box
        current = closedList.back();
        //Remove the current box from the open list
        openList.erase(openList.begin() + minIndex);
        if(counter > 150)
            break;
    }
    //If the path has been found...
    if(found){
        std::shared_ptr<Box> p = current;
        if(counter > 2){
            //Iterate backwards to find where it started
            while(p->getPreviousBox()->getPreviousBox() != nullptr){
                p = p->getPreviousBox();
            }
        }
        c.setCoordinates(p->getX(), p->getY());
    }else{
        //Else there is no path, the character won't move for now
        //std::cout<<"\n\nNot found :(\n\n";
    }
}


//----------------------------------------------------------------------------------------------------

//BOX SECTION
Box::Box(unsigned int x, unsigned int y, unsigned int g, unsigned int h){
    this->x = x;
    this->y = y;
    this->g = g;
    this->h = h;
}

unsigned int Box::getX() const{return this->x;}
unsigned int Box::getY() const{return this->y;}
unsigned int Box::getG() const{return this->g;}
unsigned int Box::getH() const{return this->h;}
std::shared_ptr<Box> Box::getPreviousBox() const{return this->previousBox;}

void Box::setG(unsigned int g){this->g = g;}
void Box::setH(unsigned int h){this->h = h;}
void Box::setPreviousBox(std::shared_ptr<Box> pBox){this->previousBox = pBox;}