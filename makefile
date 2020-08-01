game.exe: Lib/main.o Lib/MapElements.o Lib/Characters.o Lib/InventoryElements.o
	g++ Lib/main.o Lib/MapElements.o Lib/Characters.o Lib/InventoryElements.o -o game.exe

Lib/main.o: main.cpp
	g++ -c main.cpp -o Lib/main.o

Lib/MapElements.o: Lib/MapElements.cpp Lib/MapElements.h
	g++ -c Lib/MapElements.cpp -o Lib/MapElements.o

Lib/Characters.o: Lib/Characters.cpp Lib/Characters.h
	g++ -c Lib/Characters.cpp -o Lib/Characters.o

Lib/InventoryElements.o: Lib/InventoryElements.cpp Lib/InventoryElements.h
	g++ -c Lib/InventoryElements.cpp -o Lib/InventoryElements.o

clean: 
	del /S *.o