#include <algorithm>
#include <vector>
#include <tuple>
#include <map>
#include <iostream>
#include <set>
#include <time.h>

using namespace std;

enum species_t {PLANT, HERBIVORE, CARNIVORE};

class Ecosystem; // just to indicate it will be defined later

class Organism {
public:
    species_t species;
    Ecosystem* parent_ecosystem;
    tuple<int, int> location;
    Organism(tuple<int, int> location, Ecosystem* parent_ecosystem, species_t species) {
        this->location = location;
        this->parent_ecosystem = parent_ecosystem;
        this->species = species;
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
        this->biotope_size_x = 200;
        this->biotope_size_y = 200;
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
            this->addOrganism(new Organism(rand_location, this, PLANT));
        }

        // Create herbivores
        for (int i; i < this->num_herbivores; i++) {
            tuple<int, int> rand_location = this->getRandomFreeLocation();
            this->addOrganism(new Organism(rand_location, this, HERBIVORE));
        }

        // Create carnivores
        for (int i; i < this->num_carnivores; i++) {
            tuple<int, int> rand_location = this->getRandomFreeLocation();
            this->addOrganism(new Organism(rand_location, this, CARNIVORE));
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
    
    /** @brief Remove organism from ecosystem and queue it for further deletion
     *
     *
     * @param[in] organism Organism object to be removed from ecosystem
     */
    void removeOrganism(Organism* organism) {
        this->dead_organisms.push_back(organism);
        this->biotope.erase(organism->location);
        this->biotope_free_locs.insert(organism->location);
    }
    
    /** @brief Update organism location within ecosystem
     *
     *
     * @param[in] organism Organism that has moved
     */
    void updateOrganismLocation(Organism& organism){
        
    }
    
    /** @brief Get a set of free locations around a given center
     *
     *
     * @param[in] center Tuple with <x, y> position of center coordinates
     */
    void getSurroundingFreeLocations(tuple<int, int> center) {
        
    }
    
    /** @brief Get a set of organisms that are around a given center
     *
     *
     * @param[in] center Tuple with <x, y> position of center coordinates
     */
    void getSurroundingOrganisms(tuple<int, int> center) {
        
    }
    
    /** @brief Evolve one time step the whole ecosystem
     */
    void evolve() {
        
    }

private:
    vector<Organism*> dead_organisms;
};


int main(int argc, char* argv[]) {
    srand( static_cast<unsigned int>(time(NULL)));
    cout << "Test" << endl;
    Ecosystem e = Ecosystem();
    tuple<int, int> location = e.getRandomFreeLocation();
    cout << get<0>(location) << "," << get<1>(location) << endl;
    return 0;
}