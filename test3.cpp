#include <iostream>
#include <cstring>
#include "src.hpp"

using namespace std;

int main() {
    try {
        Pokedex dex("test3.txt");

        // Test invalid name
        try {
            dex.pokeAdd("pm1", 1, "water");
        } catch (const ArgumentException& e) {
            cout << "Caught: " << e.what() << endl;
        }

        // Test invalid type
        try {
            dex.pokeAdd("Valid", 2, "water#ice");
        } catch (const ArgumentException& e) {
            cout << "Caught: " << e.what() << endl;
        }

        // Test iterator exception
        try {
            auto it = dex.end();
            cout << (*it).name << endl;
        } catch (const IteratorException& e) {
            cout << "Caught: " << e.what() << endl;
        }

    } catch (const BasicException& e) {
        cout << "Unexpected exception: " << e.what() << endl;
    }

    return 0;
}
