
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

struct Data {
	void load(sf2::Deserializer<sf2::format::Json_reader>& s) {
		s.read_virtual(
		            sf2::vmember("a", a),
		            sf2::vmember("b", b) );
	}
	void save(sf2::Serializer<sf2::format::Json_writer>& s)const {
		s.write_virtual(
		            sf2::vmember("a", a),
		            sf2::vmember("b", b) );
	}

	float a;
	bool b;
};

/* Virtual struct
 * - id: std::string
 * - data: Data
 * - players: std::vector<Player>
 */

int main() {
	std::cout<<"Test_complex:"<<std::endl;

	std::cout<<"is_range: "<<sf2::details::is_list<std::vector<Player>>::value<<std::endl;

	std::string id = "test";

	Data data{42.1, true};

	Player player {Position{5,2,1}, Color::GREEN, "The first player is \"/%&ÄÖ\""};

	std::vector<Player> players {player};

	sf2::serialize_virtual(sf2::format::Json_writer{std::cout},
			sf2::vmember("id", id),
			sf2::vmember("data", data),
			sf2::vmember(std::string("players"), players));


	std::string str = R"({
    "id": "test",
    "data": {
        "a": 42.1,
        "b": true
    },
    "players": [
        {
            "position": {
                "x": 5,
                "y": 2,
                "z": 1
            },
            "color": "GREEN",
            "name": "The first player is \"/%&ÄÖ\""
        }
    ]
}
)";

	auto istream = std::istringstream{str};

	sf2::deserialize_virtual(sf2::format::Json_reader{istream},
			sf2::vmember("id", id),
			sf2::vmember("data", data),
			sf2::vmember("players", players));


	std::stringstream out;

	sf2::serialize_virtual(sf2::format::Json_writer{out},
			sf2::vmember("id", id),
			sf2::vmember("data", data),
			sf2::vmember("players", players));

	assert(out.str()==str && "generated string doesn't match expected result");

	std::cout<<"success"<<std::endl;
}
