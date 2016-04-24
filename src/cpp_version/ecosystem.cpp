/** @file ecosystem.cpp
 * @brief Everything is in this file
 *
 */
#include "ecosystem.h"

using namespace std;

default_random_engine eng((random_device())());

Organism::Organism(tuple<int, int> location, Ecosystem* parent_ecosystem, species_t species, float energy_reserve) {
    this->ENERGY_COST["to have the capacity of moving"] = 2.0f;
    this->ENERGY_COST["to move"] = 5.0f;
    this->ENERGY_COST["to have the capacity of hunting"] = 4.0f;
    this->ENERGY_COST["to hunt"] = 10.0f;
    this->ENERGY_COST["to have the capacity of procreating"] = 0.0f;
    this->ENERGY_COST["to procreate"] = 15.0f;
    this->MINIMUM_ENERGY_REQUIRED_TO["move"] = 30.0f;
    this->MINIMUM_ENERGY_REQUIRED_TO["hunt"] = 30.0f;
    this->MINIMUM_ENERGY_REQUIRED_TO["procreate"] = 100.0f;
    this->MAX_LIFESPAN[PLANT] = 40;
    this->MAX_LIFESPAN[HERBIVORE] = 35;
    this->MAX_LIFESPAN[CARNIVORE] = 100;
    this->PROCREATION_PROBABILITY[PLANT] = 0.5f;
    this->PROCREATION_PROBABILITY[HERBIVORE] = 0.1f;
    this->PROCREATION_PROBABILITY[CARNIVORE] = 0.02f;
    this->PHOTOSYNTHESIS_CAPACITY = 5.0f;

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
    this->BIOTOPE_SETTINGS["size_x"] = 200;
    this->BIOTOPE_SETTINGS["size_y"] = 200;
    this->INITIAL_NUM_OF_ORGANISMS[PLANT] = 3;
    this->INITIAL_NUM_OF_ORGANISMS[HERBIVORE] = 3;
    this->INITIAL_NUM_OF_ORGANISMS[CARNIVORE] = 3;
    this->INITIAL_ENERGY_RESERVE = 100.0f;

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