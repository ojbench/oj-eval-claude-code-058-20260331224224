#include <iostream>
#include <cstring>
#include "src.hpp"

using namespace std;

int main() {
    try {
        Pokedex dex("test.txt");

        // Test basic add
        cout << "Adding Pikachu: " << dex.pokeAdd("Pikachu", 25, "electric") << endl;
        cout << "Adding Bulbasaur: " << dex.pokeAdd("Bulbasaur", 1, "grass") << endl;
        cout << "Adding Charmander: " << dex.pokeAdd("Charmander", 4, "fire") << endl;
        cout << "Adding Squirtle: " << dex.pokeAdd("Squirtle", 7, "water") << endl;

        // Test find
        cout << "Find 25: " << dex.pokeFind(25) << endl;
        cout << "Find 1: " << dex.pokeFind(1) << endl;
        cout << "Find 999: " << dex.pokeFind(999) << endl;

        // Test type find
        cout << "Find electric: " << endl << dex.typeFind("electric") << endl;

        // Test attack
        cout << "Attack electric on 7: " << dex.attack("electric", 7) << endl;

        // Test iterator
        cout << "Iterator test:" << endl;
        for (auto it = dex.begin(); it != dex.end(); ++it) {
            cout << "  " << it->name << " (id=" << it->id << ")" << endl;
        }

        // Test catchTry
        cout << "CatchTry: " << dex.catchTry() << endl;

    } catch (const BasicException& e) {
        cout << "Exception: " << e.what() << endl;
    }

    return 0;
}
