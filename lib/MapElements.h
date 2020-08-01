#ifndef _ELEMENTS_H
#define _ELEMENTS_H
#include "Characters.h"


class Box{
    private:
    unsigned int x;
    unsigned int y;
    unsigned int g;
    unsigned int h;
    std::shared_ptr<Box> previousBox;

    public:
    Box(unsigned int x, unsigned int y, unsigned int g, unsigned int h);
    unsigned int getX() const;
    unsigned int getY() const;
    unsigned int getG() const;
    unsigned int getH() const;
    std::shared_ptr<Box> getPreviousBox() const;

    void setG(unsigned int g);
    void setH(unsigned int h);
    void setPreviousBox(std::shared_ptr<Box> pBox);
};

class Door{
    private:
    std::string roomLabel;
    int id;
    int x;
    int y;
    int connectedDoorId;
    int connectedRoomId;
    bool locked;
    std::string chUnlocked;
    std::string chLocked;

    public:
    Door(int id, int x, int y, int connectedDoorId, int connectedRoomId, bool locked, std::string chUnlocked, std::string chLocked);
    Door(int id, int x, int y, bool locked, std::string chUnlocked, std::string chLocked);
    
    unsigned int getId() const;
    unsigned int getX() const;
    unsigned int getY() const;
    unsigned int getConnectedDoorId() const;
    unsigned int getConnectedRoomId() const;
    bool isLocked() const;
    std::string getChUnlocked() const;
    std::string getChLocked() const;

    void setConnectedDoorId(unsigned int id);
    void setConnectedRoomId(unsigned int id);
    void setLocked(bool input);
};

class Room{
    private:
    int id;
    std::string label;
    unsigned int x;
    unsigned int y;
    unsigned int w;
    unsigned int h;
    std::string chWall;
    std::string chFloor;
    public:
    std::vector<std::unique_ptr<Door>> doors;

    Room(int id, std::string chWall, std::string chFloor);
    Room(int id, int newX, int newY, int width, int height, std::string chWall, std::string chFloor);

    unsigned int getId() const;
    std::string getLabel() const;
    unsigned int getX() const;
    unsigned int getY() const;
    unsigned int getWidth() const;
    unsigned int getHeight() const;
    std::string getChWall() const;
    std::string getChFloor() const;

    bool addDoor(std::unique_ptr<Door>& door);
};


class Game{
    private:
    unsigned int w;
    unsigned int h;
    int exitX;
    int exitY;


    Player* player;

    bool fogMatrix[MAX_MATRIX_HEIGHT][MAX_MATRIX_WIDTH];
    int m[MAX_MATRIX_HEIGHT][MAX_MATRIX_WIDTH];
    
    
    std::vector<std::unique_ptr<Room>> rooms;
    std::vector<std::unique_ptr<Enemy>> enemies; 
    std::vector<std::unique_ptr<Enemy>> bestiaryEnemies;//This will contain each type of enemy that is available (in single copy)
    std::vector<std::unique_ptr<InventoryElement>> items;
    std::vector<InventoryElement> inventoryItems; //This will contain each type of item that is available (in single copy)
    std::vector<std::unique_ptr<Weapon>> weapons;
    std::vector<Weapon> inventoryWeapons; //This will contain each type of item that is available (in single copy)
    std::vector<std::unique_ptr<Scroll>> scrolls;
    std::vector<Scroll> inventoryScrolls; //This will contain each type of item that is available (in single copy)
    std::string chExit;
    std::string chPath;
    std::string chFog;

    public:
    float lapsedTime;
    Game();
    //~Game();
    Player* getPlayer() const;
    unsigned int getExitX() const;
    unsigned int getExitY() const;
    void generateMap();
    void initMap();
    void exportMap();
    void printInterface();
    void printMatrix();
    bool isWalkable(int x, int y); //Checks if a position can be walked
    bool isWalkableForPlayer(int x, int y); //Checks if a position can be walked, without considering the player
    unsigned int getElementType(int i, int j); //Returns the type of element at the chosen position
    unsigned int getElementTypeNoEnemy(int i, int j); //Returns the type of element at the chosen position without considering enemies
    void addRoom(std::unique_ptr<Room>& room);
    void addRoom(int id, std::string chWall, std::string chFloor);
    int isColliding(Room& room1, Room& room2); //Checks if two rooms are colliding (and if they have to be fused)
    void linkRooms(); //Link rooms when generating
    bool createPath(Room& room1, Room& room2, int& doorsCounter); //Create path when generating
    void linkRoomsByDoors(); //Link rooms when importing
    void createPathByDoors(Door& door1, Door& door2, int roomId); //Create path when importing
    void clearUnusedDoors();

    void chooseClass(); //Choose class and istantiate player
    void clearFog(); //Clear the fog around the player
    void moveEnemies();
    unsigned int getDistance(Character& a, Character& b) const;
    unsigned int getDistance(int x1, int y1, int x2, int y2) const;
    int getEnemyIndexAtPosition(unsigned int i, unsigned int j);
    int getItemIndexAtPosition(unsigned int i, unsigned int j);
    int getWeaponIndexAtPosition(unsigned int i, unsigned int j);
    int getScrollIndexAtPosition(unsigned int i, unsigned int j);

    void playerLoot(); //Manage the "take" command
    bool playerCastSpell(char direction, unsigned int index); //Manage the "cast" command
    void playerAttack(char direction); //Manage the "atk" command
    void playerAOE(Square effect, unsigned int damage, unsigned int range, int iInc, int jInc); //Manage the AOE effects of weapons and scrolls
    bool playerOpen(char direction); //Manage the "open" command
    bool playerUse(unsigned int index); //Manage the "use" command

    void getBestiary();
    void getItems();

    void printUnicode(std::string character, unsigned int color) const;
    void printRange(std::vector<Square> areasOfEffect, unsigned int range, char direction);

    void checkBox(std::shared_ptr<Box> current, std::shared_ptr<Box> temp, std::vector<std::shared_ptr<Box>>& openList, std::vector<std::shared_ptr<Box>>& closedList, unsigned int targetX, unsigned int targetY);
    void walkShortestPath(Character& c, unsigned int targetX, unsigned int targetY); //Moves the passed character according to the A* Algorithm
};

#endif