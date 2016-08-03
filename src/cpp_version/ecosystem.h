/*
  ==============================================================================

    ecosystem.h
    Created: 24 Apr 2016 10:13:01pm
    Author:  Emilio Molina

  ==============================================================================
*/

#ifndef ECOSYSTEM_H_INCLUDED
#define ECOSYSTEM_H_INCLUDED
#include <algorithm>
#include <vector>
#include <tuple>
#include <map>
#include <iostream>
#include <fstream>
#include <string>
#include <set>
#include <random>
#include <unordered_set>
#include <unordered_map>
#include <sstream>
#include <boost/filesystem.hpp>
#include "json.hpp"

namespace fs = boost::filesystem;
using namespace std;
using json = nlohmann::json;

extern string PLANT;      // typically "P"
extern string HERBIVORE;  // typically "H"
extern string CARNIVORE;  // typically "C"

extern map<string, int> BIOTOPE_SETTINGS;
extern map<string, float> ENERGY_COST;
extern map<string, float> MINIMUM_ENERGY_REQUIRED_TO;
extern map<string, float> PHOTOSYNTHESIS_CAPACITY;
extern map<string, int> INITIAL_NUM_OF_ORGANISMS;
extern map<string, int> MAX_LIFESPAN;
extern map<string, float> PROCREATION_PROBABILITY;
extern float INITIAL_ENERGY_RESERVE;


//************ HEADERS

class Organism;

/** @brief Class defining the environment where ecosystem can develop
*
* This is the class used in the main() function of the program.
*
*/
class Ecosystem {
public:
    // Public ttributes
    /** @brief Current time in ecosystem
    */
    int time;

    /** @brief Size of biotope in X axis
    */
    int biotope_size_x;

    /** @brief Size of biotope in Y axis
    */
    int biotope_size_y;

    /** @brief Main map of organisms in ecosystem
    *
    * Keys are (x, y) coordinates (position) and values are pointers
    * to Organisms living in ecosystem.
    */
    map<tuple<int, int>, Organism*> biotope;

    /** @brief Set of free locations in biotope
    *
    * Useful to easily find a new free position
    */
    set<tuple<int, int>> biotope_free_locs;

    // Public methods (documentation in ecosystem.cpp)
    Ecosystem();
    Ecosystem(const string& json_path);
    void addOrganism(Organism* organism);
    void removeOrganism(Organism* organism);
    void updateOrganismLocation(Organism* organism);
    void getSurroundingFreeLocations(tuple<int, int> center, vector<tuple<int, int>> &surrounding_free_locations);
    void getSurroundingOrganisms(tuple<int, int> center, vector<Organism*> &surrounding_organisms);
    void evolve();
    void serialize(json& data_json);
private:
    // Private attributes
    /** @brief Vector of dead organisms to be freed
    */
    vector<Organism*> _dead_organisms;

    /** @brief Initial number of plants in ecosystem
    */
    int _initial_num_plants;

    /** @brief Initial number of herbivores in ecosystem
    */
    int _initial_num_herbivores;

    /** @brief Initial number of carnivores in ecosystem
    */
    int _initial_num_carnivores;

    // Public methods (documentation in ecosystem.cpp)
    void _initializeBiotope();
    void _initializeOrganisms();
    void _initializeOrganisms(json& data_json);
    tuple<int, int> _getRandomFreeLocation();
    void _deleteDeadOrganisms();
};



/** @brief Class defining organism
*
* A lot of objects of this class are usually used in ecosystem
*/
class Organism {
public:
    // Public attributes
    /** @brief True if organism is still alive
    */
    bool is_alive;

    /** @brief Current location of organism
    */
    tuple<int, int> location;

    /** @brief Old location of organism (before moving)
    */
    tuple<int, int> old_location;

    /** @brief Species of this organism: PLANT, HERBIVORE or CARNIVORE
    */
    string species;

    /** @brief Energy reserve of the organism.
    *
    * If the energy reserve reaches 0, the organism dies because of starvation.
    */
    float energy_reserve;

    /** @brief Age of organism
    */
    int age;

    /** @brief Death age
    */
    int death_age;

    /** @brief Cause of death (in case it is dead)
    */
    string cause_of_death;

    /** @brief true if energy matters (typically YES)
    */
    bool is_energy_dependent;

    /** @brief Photosynthesis capacity
    */
    float photosynthesis_capacity;

    // Public methods (documentation in ecosystem.cpp)
    Organism(tuple<int, int> location, Ecosystem* parent_ecosystem, string& species, float energy_reserve);
    void act();

private:
    // Private attributes
    /** @brief Pointer to parent ecosystem
    */
    Ecosystem* _parent_ecosystem;

    // Private methods (documentation in ecosystem.cpp)
    void _do_photosynthesis();
    bool _has_enough_energy_to(const string &action);
    void _do_spend_energy(float amount_of_energy);
    void _do_move();
    bool _is_eatable(Organism* prey);
    void _do_hunt();
    void _do_procreate();
    void _do_age();
    void _do_die(const string &cause_of_death);
};

#endif  // ECOSYSTEM_H_INCLUDED
