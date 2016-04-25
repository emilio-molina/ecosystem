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
    map<string, int> BIOTOPE_SETTINGS;
    map<species_t, int> INITIAL_NUM_OF_ORGANISMS;
    float INITIAL_ENERGY_RESERVE;
    
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
    map<string, float> ENERGY_COST;
    map<string, float> MINIMUM_ENERGY_REQUIRED_TO;
    map<species_t, int> MAX_LIFESPAN;
    map<species_t, float> PROCREATION_PROBABILITY;
    float PHOTOSYNTHESIS_CAPACITY;

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
