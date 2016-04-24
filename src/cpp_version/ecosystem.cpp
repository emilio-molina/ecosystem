/** @file ecosystem.cpp
 * @brief Everything is in this file
 *
 */

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

default_random_engine eng((random_device())());

enum species_t {PLANT, HERBIVORE, CARNIVORE};

// Ecosystem constraints
map<string, int> BIOTOPE_SETTINGS = {
    {"size_x", 200},
    {"size_y", 200}
};

map<string, float> ENERGY_COST = {
    {"to have the capacity of moving", 2.0f},
    {"to move", 5.0f},
    {"to have the capacity of hunting", 4.0f},
    {"to hunt", 10.0f},
    {"to have the capacity of procreating", 0.0f},
    {"to procreate", 15.0f},
};

map<string, float> MINIMUM_ENERGY_REQUIRED_TO = {
    {"move", 30.0f},
    {"hunt", 30.0f},
    {"procreate", 100.0f},
};

float PHOTOSYNTHESIS_CAPACITY = 5.0f;

// Definition of gens grouped by species
map<species_t, int> INITIAL_NUM_OF_ORGANISMS = {
    {PLANT, 3},
    {HERBIVORE, 3},
    {CARNIVORE, 3}
};

map<species_t, int> MAX_LIFESPAN = {
    {PLANT, 40},
    {HERBIVORE, 35},
    {CARNIVORE, 100}
};

map<species_t, float> PROCREATION_PROBABILITY = {
    {PLANT, 0.5f},
    {HERBIVORE, 0.1f},
    {CARNIVORE, 0.02f}
};

const float INITIAL_ENERGY_RESERVE = 100.0f;

//************ HEADERS

class Organism;

class Ecosystem {

public:
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

//******* IMPLEMENTATION

Organism::Organism(tuple<int, int> location, Ecosystem* parent_ecosystem, species_t species, float energy_reserve) {

    // Relative to parent_ecosystem:
    this->parent_ecosystem = parent_ecosystem;
    this->location = location;
    this->old_location = location;

    // Genes:
    this->species = species;
    this->death_age = rand() % MAX_LIFESPAN[this->species];

    // State:
    this->age = 0;
    this->energy_reserve = energy_reserve;
    this->is_alive = true;
    this->cause_of_death = "";
    this->is_energy_dependent = true;
}

void Organism::act() {

    // If PLANT: do_photosynthesis
    if (this->species == PLANT) {
        this->do_photosynthesis();
    }

    // Move
    this->do_move();
    
    // Hunt
    if (!this->is_alive)
        return;
    this->do_hunt();
    
    // Procreate
    if (!this->is_alive)
        return;
    this->do_procreate();
    
    // Procreate
    if (!this->is_alive)
        return;
    this->do_age();
}
    
void Organism::do_photosynthesis() {
    if (this->is_energy_dependent)
        this->energy_reserve = this->energy_reserve + PHOTOSYNTHESIS_CAPACITY;
}
    
bool Organism::has_enough_energy_to(const string &action) {
    return (this->energy_reserve > MINIMUM_ENERGY_REQUIRED_TO[action]);
}

void Organism::do_spend_energy(float amount_of_energy) {
    this->energy_reserve = this->energy_reserve - amount_of_energy;
    if (this->energy_reserve <= 0) {
        this->do_die("starvation");
    }
}

void Organism::do_move() {
    if (this->species==PLANT)
        return;

    // If it is energy dependent
    if (is_energy_dependent) {
        if (this->has_enough_energy_to("move")) {
            this->do_spend_energy(ENERGY_COST["to have the capacity of moving"]);
        }
        if (!this->is_alive)
            return;
    }
    
    vector<tuple<int, int>> surrounding_free_locations;
    this->parent_ecosystem->getSurroundingFreeLocations(this->location, surrounding_free_locations);
    if (surrounding_free_locations.size() > 0) {
        if (this->is_energy_dependent) {
            this->do_spend_energy(ENERGY_COST["to move"]);
            if (!this->is_alive)
                return;
        }
        tuple<int, int> new_location = surrounding_free_locations[0];
        this->location = new_location;
        this->parent_ecosystem->updateOrganismLocation(this);
    }
}


bool Organism::is_eatable(Organism* prey) {
    bool is_eatable = false;
    if ((this->species == CARNIVORE) && (this->species == HERBIVORE))
        is_eatable = true;
    if ((this->species == HERBIVORE) && (this->species == PLANT))
        is_eatable = true;
    return is_eatable;
}


void Organism::do_hunt() {
    if (this->species == PLANT)
        return;  // plants don't hunt
    
    if (this->is_energy_dependent) {
        if (this->has_enough_energy_to("hunt"))
            this->do_spend_energy(ENERGY_COST["to have the capability of hunting"]);
        if (!this->is_alive)
            return;
    }
    
    vector<Organism*> surrounding_organisms;
    this->parent_ecosystem->getSurroundingOrganisms(this->location, surrounding_organisms);
    for (auto surr_organism:surrounding_organisms) {
        if (this->is_eatable(surr_organism)) {
            Organism* prey = surr_organism;
            // other possibility: this->age = 0;
            this->energy_reserve = this->energy_reserve + prey->energy_reserve;
            prey->do_die("hunted");
        }
    }
}

void Organism::do_procreate() {
    if (this->is_energy_dependent) {
        if (this->has_enough_energy_to("procreate"))
            this->do_spend_energy(ENERGY_COST["to have the capability of procreating"]);
        if (!this->is_alive)
            return;  // may have died because of starvation
    }
    uniform_real_distribution<float> fdis(0, 1.0);
    float random_value = fdis(eng);
    
    if (random_value >= PROCREATION_PROBABILITY[this->species])  // do not procreate
        return;

    vector<tuple<int, int>> free_locs;
    this->parent_ecosystem->getSurroundingFreeLocations(this->location, free_locs);
    if (free_locs.size() == 0)
        return;

    tuple<int, int> baby_location = free_locs[0];
    float baby_energy_reserve = this->energy_reserve / 2.0f;
    this->energy_reserve = this->energy_reserve / 2.0f;
    Organism* baby = new Organism(baby_location, this->parent_ecosystem, this->species, baby_energy_reserve);
    this->parent_ecosystem->addOrganism(baby);
    if (this->is_energy_dependent)
        this->do_spend_energy(ENERGY_COST["to procreate"]);
}

void Organism::do_age() {
    this->age += 1;
    if (this->age > this->death_age)
        this->do_die("age");
}


void Organism::do_die(const string &cause_of_death) {
    this->is_alive = false;
    this->parent_ecosystem->removeOrganism(this);
}


Ecosystem::Ecosystem() {
    this->num_plants = INITIAL_NUM_OF_ORGANISMS[PLANT];
    this->num_herbivores = INITIAL_NUM_OF_ORGANISMS[HERBIVORE];
    this->num_carnivores = INITIAL_NUM_OF_ORGANISMS[CARNIVORE];
    this->biotope_size_x = BIOTOPE_SETTINGS["size_x"];
    this->biotope_size_y = BIOTOPE_SETTINGS["size_y"];
    this->initializeBiotope();
    this->initializeOrganisms();
    this->time = 0;
}
    
void Ecosystem::initializeBiotope() {
    for (int x = 0; x < this->biotope_size_x; x++) {
        for (int y = 0; y < this->biotope_size_y; y++) {
            this->biotope_free_locs.insert(make_tuple(x, y));
        }
    }
}
    
    
tuple<int, int> Ecosystem::getRandomFreeLocation() {
    auto it = this->biotope_free_locs.begin();
    int r = rand() % this->biotope_free_locs.size();
    advance(it, r);
    return *it;
}
    
void Ecosystem::initializeOrganisms() {
    // Create plants
    for (int i = 0; i < this->num_plants; i++) {
        tuple<int, int> rand_location = this->getRandomFreeLocation();
        this->addOrganism(new Organism(rand_location, this, PLANT, INITIAL_ENERGY_RESERVE));
    }

    // Create herbivores
    for (int i = 0; i < this->num_herbivores; i++) {
        tuple<int, int> rand_location = this->getRandomFreeLocation();
        this->addOrganism(new Organism(rand_location, this, HERBIVORE, INITIAL_ENERGY_RESERVE));
    }

    // Create carnivores
    for (int i = 0; i < this->num_carnivores; i++) {
        tuple<int, int> rand_location = this->getRandomFreeLocation();
        this->addOrganism(new Organism(rand_location, this, CARNIVORE, INITIAL_ENERGY_RESERVE));
    }
}
    
void Ecosystem::addOrganism(Organism* organism) {
    this->biotope[organism->location] = organism;
    this->biotope_free_locs.erase(organism->location);
}
    
void Ecosystem::removeOrganism(Organism* organism) {
    this->biotope.erase(organism->location);
    this->biotope_free_locs.insert(organism->location);
    this->dead_organisms.push_back(organism);
}


    
void Ecosystem::updateOrganismLocation(Organism* organism){
    this->biotope.erase(organism->old_location);
    this->biotope_free_locs.insert(organism->old_location);
    this->biotope[organism->location] = organism;
    this->biotope_free_locs.erase(organism->location);
    organism->old_location = organism->location;
}
    
void Ecosystem::getSurroundingFreeLocations(tuple<int, int> center, vector<tuple<int, int>> &surrounding_free_locations) {
    int center_x = get<0>(center);
    int center_y = get<1>(center);
    for (int dx = -1; dx <= 1; dx++) {
        for (int dy = -1; dy <= 1; dy++) {
            if ((dx == 0) && (dy == 0))
                break;
            int x = (center_x + dx) % this->biotope_size_x;
            int y = (center_y + dy) % this->biotope_size_y;
            tuple<int, int> candidate_location = make_tuple(x, y);
            bool candidate_location_in_biotope_free_locs = (this->biotope_free_locs.find(candidate_location) != this->biotope_free_locs.end());
            if (candidate_location_in_biotope_free_locs) {
                surrounding_free_locations.push_back(candidate_location);
            }
        }
    }
    random_shuffle(surrounding_free_locations.begin(), surrounding_free_locations.end());
}
    
void Ecosystem::getSurroundingOrganisms(tuple<int, int> center, vector<Organism*> &surrounding_organisms) {
    int center_x = get<0>(center);
    int center_y = get<1>(center);
    for (int dx = -1; dx <= 1; dx++) {
        for (int dy = -1; dy <= 1; dy++) {
            if ((dx == 0) && (dy == 0))
                break;
            int x = (center_x + dx) % this->biotope_size_x;
            int y = (center_y + dy) % this->biotope_size_y;
            tuple<int, int> candidate_location = make_tuple(x, y);
            bool organism_is_in_biotope = (this->biotope.find(candidate_location) != this->biotope.end());
            if (organism_is_in_biotope) {
                surrounding_organisms.push_back(this->biotope[candidate_location]);
            }
        }
    }
    random_shuffle(surrounding_organisms.begin(), surrounding_organisms.end());
}
    
void Ecosystem::deleteDeadOrganisms() {
    for (auto dead_organism:this->dead_organisms) {
        delete dead_organism;
    }
    this->dead_organisms.clear();
}
    
void Ecosystem::evolve() {
    // Create a vector of current organisms
    vector<Organism*> organisms_to_act(this->biotope.size(), nullptr);
    int i = 0;
    for (auto x:this->biotope) {
        organisms_to_act[i] = x.second;
        i += 1;
    }
    // For each organism, act
    for (auto organism:organisms_to_act) {
        if (organism->is_alive) {;
            organism->act();
        }
    }
    this->deleteDeadOrganisms();
    this->time += 1;
}

/*
int main(int argc, char* argv[]) {
    srand( static_cast<unsigned int>(time(NULL)));
    Ecosystem ecosystem = Ecosystem();
    while (true) {
        ecosystem.evolve();
        auto num_organisms = ecosystem.biotope.size();
        auto num_free_locs = ecosystem.biotope_free_locs.size();
        cout << "Time: " << ecosystem.time << endl;
        cout << "    num organism: " << num_organisms << endl;
        cout << "    num free locs: " << num_free_locs << endl;
        cout << "    sum previous numbers: " << num_organisms + num_free_locs << endl;
    }
    return 0;
}*/