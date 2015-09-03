
#include <iostream>
#include <cassert>

#include <sf2/sf2.hpp>

enum class Color {
	RED, GREEN, BLUE
};
sf2_enumDef(Color, RED, GREEN, BLUE)

struct Position {
	float x, y, z;
};
sf2_structDef(Position, x, y, z)

struct Player {
	Position position;
	Color color;
	std::string name;
};
sf2_structDef(Player, position, color, name)


int main() {
	std::cout<<"Test01:"<<std::endl;

	//Player player1 {Position{5,2,1}, Color::GREEN, "The first player is \"ÄÖÜ ß öäü ẑ\"⸮"};
	Player player1 {Position{5,2,1}, Color::GREEN, "The first player is"};

	auto player1AsString = sf2::writeString(player1);

	std::cout<<"player1:  "<<player1AsString<<std::endl;

	Player parsedPlayer = sf2::parseString<Player>(player1AsString);

	std::cout<<"parsedPlayer:  "<<sf2::writeString(parsedPlayer)<<std::endl;

	assert(sf2::writeString(parsedPlayer)==player1AsString);
}
