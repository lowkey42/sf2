
#include <iostream>
#include <cassert>
#include <sstream>

//#include <sf2/sf2.hpp>
#include <sf2/reflection.hpp>
#include <sf2/serializer.hpp>
#include <sf2/formats/json_writer.hpp>
#include <sf2/formats/json_reader.hpp>


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

	Player player1 {Position{5,2,1}, Color::GREEN, "The first player is \"/%&ÄÖ\""};

	sf2::serialize(sf2::format::Json_writer{std::cout}, player1);

	std::string str = R"({
    "position": {
        "x": 5,
        "y": 2,
        "z": 1
    },
    "color": "GREEN",
    "name": "The first player is \"/%&ÄÖ\""
}
)";

	auto istream = std::istringstream{str};

	Player player2;
	sf2::deserialize(sf2::format::Json_reader{istream}, player2);

	std::stringstream out;

	sf2::serialize(sf2::format::Json_writer{out}, player2);

	assert(out.str()==str && "generated string doesn't match expected result");

	std::cout<<"success"<<std::endl;
}
