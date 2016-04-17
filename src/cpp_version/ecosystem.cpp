/** @mainpage
 * Esta es la main page
 *
 * Donde podemos tener un readme sencillote
 */


/** @file ecosystem.cpp
 * @brief Todo esta aqui
 *
 * Contiene todo todito
 * Contiene todo todito
 *
 * @author Emilio Molina
 * @todo Try unordered_map and unordered_set: might be faster.
 * @bug Hay un bug!
 *
 */

#include <algorithm>
#include <vector>
#include <tuple>
#include <map>
#include <iostream>
#include <string>
#include <set>
#include <time.h>

using namespace std;

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
    {PLANT, 40},
    {HERBIVORE, 100},
    {CARNIVORE, 100}
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

class Ecosystem; // just to indicate it will be defined later

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

    Organism(tuple<int, int> location, Ecosystem* parent_ecosystem, species_t species, float energy_reserve) {
        // Relative to parent_ecosystem:
        this->parent_ecosystem = parent_ecosystem;
        this->location = location;
        this->old_location = old_location;
        // Genes:
        this->species = species;
        this->death_age = rand() % MAX_LIFESPAN[this->species];
        // State:
        this->age = 0;
        this->energy_reserve = energy_reserve;
        this->is_alive = true;
        this->cause_of_death = "";
    }
    
    void act() {
        // Check a buggy situation
        if (!this->is_alive) {
            cout << "Error! A dead organism is trying to act" << endl;
            return;
        }
        // If PLANT: do_photosynthesis
        if (this->species == PLANT) {
            this->do_photosynthesis();
        }
        // Move
        if (this->is_alive)
            this->do_move();
        
        // Hunt
        if (this->is_alive)
            this->do_hunt();
        
        // Procreate
        if (this->is_alive)
            this->do_procreate();
        
        // Procreate
        if (this->is_alive)
            this->do_age();
    }
    
    void do_photosynthesis() {
        
    }
    
    void do_move() {
        
    }
    
    void do_hunt() {
        
    }
    
    void do_procreate() {
        
    }
    
    void do_age() {
        
    }
    
    void do_die(const string &cause_of_death) {
        
    }
};

/** @brief Environment of the ecosystem where organisms can live and evolve
 *
 */
class Ecosystem {
    
public:
    int num_plants;
    int num_herbivores;
    int num_carnivores;
    int biotope_size_x;
    int biotope_size_y;
    map<tuple<int, int>, Organism*> biotope;
    set<tuple<int, int>> biotope_free_locs;

    /** @brief Constructor
     */
    Ecosystem() {
        this->num_plants = 40;
        this->num_herbivores = 40;
        this->num_carnivores = 40;
        this->biotope_size_x = BIOTOPE_SETTINGS["size_x"];
        this->biotope_size_y = BIOTOPE_SETTINGS["size_y"];
        this->initializeBiotope();
        this->initializeOrganisms();
    }
    
    /** @brief Initialize biotope
     */
    void initializeBiotope() {
        for (int x = 0; x < this->biotope_size_x; x++) {
            for (int y = 0; y < this->biotope_size_y; y++) {
                this->biotope_free_locs.insert(make_tuple(x, y));
            }
        }
    }
    
    
    /** @brief Returns a random free location
     *
     * @return Tuple <int, int> with random free location in ecosystem
     */
    tuple<int, int> getRandomFreeLocation() {
        auto it = this->biotope_free_locs.begin();
        int r = rand() % this->biotope_free_locs.size();
        advance(it, r);
        return *it;
    }
    
    /** @brief Initialize organisms
     */
    void initializeOrganisms() {
        // Create plants
        for (int i; i < this->num_plants; i++) {
            tuple<int, int> rand_location = this->getRandomFreeLocation();
            this->addOrganism(new Organism(rand_location, this, PLANT, INITIAL_ENERGY_RESERVE));
        }

        // Create herbivores
        for (int i; i < this->num_herbivores; i++) {
            tuple<int, int> rand_location = this->getRandomFreeLocation();
            this->addOrganism(new Organism(rand_location, this, HERBIVORE, INITIAL_ENERGY_RESERVE));
        }

        // Create carnivores
        for (int i; i < this->num_carnivores; i++) {
            tuple<int, int> rand_location = this->getRandomFreeLocation();
            this->addOrganism(new Organism(rand_location, this, CARNIVORE, INITIAL_ENERGY_RESERVE));
        }

    }
    
    /** @brief Add organism to ecosystem
     *
     * Note: organism is supposed to be already initialized.
     * First Header  | Second Header
     * ------------- | -------------
     * Content Cell  | Content Cell
     * Content Cell  | Content Cell
     *
     * ~~~~~~~~~~~~~~~{.c}
     * int func(int a,int b) { return a*b; }
     * ~~~~~~~~~~~~~~~
     * # Header 1
     * - list 1
     * - list 2
     *
     * @deprecated Esto es antiguo
     *
     * @todo Esto es un todo
     *
     * Esto es una ```prueba```
     *
     * A `cool' word in a `nice' sentence.
     *
     * @param[in] organism Organism object to be added to ecosystem
     */
    void addOrganism(Organism* organism) {
        this->biotope[organism->location] = organism;
        this->biotope_free_locs.erase(organism->location);
    }
    
    /** @brief Remove organism from ecosystem
     *
     *
     * @param[in] organism Organism object to be removed from ecosystem
     */
    void removeOrganism(Organism* organism) {
        this->biotope.erase(organism->location);
        this->biotope_free_locs.insert(organism->location);
    }
    

    /** @brief Append organism to a vector of organisms that will be deleted at the end of this time slice
     *
     *
     * @param[in] organism Organism object to be appended to vector of organisms to be removed from ecosystem
     */
    void appendToDeadOrganismsVector(Organism* organism) {
        this->dead_organisms.push_back(organism);
    }

    /** @brief Append organism to a vector of organisms that will be added to the ecosystem at the end of this time slice
     *
     *
     * @param[in] organism Organism object to be appended to vector of organisms to be added ecosystem
     */
    void appendToNewBornOrganismsVector(Organism* organism) {
        this->new_born_organisms.push_back(organism);
    }

    
    /** @brief Update organism location within ecosystem
     *
     *
     * @param[in] organism Organism that has moved
     */
    void updateOrganismLocation(Organism* organism){
        this->biotope.erase(organism->old_location);
        this->biotope_free_locs.insert(organism->old_location);
        this->biotope[organism->location] = organism;
        this->biotope_free_locs.erase(organism->location);
        organism->old_location = organism->location;
    }
    
    /** @brief Get a set of free locations around a given center
     *
     *
     * @param[in] center Tuple with <x, y> position of center coordinates
     * @param[out] surrounding_free_location Set of tuples where surrounding_free_locations are stored
     */
    void getSurroundingFreeLocations(tuple<int, int> center, set<tuple<int, int>> &surrounding_free_locations) {
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
                    surrounding_free_locations.insert(candidate_location);
                }
            }
        }
        
    }
    
    /** @brief Get a set of organisms that are around a given center
     *
     *
     * @param[in] center Tuple with <x, y> position of center coordinates
     * @param[out] surrounding_organisms Vector of pointer to Organism where surrounding organisms are stored
     */
    void getSurroundingOrganisms(tuple<int, int> center, vector<Organism*> &surrounding_organisms) {
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
    }
    
    /** @brief Delete from memory all dead organisms to free memory
     *
     */
    void deleteDeadOrganisms() {
        for (auto dead_organism:this->dead_organisms) {
            delete dead_organism;
        }
        this->dead_organisms.clear();
    }
    
    /** @brief Add new born organisms to ecosystem
     *
     */
    void addNewBornOrganisms() {
        for (auto new_born_organism:this->new_born_organisms) {
            this->addOrganism(new_born_organism);
        }
        this->new_born_organisms.clear();
    }
    
    
    /** @brief Evolve one time step the whole ecosystem
     */
    void evolve() {
        for (auto iterator = this->biotope.begin(); iterator != this->biotope.end(); ++iterator) {
            Organism* organism = iterator->second;
            organism->act();
        }
        this->deleteDeadOrganisms();
        this->addNewBornOrganisms();
    }

private:
    vector<Organism*> dead_organisms;
    vector<Organism*> new_born_organisms;
};


int main(int argc, char* argv[]) {
    srand( static_cast<unsigned int>(time(NULL)));
    cout << "Test" << endl;
    Ecosystem e = Ecosystem();
    tuple<int, int> location = e.getRandomFreeLocation();
    cout << get<0>(location) << "," << get<1>(location) << endl;
    return 0;
}