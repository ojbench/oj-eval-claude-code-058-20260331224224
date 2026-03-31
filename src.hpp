#ifndef POKEDEX_HPP
#define POKEDEX_HPP

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cstring>
#include <vector>
#include <algorithm>
#include <set>
#include <map>

// Exception classes
class BasicException {
protected:
    std::string message;

public:
    explicit BasicException(const char *_message) : message(_message) {}

    virtual const char *what() const {
        return message.c_str();
    }
};

class ArgumentException : public BasicException {
public:
    explicit ArgumentException(const char *_message) : BasicException(_message) {}
};

class IteratorException : public BasicException {
public:
    explicit IteratorException(const char *_message) : BasicException(_message) {}
};

// Pokemon structure
struct Pokemon {
    char name[12];
    int id;
    std::vector<std::string> types;

    Pokemon() : id(0) {
        memset(name, 0, sizeof(name));
    }

    Pokemon(const char* _name, int _id, const std::vector<std::string>& _types)
        : id(_id), types(_types) {
        strncpy(name, _name, 11);
        name[11] = '\0';
    }
};

// Type effectiveness table
class TypeChart {
private:
    std::map<std::pair<std::string, std::string>, float> effectiveness;
    std::set<std::string> validTypes;

public:
    TypeChart() {
        // Valid types
        validTypes = {"fire", "water", "grass", "electric", "ground", "flying", "dragon"};

        // Initialize all to 1.0
        for (const auto& attacker : validTypes) {
            for (const auto& defender : validTypes) {
                effectiveness[{attacker, defender}] = 1.0f;
            }
        }

        // Super effective (2x)
        effectiveness[{"fire", "grass"}] = 2.0f;
        effectiveness[{"water", "fire"}] = 2.0f;
        effectiveness[{"water", "ground"}] = 2.0f;
        effectiveness[{"grass", "water"}] = 2.0f;
        effectiveness[{"grass", "ground"}] = 2.0f;
        effectiveness[{"electric", "water"}] = 2.0f;
        effectiveness[{"electric", "flying"}] = 2.0f;
        effectiveness[{"ground", "fire"}] = 2.0f;
        effectiveness[{"ground", "electric"}] = 2.0f;
        effectiveness[{"dragon", "dragon"}] = 2.0f;

        // Not very effective (0.5x)
        effectiveness[{"fire", "water"}] = 0.5f;
        effectiveness[{"fire", "fire"}] = 0.5f;
        effectiveness[{"water", "grass"}] = 0.5f;
        effectiveness[{"water", "dragon"}] = 0.5f;
        effectiveness[{"grass", "fire"}] = 0.5f;
        effectiveness[{"grass", "flying"}] = 0.5f;
        effectiveness[{"electric", "grass"}] = 0.5f;
        effectiveness[{"electric", "dragon"}] = 0.5f;

        // No effect (0x)
        effectiveness[{"ground", "flying"}] = 0.0f;
        effectiveness[{"electric", "ground"}] = 0.0f;
    }

    float getEffectiveness(const std::string& attackType, const std::string& defenseType) const {
        auto it = effectiveness.find({attackType, defenseType});
        if (it != effectiveness.end()) {
            return it->second;
        }
        return 1.0f;
    }

    bool isValidType(const std::string& type) const {
        return validTypes.find(type) != validTypes.end();
    }
};

static TypeChart typeChart;

// Pokedex class
class Pokedex {
private:
    std::string fileName;
    std::map<int, Pokemon> pokedex; // id -> Pokemon

    // Helper functions
    bool isValidName(const char* name) const {
        if (!name || name[0] == '\0') return false;
        for (int i = 0; name[i] != '\0'; i++) {
            if (!((name[i] >= 'A' && name[i] <= 'Z') || (name[i] >= 'a' && name[i] <= 'z'))) {
                return false;
            }
        }
        return true;
    }

    std::vector<std::string> parseTypes(const char* types) const {
        std::vector<std::string> result;
        std::string str(types);
        std::stringstream ss(str);
        std::string token;

        while (std::getline(ss, token, '#')) {
            result.push_back(token);
        }

        return result;
    }

    void saveToFile() {
        std::ofstream file(fileName);
        if (!file.is_open()) return;

        for (const auto& pair : pokedex) {
            const Pokemon& pm = pair.second;
            file << pm.name << " " << pm.id;
            for (size_t i = 0; i < pm.types.size(); i++) {
                file << (i == 0 ? " " : "#") << pm.types[i];
            }
            file << "\n";
        }

        file.close();
    }

    void loadFromFile() {
        std::ifstream file(fileName);
        if (!file.is_open()) return;

        std::string line;
        while (std::getline(file, line)) {
            if (line.empty()) continue;

            std::stringstream ss(line);
            std::string name;
            int id;
            std::string typesStr;

            ss >> name >> id >> typesStr;

            std::vector<std::string> types = parseTypes(typesStr.c_str());
            Pokemon pm(name.c_str(), id, types);
            pokedex[id] = pm;
        }

        file.close();
    }

public:
    explicit Pokedex(const char *_fileName) : fileName(_fileName) {
        loadFromFile();
    }

    ~Pokedex() {
        saveToFile();
    }

    bool pokeAdd(const char *name, int id, const char *types) {
        // Validate name
        if (!isValidName(name)) {
            std::string msg = std::string("Argument Error: PM Name Invalid (") + name + ")";
            throw ArgumentException(msg.c_str());
        }

        // Validate and parse types
        std::vector<std::string> typeList = parseTypes(types);
        for (const auto& type : typeList) {
            if (!typeChart.isValidType(type)) {
                std::string msg = std::string("Argument Error: PM Type Invalid (") + type + ")";
                throw ArgumentException(msg.c_str());
            }
        }

        // Check if id already exists
        if (pokedex.find(id) != pokedex.end()) {
            return false;
        }

        // Check if name already exists
        for (const auto& pair : pokedex) {
            if (strcmp(pair.second.name, name) == 0) {
                return false;
            }
        }

        Pokemon pm(name, id, typeList);
        pokedex[id] = pm;
        return true;
    }

    bool pokeDel(int id) {
        auto it = pokedex.find(id);
        if (it == pokedex.end()) {
            return false;
        }
        pokedex.erase(it);
        return true;
    }

    std::string pokeFind(int id) const {
        auto it = pokedex.find(id);
        if (it == pokedex.end()) {
            return "None";
        }
        return std::string(it->second.name);
    }

    std::string typeFind(const char *types) const {
        // Validate and parse types
        std::vector<std::string> searchTypes = parseTypes(types);
        for (const auto& type : searchTypes) {
            if (!typeChart.isValidType(type)) {
                std::string msg = std::string("Argument Error: PM Type Invalid (") + type + ")";
                throw ArgumentException(msg.c_str());
            }
        }

        // Find Pokemon with all specified types
        std::vector<Pokemon> results;
        for (const auto& pair : pokedex) {
            const Pokemon& pm = pair.second;
            bool hasAllTypes = true;

            for (const auto& searchType : searchTypes) {
                bool hasType = false;
                for (const auto& pmType : pm.types) {
                    if (pmType == searchType) {
                        hasType = true;
                        break;
                    }
                }
                if (!hasType) {
                    hasAllTypes = false;
                    break;
                }
            }

            if (hasAllTypes) {
                results.push_back(pm);
            }
        }

        if (results.empty()) {
            return "None";
        }

        // Sort by id
        std::sort(results.begin(), results.end(), [](const Pokemon& a, const Pokemon& b) {
            return a.id < b.id;
        });

        // Build result string
        std::stringstream ss;
        ss << results.size() << "\n";
        for (const auto& pm : results) {
            ss << pm.name << "\n";
        }

        std::string result = ss.str();
        if (!result.empty() && result[result.length() - 1] == '\n') {
            result = result.substr(0, result.length() - 1);
        }
        return result;
    }

    float attack(const char *type, int id) const {
        // Find the Pokemon
        auto it = pokedex.find(id);
        if (it == pokedex.end()) {
            return -1.0f;
        }

        const Pokemon& defender = it->second;
        std::string attackType(type);

        // Calculate total damage multiplier
        float totalMultiplier = 1.0f;
        for (const auto& defenseType : defender.types) {
            totalMultiplier *= typeChart.getEffectiveness(attackType, defenseType);
        }

        return totalMultiplier;
    }

    int catchTry() const {
        if (pokedex.empty()) {
            return 0;
        }

        // Start with Pokemon with smallest id
        std::set<int> owned;
        int minId = pokedex.begin()->first;
        owned.insert(minId);

        bool changed = true;
        while (changed) {
            changed = false;

            for (const auto& pair : pokedex) {
                int targetId = pair.first;

                // Skip if already owned
                if (owned.find(targetId) != owned.end()) {
                    continue;
                }

                const Pokemon& target = pair.second;

                // Check if any owned Pokemon can deal 2x damage
                bool canCatch = false;
                for (int ownedId : owned) {
                    const Pokemon& owned_pm = pokedex.at(ownedId);

                    // Try each type of owned Pokemon
                    for (const auto& attackType : owned_pm.types) {
                        float damage = 1.0f;
                        for (const auto& defenseType : target.types) {
                            damage *= typeChart.getEffectiveness(attackType, defenseType);
                        }

                        if (damage >= 2.0f) {
                            canCatch = true;
                            break;
                        }
                    }

                    if (canCatch) break;
                }

                if (canCatch) {
                    owned.insert(targetId);
                    changed = true;
                }
            }
        }

        return owned.size();
    }

    // Iterator
    struct iterator {
        std::map<int, Pokemon>::iterator it;
        std::map<int, Pokemon>* container;

        iterator() : container(nullptr) {}
        iterator(std::map<int, Pokemon>::iterator _it, std::map<int, Pokemon>* _container)
            : it(_it), container(_container) {}

        iterator &operator++() {
            if (container == nullptr || it == container->end()) {
                throw IteratorException("Iterator Error: Iterator Out of Bound");
            }
            ++it;
            return *this;
        }

        iterator &operator--() {
            if (container == nullptr || it == container->begin()) {
                throw IteratorException("Iterator Error: Iterator Out of Bound");
            }
            --it;
            return *this;
        }

        iterator operator++(int) {
            if (container == nullptr || it == container->end()) {
                throw IteratorException("Iterator Error: Iterator Out of Bound");
            }
            iterator temp = *this;
            ++it;
            return temp;
        }

        iterator operator--(int) {
            if (container == nullptr || it == container->begin()) {
                throw IteratorException("Iterator Error: Iterator Out of Bound");
            }
            iterator temp = *this;
            --it;
            return temp;
        }

        iterator& operator=(const iterator &rhs) {
            it = rhs.it;
            container = rhs.container;
            return *this;
        }

        bool operator==(const iterator &rhs) const {
            return it == rhs.it;
        }

        bool operator!=(const iterator &rhs) const {
            return it != rhs.it;
        }

        Pokemon& operator*() const {
            if (container == nullptr || it == container->end()) {
                throw IteratorException("Iterator Error: Invalid Dereference");
            }
            return it->second;
        }

        Pokemon* operator->() const {
            if (container == nullptr || it == container->end()) {
                throw IteratorException("Iterator Error: Invalid Dereference");
            }
            return &(it->second);
        }
    };

    iterator begin() {
        return iterator(pokedex.begin(), &pokedex);
    }

    iterator end() {
        return iterator(pokedex.end(), &pokedex);
    }
};

#endif // POKEDEX_HPP
