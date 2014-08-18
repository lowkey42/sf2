
#include <iostream>
#include <sf2/sf2.hpp>

enum class Color {
	RED, GREEN, BLUE
};
sf2_enumDef(Color,
	sf2_value(RED),
	sf2_value(GREEN),
	sf2_value(BLUE)
);

struct Position {
	float x, y, z;
};
sf2_structDef(Position,
	sf2_member(x),
	sf2_member(y),
	sf2_member(z)
);

struct Player {
	Position position;
	Color color;
	std::string name;
};
sf2_structDef(Player,
	sf2_member(position),
	sf2_member(color),
	sf2_member(name)
);



int main() {
	std::cout<<"Test01:"<<std::endl;

	Player player1 {Position{5,2,1}, Color::GREEN, "The first \nplayer \""};

	auto player1AsString = sf2::writeString(player1);

	std::cout<<"player1:  "<<player1AsString<<std::endl;

	Player parsedPlayer = sf2::parseString<Player>(player1AsString);

	std::cout<<"parsedPlayer:  "<<sf2::writeString(parsedPlayer)<<std::endl;
}
