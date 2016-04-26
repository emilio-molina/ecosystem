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
#include <string>
#include <set>
#include <random>
#include <time.h>
#include <unordered_set>
#include <unordered_map>

using namespace std;

enum species_t {PLANT, HERBIVORE, CARNIVORE};


// Ecosystem constraints
const map<string, int> BIOTOPE_SETTINGS = {
    {"size_x", 200},
    {"size_y", 200}
};

const map<string, float> ENERGY_COST = {
    {"to have the capacity of moving", 0.0f},
    {"to move", 5.0f},
    {"to have the capacity of hunting", 0.0f},
    {"to hunt", 10.0f},
    {"to have the capacity of procreating", 0.0f},
    {"to procreate", 1000.0f},
};

const map<string, float> MINIMUM_ENERGY_REQUIRED_TO = {
    {"move", 100.0f},
    {"hunt", 100.0f},
    {"procreate", 100.0f},
};

const float PHOTOSYNTHESIS_CAPACITY = 5.0f;

// Definition of gens grouped by species
const map<species_t, int> INITIAL_NUM_OF_ORGANISMS = {
    {PLANT, 100},
    {HERBIVORE, 100},
    {CARNIVORE, 100}
};

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

class Ecosystem {
    
public:
    bool rendered;
    int num_plants;
    int num_herbivores;
    int num_carnivores;
    int biotope_size_x;
    int biotope_size_y;
    map<tuple<int, int>, Organism*> biotope;
    set<tuple<int, int>> biotope_free_locs;
    int time;
    
    Ecosystem();
    void initializeBiotope();
    tuple<int, int> getRandomFreeLocation();
    void initializeOrganisms();
    void addOrganism(Organism* organism);
    void removeOrganism(Organism* organism);
    void updateOrganismLocation(Organism* organism);
    void getSurroundingFreeLocations(tuple<int, int> center, vector<tuple<int, int>> &surrounding_free_locations);
    void getSurroundingOrganisms(tuple<int, int> center, vector<Organism*> &surrounding_organisms);
    void evolve();
    
private:
    vector<Organism*> dead_organisms;
    void deleteDeadOrganisms();
};


class Organism {
public:
    Ecosystem* parent_ecosystem;
    tuple<int, int> location;
    tuple<int, int> old_location;

    species_t species;
    int death_age;
    
    int age;
    float energy_reserve;
    bool is_alive;
    string cause_of_death;
    
    bool is_energy_dependent;
    
    Organism(tuple<int, int> location, Ecosystem* parent_ecosystem, species_t species, float energy_reserve);
    void act();
    bool has_enough_energy_to(const string &action);
    bool is_eatable(Organism* prey);
    void do_photosynthesis();
    void do_move();
    void do_hunt();
    void do_procreate();
    void do_age();
    void do_spend_energy(float amount_of_energy);
    void do_die(const string &cause_of_death);
};



#endif  // ECOSYSTEM_H_INCLUDED
