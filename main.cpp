#include "Lib/MapElements.h"

unsigned int gameState;
int main(){
    SetConsoleOutputCP(65001);
    srand(time(NULL));
    Game game = Game();
    game.getBestiary();
    game.getItems();
    std::string a = "0";
    while(atoi(a.c_str()) != 1 && atoi(a.c_str()) != 2){
        std::cout<<"1.Generate map\n2.Import map and play\n";
        std::getline(std::cin, a);
        while((atoi(a.c_str()) != 1 && atoi(a.c_str()) != 2)){
            std::cout<<"\nInsert a valid input: ";
            std::getline(std::cin, a);
        }
    }
    if(atoi(a.c_str()) == 1){
        game.generateMap();
        std::cout<<"\n";
        //game.printMatrix();
        return 0;
    }else if(atoi(a.c_str()) == 2){
        std::cout<<"\nImporting map...";
        game.initMap();
        std::cout<<"\nDone!\nLinking rooms...";
        game.linkRoomsByDoors();
        std::cout<<"\nDone!\nClearing unused doors...";
        game.clearUnusedDoors();
        std::cout<<"\nDone!\n";
        game.chooseClass();
    }


    
    //Game loop
    gameState = 0; //1=lost, 2=won
    int error = 0;
    
    unsigned int actionType;
    //system("cls");
    while(gameState == 0){
        do{
            
            //Clear the fog around the player
            game.clearFog();
            system("cls");
            game.printInterface();
            std::string choice = "";
            std::getline(std::cin, choice);
            actionType = game.player->input(choice);
            //If the user inserted a valid action
            if(actionType != 0){
                error = 0;
                switch (actionType){
                    case 1: 
                        //Player movement
                        switch (game.player->getMovementIntention()){
                            case 1: //Up
                                if(game.isWalkable(game.player->getX(), game.player->getY() - 1))
                                    game.player->setCoordinates(game.player->getX(), game.player->getY() - 1);
                                else
                                    error = 1;
                                break;
                            case 2: //Down
                                if(game.isWalkable(game.player->getX(), game.player->getY() + 1))
                                    game.player->setCoordinates(game.player->getX(), game.player->getY() + 1);
                                else
                                    error = 1;
                                break;
                            case 3: //Left
                                if(game.isWalkable(game.player->getX() - 1, game.player->getY()))
                                    game.player->setCoordinates(game.player->getX() - 1, game.player->getY());
                                else
                                    error = 1;
                                break;
                            case 4: //Right
                                if(game.isWalkable(game.player->getX() + 1, game.player->getY())){
                                    game.player->setCoordinates(game.player->getX() + 1, game.player->getY());
                                }else
                                    error = 1;
                                break;
                        }
                        if(error == 0){
                            game.lapsedTime += game.player->getMovTime();
                        }
                        game.player->resetMovementIntention();
                        break;
                    case 2:
                        //Equip item
                        if(game.player->equipItem((choice[6]) - '0'))
                            game.lapsedTime += game.player->getActTime();
                        break;
                    case 3:
                        //Attack
                        game.playerAttack(choice[4]); //Even if player hits none, time passes
                        game.lapsedTime += game.player->getActTime();
                        break;
                    case 4:
                        //Cast spell
                        if(game.playerCastSpell(choice[7], choice[5] - '0'))
                            game.lapsedTime += game.player->getActTime();
                        break;
                    case 5:
                        //Use item
                        if(game.playerUse())
                            game.lapsedTime += game.player->getActTime();
                       break;
                    case 6:
                        //Loot items
                        if(game.player->getInventorySize() >= 10){
                            std::cout<<"\nYour inventory is full, you can't loot more items.";
                            system("pause");
                        }else{
                            game.playerLoot();
                            game.lapsedTime += game.player->getActTime();
                        }
                        break;
                    case 7:
                        //Discard item
                        if(game.player->discardItem(choice[8] - '0', true))
                            game.lapsedTime += game.player->getActTime();
                        break;
                    case 8:
                        //Identify item
                        if(game.player->identifyItem(choice[9] - '0'))
                            game.lapsedTime += game.player->getActTime();
                        break;
                    case 9:
                        //Open door/chest
                        if(game.playerOpen(choice[5]))
                            game.lapsedTime += game.player->getActTime();
                        break;
                    case 10:
                        //Show object range
                        if(choice[6] - '0' >= game.player->getInventorySize()){
                            std::cout<<"\nThe index is not valid. Press enter to continue.\n";
                            fflush(stdin);
                            system("pause");
                        }else if(!game.player->getInventoryElementAt(choice[6] - '0').getIsIdentified()){
                            std::cout<<"\nThis item has to be identified first. Press enter to continue.\n";
                            fflush(stdin);
                            system("pause");
                        }else if(areStringsEqual(game.player->getInventoryElementAt(choice[6] - '0').getType(), "weapon")){
                            game.printRange(game.player->getWeaponAt(choice[6] - '0').getAreasOfEffect(), game.player->getWeaponAt(choice[6] - '0').getRange(), choice[8]);
                        }else if(areStringsEqual(game.player->getInventoryElementAt(choice[6] - '0').getType(), "scroll")){
                            game.printRange(game.player->getScrollAt(choice[6] - '0').getAreasOfEffect(), game.player->getScrollAt(choice[6] - '0').getRange(), choice[8]);
                        }else{
                            std::cout<<"\nYou have to indicate a weapon or a scroll. Press enter to continue.\n";
                            fflush(stdin);
                            system("pause");
                        }
                        break;
                    default:
                        break;
                }
                
            }else{
                std::cout<<"\nThe input is not valid. Press enter to retry.\n";
                fflush(stdin);
                system("pause");
            }
            if(error == 1){
                std::cout<<"\nYou can't move in this direction. Press enter to retry.\n";
                fflush(stdin);
                system("pause");
            }
        }while(actionType == 0 || error != 0);
        //Check if player died because of an ability check
        if(game.player->getHP() == 0){
            gameState = 1;
            break;
        }
        //Check if player won
        if(game.player->getX() == game.getExitX() &&  game.player->getY() == game.getExitY()){
            gameState = 2;
            break;
        } 
        //Move enemies
        if(game.lapsedTime > 0)
            game.moveEnemies();
        //Check if the player died after the enemy moves
        if(game.player->getHP() == 0){
            gameState = 1;
            break;
        }
    }
    if(gameState == 2){
        system("cls");
        int score = 0;
        for(int i = 1; i != game.player->getLvl(); i++){
            score += 100 * i;
        }
        score += 25 * game.player->getGp() + game.player->getExp();
        std::cout<<"\n\n\n                         \\(*-*)/ Congratulations! You won with a score of "<<score<<" \\(*-*)/\n\n\n";
        system("pause");
    }else{
        game.printInterface();
        fflush(stdin);
        system("pause");
        system("cls");
        std::cout<<"\n\n\n                          /(-.-)\\ You lost... Better luck next time! /(-.-)\\\n\n\n";
        system("pause");
    }
    
    return 0;
}