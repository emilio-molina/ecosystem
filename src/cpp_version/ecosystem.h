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
#include <time.h>
#include <unordered_set>
#include <unordered_map>
#include <sstream>
#include <boost/filesystem.hpp>

using namespace std;
namespace fs = boost::filesystem;

enum species_t {PLANT, HERBIVORE, CARNIVORE};


// Ecosystem constraints
const map<string, int> BIOTOPE_SETTINGS = {
    {"size_x", 200},
    {"size_y", 200}
};

const map<string, float> ENERGY_COST = {
    {"to have the capability of moving", 0.0f},
    {"to move", 5.0f},
    {"to have the capability of hunting", 0.0f},
    {"to hunt", 10.0f},
    {"to have the capability of procreating", 0.0f},
    {"to procreate", 1000.0f},
};

const map<string, float> MINIMUM_ENERGY_REQUIRED_TO = {
    {"move", 100.0f},
    {"hunt", 100.0f},
    {"procreate", 100.0f},
};

const float PHOTOSYNTHESIS_CAPACITY = 5.0f;

const map<species_t, int> INITIAL_NUM_OF_ORGANISMS = {
    {PLANT, 100},
    {HERBIVORE, 100},
    {CARNIVORE, 100}
};

// Definition of gens grouped by species
const map<species_t, int> MAX_LIFESPAN = {
    {PLANT, 50},
    {HERBIVORE, 200},
    {CARNIVORE, 100}
};

const map<species_t, float> PROCREATION_PROBABILITY = {
    {PLANT, 0.15f},
    {HERBIVORE, 0.08f},
    {CARNIVORE, 0.08f}
};

const float INITIAL_ENERGY_RESERVE = 30000.0f;


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

    /** @brief True if current time slice has been already rendered
    *
    * @todo Move from here to the GUI code.
    */
    bool rendered;

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
    void addOrganism(Organism* organism);
    void removeOrganism(Organism* organism);
    void updateOrganismLocation(Organism* organism);
    void getSurroundingFreeLocations(tuple<int, int> center, vector<tuple<int, int>> &surrounding_free_locations);
    void getSurroundingOrganisms(tuple<int, int> center, vector<Organism*> &surrounding_organisms);
    void evolve();
    
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
    species_t species;

    /** @brief Energy reserve of the organism.
    *
    * If the energy reserve reaches 0, the organism dies because of starvation.
    */
    float energy_reserve;

    // Public methods (documentation in ecosystem.cpp)
    Organism(tuple<int, int> location, Ecosystem* parent_ecosystem, species_t species, float energy_reserve);
    void act();

private:
    // Private attributes
    /** @brief Pointer to parent ecosystem
    */
    Ecosystem* _parent_ecosystem;

    /** @brief Age of organism
    */
    int _age;

    /** @brief Death age
    */
    int _death_age;

    /** @brief Cause of death (in case it is dead)
    */
    string _cause_of_death;

    /** @brief true if energy matters (typically YES)
    */
    bool _is_energy_dependent;

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



/** @brief Class to export ecosystem to files
*/
class Exporter {
public:
    Exporter(Ecosystem* ecosystem, const string& dst_path);
    void exportInitialSettings();
    void exportTimeSlice();
private:
    Ecosystem* ecosystem;
    fs::path dst_path;
    void organismToString(Organism* organism);
};

#endif  // ECOSYSTEM_H_INCLUDED
