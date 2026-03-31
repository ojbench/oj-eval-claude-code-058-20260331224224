#include <iostream>
#include <cstring>
#include "src.hpp"

using namespace std;

int main() {
    try {
        Pokedex dex("test2.txt");

        // Test multi-type Pokemon
        cout << "Adding Garchomp (ground#dragon): " << dex.pokeAdd("Garchomp", 445, "ground#dragon") << endl;
        cout << "Adding Dragonite (dragon#flying): " << dex.pokeAdd("Dragonite", 149, "dragon#flying") << endl;

        // Test attack on multi-type
        cout << "Dragon attack on Dragonite (dragon#flying): " << dex.attack("dragon", 149) << endl;
        cout << "Ground attack on Dragonite: " << dex.attack("ground", 149) << endl;

    } catch (const BasicException& e) {
        cout << "Exception: " << e.what() << endl;
    }

    return 0;
}
