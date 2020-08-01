#include "Lib/MapElements.h"
#include <Windows.h>
#include <limits>

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
        /*for(int i = 0; i != MAX_MATRIX_HEIGHT; i++){
            for(int j = 0; j != MAX_MATRIX_WIDTH; j++){
                std::cout<<"\nI: "<<i<<"   J: "<<j<<"       ";
                std::cout<<abs(game.m[i][j]);
            }
        }
        game.printMatrix();
        return 0;*/
        game.chooseClass();
    }

     std::cout<<"\n1\n";

    
    //Game loop
    gameState = 0; //1=lost, 2=won
    int error = 0;
    
    unsigned int actionType;
    //system("cls");
    while(gameState == 0){
        do{
            std::cout<<"\n1\n";
            //Clear the fog around the player
            game.getPlayer()->setCoordinates(6, 89);
            game.clearFog();
            std::cout<<"\nI:"<<game.getPlayer()->getY()<<" J:"<<game.getPlayer()->getX()<<"\n\n";
            game.printInterface();
            std::string choice = "";
            std::getline(std::cin, choice);
            std::cout<<"\n2\n";
            //system("cls");
            std::cout<<"\n3\n";
            actionType = game.getPlayer()->input(choice);
            std::cout<<"\n4\n";
            //If the user inserted a valid action
            if(actionType != 0){
                std::cout<<"\n5\n";
                error = 0;
                switch (actionType){
                    case 1: 
                        //Player movement
                        switch (game.getPlayer()->getMovementIntention()){
                            case 1: //Up
                                if(game.isWalkable(game.getPlayer()->getX(), game.getPlayer()->getY() - 1))
                                    game.getPlayer()->setCoordinates(game.getPlayer()->getX(), game.getPlayer()->getY() - 1);
                                else
                                    error = 1;
                                break;
                            case 2: //Down
                                if(game.isWalkable(game.getPlayer()->getX(), game.getPlayer()->getY() + 1))
                                    game.getPlayer()->setCoordinates(game.getPlayer()->getX(), game.getPlayer()->getY() + 1);
                                else
                                    error = 1;
                                break;
                            case 3: //Left
                                if(game.isWalkable(game.getPlayer()->getX() - 1, game.getPlayer()->getY()))
                                    game.getPlayer()->setCoordinates(game.getPlayer()->getX() - 1, game.getPlayer()->getY());
                                else
                                    error = 1;
                                break;
                            case 4: //Right
                                if(game.isWalkable(game.getPlayer()->getX() + 1, game.getPlayer()->getY())){
                                    game.getPlayer()->setCoordinates(game.getPlayer()->getX() + 1, game.getPlayer()->getY());
                                }else
                                    error = 1;
                                break;
                        }
                        if(error == 0){
                            game.lapsedTime += game.getPlayer()->getMovTime();
                        }
                        game.getPlayer()->resetMovementIntention();
                        break;
                    case 2:
                        //Equip item
                        if(game.getPlayer()->equipItem((choice[6]) - '0'))
                            game.lapsedTime += game.getPlayer()->getActTime();
                        break;
                    case 3:
                        //Attack
                        game.playerAttack(choice[4]); //Even if player hits none, time passes
                        game.lapsedTime += game.getPlayer()->getActTime();
                        break;
                    case 4:
                        //Cast spell
                        if(game.playerCastSpell(choice[7], choice[5] - '0'))
                            game.lapsedTime += game.getPlayer()->getActTime();
                        break;
                    case 5:
                        //Use item
                        if(game.playerUse(choice[4] - '0'))
                            game.lapsedTime += game.getPlayer()->getActTime();
                       break;
                    case 6:
                        //Loot items
                        if(game.getPlayer()->getInventorySize() >= 10){
                            std::cout<<"\nYour inventory is full, you can't loot more items.";
                            system("pause");
                        }else{
                            game.playerLoot();
                            game.lapsedTime += game.getPlayer()->getActTime();
                        }
                        break;
                    case 7:
                        //Discard item
                        if(game.getPlayer()->discardItem(choice[8] - '0'))
                            game.lapsedTime += game.getPlayer()->getActTime();
                        break;
                    case 8:
                        //Identify item
                        if(game.getPlayer()->identifyItem(choice[9] - '0'))
                            game.lapsedTime += game.getPlayer()->getActTime();
                        break;
                    case 9:
                        //Open door/chest
                        if(game.playerOpen(choice[5]))
                            game.lapsedTime += game.getPlayer()->getActTime();
                        break;
                    case 10:
                        //Show object range
                        if(choice[6] - '0' >= game.getPlayer()->getInventorySize()){
                            std::cout<<"\nThe index is not valid. Press enter to continue.\n";
                            fflush(stdin);
                            system("pause");
                        }else if(!game.getPlayer()->getInventoryElementAt(choice[6] - '0').getIsIdentified()){
                            std::cout<<"\nThis item has to be identified first. Press enter to continue.\n";
                            fflush(stdin);
                            system("pause");
                        }else if(areStringsEqual(game.getPlayer()->getInventoryElementAt(choice[6] - '0').getType(), "weapon")){
                            game.printRange(game.getPlayer()->getWeaponAt(choice[6] - '0').getAreasOfEffect(), game.getPlayer()->getWeaponAt(choice[6] - '0').getRange(), choice[8]);
                        }else if(areStringsEqual(game.getPlayer()->getInventoryElementAt(choice[6] - '0').getType(), "scroll")){
                            game.printRange(game.getPlayer()->getScrollAt(choice[6] - '0').getAreasOfEffect(), game.getPlayer()->getScrollAt(choice[6] - '0').getRange(), choice[8]);
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
            std::cout<<"\n6\n";
        }while(actionType == 0 || error != 0);
        std::cout<<"\n7\n";
        //Check if player died because of an ability check
        if(game.getPlayer()->getHP() == 0){
            gameState = 1;
            break;
        }
        std::cout<<"\n8\n";
        //Check if player won
        if(game.getPlayer()->getX() == game.getExitX() &&  game.getPlayer()->getY() == game.getExitY()){
            gameState = 2;
            break;
        } 
        std::cout<<"\n9\n";
        //Move enemies
        std::cout<<"moving enemies...";
        if(game.lapsedTime > 0)
            game.moveEnemies();
        std::cout<<"enemies moved";
        //Check if the player died after the enemy moves
        if(game.getPlayer()->getHP() == 0){
            gameState = 1;
            break;
        }
        std::cout<<"\n10\n";
    }
    std::cout<<"\n11\n";
    if(gameState == 2){
        //system("cls");
        int score = 0;
        for(int i = 1; i != game.getPlayer()->getLvl(); i++){
            score += 100 * i;
        }
        score += 25 * game.getPlayer()->getGp() + game.getPlayer()->getExp();
        std::cout<<"\n\n\n                         \\(*-*)/ Congratulations! You won with a score of "<<score<<" \\(*-*)/\n\n\n";
        system("pause");
    }else{
        game.printInterface();
        fflush(stdin);
        system("pause");
        //system("cls");
        std::cout<<"\n\n\n                          /(-.-)\\ You lost... Better luck next time! /(-.-)\\\n\n\n";
        system("pause");
    }
    //Free pointers
    delete game.getPlayer();
    
    return 0;
}