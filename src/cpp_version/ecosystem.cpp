/** @file ecosystem.cpp
 * @brief Everything is in this file
 *
 */
#include "ecosystem.h"

using namespace std;

default_random_engine eng((random_device())());


/*********************************************************
 * Ecosystem implementation
 */

/** @brief Ecosystem constructor
* 
* It reads experiments settings from constants, initialize biotope and create organisms.
*/
Ecosystem::Ecosystem() {
    this->_initial_num_plants = INITIAL_NUM_OF_ORGANISMS.at(PLANT);
    this->_initial_num_herbivores = INITIAL_NUM_OF_ORGANISMS.at(HERBIVORE);
    this->_initial_num_carnivores = INITIAL_NUM_OF_ORGANISMS.at(CARNIVORE);
    this->biotope_size_x = BIOTOPE_SETTINGS.at("size_x");
    this->biotope_size_y = BIOTOPE_SETTINGS.at("size_y");
    this->_initializeBiotope();
    this->_initializeOrganisms();
    this->time = 0;
    this->rendered = false;
}


/** @brief Add organism to ecosystem
*
* Procedure:
* 1. add organism to current biotope
* 2. delete its position from biotope_free_locs
*
* @param[in] organism Pointer to organism to be added to ecosystem
*/
void Ecosystem::addOrganism(Organism* organism) {
    this->biotope[organism->location] = organism;
    this->biotope_free_locs.erase(organism->location);
}

/** @brief Remove organism from ecosystem
*
* Procedure:
* 1. delete organism from current biotope
* 2. add its position to biotope_free_locs
* 3. queue organism to be deleted at the end of iteration
*
* @param[in] organism Pointer to organism to be removed from ecosystem
*/
void Ecosystem::removeOrganism(Organism* organism) {
    this->biotope.erase(organism->location);
    this->biotope_free_locs.insert(organism->location);
    this->_dead_organisms.push_back(organism);
}

/** @brief Update organism location
*
* Organism must call this function when it moves to let Ecosystem know
*
* Procedure:
* 1. delete organism's old_location (inner variable of organism) from biotope
* 2. add its old position to biotope_free_locs
* 3. add organism to biotope according with its new location
* 2. delete organism's new location from biotope_free_locs
* 3. update organisms->old_location with its new location
*
* @param[in] organism Pointer to organism that is moving
*/
void Ecosystem::updateOrganismLocation(Organism* organism){
    this->biotope.erase(organism->old_location);
    this->biotope_free_locs.insert(organism->old_location);
    this->biotope[organism->location] = organism;
    this->biotope_free_locs.erase(organism->location);
    organism->old_location = organism->location;
}

/** @brief Get a vector of free positions (random order) around a given center (x, y)
*
* Only positions adjacent to center are reported.
*
* @param[in] center <x. y> tuple around which free locations are searched
* @param[out] surrounding_free_locations Vector where surrounding free locations are appended
*/
void Ecosystem::getSurroundingFreeLocations(tuple<int, int> center, vector<tuple<int, int>> &surrounding_free_locations) {
    int center_x = get<0>(center);
    int center_y = get<1>(center);
    for (int dx = -1; dx <= 1; dx++) {
        for (int dy = -1; dy <= 1; dy++) {
            if ((dx == 0) && (dy == 0))
                break;
            int x = (center_x + dx + this->biotope_size_x) % this->biotope_size_x;
            int y = (center_y + dy + this->biotope_size_y) % this->biotope_size_y;
            tuple<int, int> candidate_location = make_tuple(x, y);
            bool candidate_location_in_biotope_free_locs = (this->biotope_free_locs.find(candidate_location) != this->biotope_free_locs.end());
            if (candidate_location_in_biotope_free_locs) {
                surrounding_free_locations.push_back(candidate_location);
            }
        }
    }
    random_shuffle(surrounding_free_locations.begin(), surrounding_free_locations.end());
}

/** @brief Get a vector of organisms (random order) around a given location (x, y)
*
* Only organisms adjacent to center (but not IN center) are reported.
*
* @param[in] center <x. y> tuple around which free locations are searched
* @param[out] surrounding_organisms Vector of organisms around center
*/
void Ecosystem::getSurroundingOrganisms(tuple<int, int> center, vector<Organism*> &surrounding_organisms) {
    int center_x = get<0>(center);
    int center_y = get<1>(center);
    for (int dx = -1; dx <= 1; dx++) {
        for (int dy = -1; dy <= 1; dy++) {
            if ((dx == 0) && (dy == 0))
                break;
            int x = (center_x + dx + this->biotope_size_x) % this->biotope_size_x;
            int y = (center_y + dy + this->biotope_size_y) % this->biotope_size_y;
            tuple<int, int> candidate_location = make_tuple(x, y);
            bool organism_is_in_biotope = (this->biotope.find(candidate_location) != this->biotope.end());
            if (organism_is_in_biotope) {
                surrounding_organisms.push_back(this->biotope[candidate_location]);
            }
        }
    }
    random_shuffle(surrounding_organisms.begin(), surrounding_organisms.end());
}

/** @brief Evolve one time unit in ecosystem
*
* 1. For each organism in biotope, run organism->act()
* 2. Delete all dead organisms to free their memory
* 3. Increase ecosystem time in 1 unit
*/
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
    this->_deleteDeadOrganisms();
    this->time += 1;
}

/** @brief Initialize biotope
* 
* Initialize biotope_free_locs with all positions in biotope.
*/
void Ecosystem::_initializeBiotope() {
    for (int x = 0; x < this->biotope_size_x; x++) {
        for (int y = 0; y < this->biotope_size_y; y++) {
            this->biotope_free_locs.insert(make_tuple(x, y));
        }
    }
}

/** @brief Create organisms and add them to ecosystem
*
* It is separately done for (1) plants, (2) herbivores and (3) carnivores.
*/
void Ecosystem::_initializeOrganisms() {
    // Create and add plants
    for (int i = 0; i < this->_initial_num_plants; i++) {
        tuple<int, int> rand_location = this->_getRandomFreeLocation();
        this->addOrganism(new Organism(rand_location, this, PLANT, INITIAL_ENERGY_RESERVE));
    }

    // Create and add herbivores
    for (int i = 0; i < this->_initial_num_herbivores; i++) {
        tuple<int, int> rand_location = this->_getRandomFreeLocation();
        this->addOrganism(new Organism(rand_location, this, HERBIVORE, INITIAL_ENERGY_RESERVE));
    }

    // Create and add carnivores
    for (int i = 0; i < this->_initial_num_carnivores; i++) {
        tuple<int, int> rand_location = this->_getRandomFreeLocation();
        this->addOrganism(new Organism(rand_location, this, CARNIVORE, INITIAL_ENERGY_RESERVE));
    }
}

/** @brief Get random free location in biotope
* 
* It just takes a random value from biotope_free_locs set.
*/
tuple<int, int> Ecosystem::_getRandomFreeLocation() {
    auto it = this->biotope_free_locs.begin();
    int r = rand() % this->biotope_free_locs.size();
    advance(it, r);
    return *it;
}

/** @brief Delete all objects queued in dead_organisms vector
*
* It is run at the end of each iteration
*/
void Ecosystem::_deleteDeadOrganisms() {
    for (auto dead_organism:this->_dead_organisms) {
        delete dead_organism;
    }
    this->_dead_organisms.clear();
}


/*********************************************************
* Organism implementation
*/


/** @brief Organism constructor
*/
Organism::Organism(tuple<int, int> location, Ecosystem* parent_ecosystem, species_t species, float energy_reserve) {

    // Relative to parent_ecosystem:
    this->_parent_ecosystem = parent_ecosystem;
    this->location = location;
    this->old_location = location;

    // Genes:
    this->species = species;
    this->_death_age = rand() % MAX_LIFESPAN.at(this->species);

    // State:
    this->energy_reserve = energy_reserve;
    this->is_alive = true;
    this->_age = 0;
    this->_cause_of_death = "";
    this->_is_energy_dependent = true;
}


/** @brief Act
*
* Procedure:
* 1. if it is a plant: do photosynthesis
* 2. move
* 3. if still alive: hunt
* 4. if still alive: procreate
* 5. if still alive: age
*/
void Organism::act() {
    // If PLANT: _do_photosynthesis
    if (this->species == PLANT) {
        this->_do_photosynthesis();
    }

    // Move
    this->_do_move();
    // Hunt
    if (!this->is_alive)
        return;
    this->_do_hunt();

    // Procreate
    if (!this->is_alive)
        return;
    this->_do_procreate();

    // Procreate
    if (!this->is_alive)
        return;
    this->_do_age();
}

/** @brief Do phosynthesis (only called in this organism is a plant)
*
* It just increases energy_reserve a constant value equals to PHOTOSYNTHESIS_CAPACITY
*/
void Organism::_do_photosynthesis() {
    if (this->_is_energy_dependent)
        this->energy_reserve = this->energy_reserve + PHOTOSYNTHESIS_CAPACITY;
}

/** @brief true if organism has enough energy to perform a given action
*
* It is defined by constant MINIMUM_ENERGY_REQUIRED_TO, which is different than ENERGY_COST
*
* @param[in] action Action to be performed (e.g. "move", "hunt", ...)
*/
bool Organism::_has_enough_energy_to(const string &action) {
    bool _has_enough_energy = this->energy_reserve > MINIMUM_ENERGY_REQUIRED_TO.at(action);
    return _has_enough_energy;
}


/** @brief Reduce energy_reserve a value equals to amount_of_energy
*
* If energy_reserve reaches 0, do die.
* @param[in] amount_of_energy Amount of energy to substract from energy_reserve
*/
void Organism::_do_spend_energy(float amount_of_energy) {
    this->energy_reserve = this->energy_reserve - amount_of_energy;
    if (this->energy_reserve <= 0) {
        this->_do_die("starvation");
    }
}

/** @brief Do organism move
*
* Procedure:
* 1. spend energy for having the capability of moving
* 2. if it is still alive: get surrounding free locations around organism's location
* if there are free locations:
* 3. spend energy for moving
* 4. if it is still alive: update organism's location
* 5. notify ecosystem through ecosystem->updateOrganismLocation(this)
*/
void Organism::_do_move() {
    if (this->species==PLANT)
        return;

    // If it is energy dependent
    if (_is_energy_dependent) {
        if (this->_has_enough_energy_to("move")) {
            this->_do_spend_energy(ENERGY_COST.at("to have the capability of moving"));
        }
        if (!this->is_alive)
            return;
    }
    
    vector<tuple<int, int>> surrounding_free_locations;
    this->_parent_ecosystem->getSurroundingFreeLocations(this->location, surrounding_free_locations);
    if (surrounding_free_locations.size() > 0) {
        if (this->_is_energy_dependent) {
            this->_do_spend_energy(ENERGY_COST.at("to move"));
            if (!this->is_alive)
                return;
        }
        tuple<int, int> new_location = surrounding_free_locations[0];
        this->location = new_location;
        this->_parent_ecosystem->updateOrganismLocation(this);
    }
}

/** @brief true if a given prey is eatable by this organism
*
* @param[in] prey Pointer to prey to be eaten
*/
bool Organism::_is_eatable(Organism* prey) {
    bool _is_eatable = false;
    if ((this->species == CARNIVORE) && (this->species == HERBIVORE))
        _is_eatable = true;
    if ((this->species == HERBIVORE) && (this->species == PLANT))
        _is_eatable = true;
    return _is_eatable;
}

/** Do hunt
*
* Procedure:
* 1. spend energy for having the capability of hunting
* 2. if it is still alive: get surrounding free organisms around organism's location
* 3. for each surrounding organism (now potential prey):
*     * check if prey is eatable
*     * sum prey's energy reserve to self one
*     * kill prey
*
* @todo Check if all surrounding organisms must be eaten
*/
void Organism::_do_hunt() {
    if (this->species == PLANT)
        return;  // plants don't hunt
    
    if (this->_is_energy_dependent) {
        if (this->_has_enough_energy_to("hunt")) {
            float energy_cost = ENERGY_COST.at("to have the capability of hunting");
            this->_do_spend_energy(energy_cost);
        }
        if (!this->is_alive)
            return;
    }
    
    vector<Organism*> surrounding_organisms;
    this->_parent_ecosystem->getSurroundingOrganisms(this->location, surrounding_organisms);
    for (auto surr_organism:surrounding_organisms) {
        if (this->_is_eatable(surr_organism)) {
            Organism* prey = surr_organism;
            this->energy_reserve = this->energy_reserve + prey->energy_reserve;
            prey->_do_die("hunted");
        }
    }
}

/** @brief Do procreate
*
* Procedure:
* 1. spend energy for having the capability of procreating
* 2. determine if it procreates according to PROCREATION_PROBABILITY
* if so: 
* 3. get surrounding free locations
* if there are free locations:
* 4. define baby's energy reserve (half of self energy_reserve)
* 5. substract baby's energy reserve from self energy reserve
* 6. create Organism* baby and add it to ecosystem
* 7. spend energy for procreating
*/
void Organism::_do_procreate() {
    if (this->_is_energy_dependent) {
        if (this->_has_enough_energy_to("procreate"))
            this->_do_spend_energy(ENERGY_COST.at("to have the capability of procreating"));
        if (!this->is_alive)
            return;  // may have died because of starvation
    }
    uniform_real_distribution<float> fdis(0, 1.0);
    float random_value = fdis(eng);
    if (random_value >= PROCREATION_PROBABILITY.at(this->species))  // do not procreate
        return;

    vector<tuple<int, int>> free_locs;
    this->_parent_ecosystem->getSurroundingFreeLocations(this->location, free_locs);
    if (free_locs.size() == 0)
        return;

    tuple<int, int> baby_location = free_locs[0];
    float baby_energy_reserve = this->energy_reserve / 2.0f;
    this->energy_reserve = this->energy_reserve - baby_energy_reserve;
    Organism* baby = new Organism(baby_location, this->_parent_ecosystem, this->species, baby_energy_reserve);
    this->_parent_ecosystem->addOrganism(baby);
    if (this->_is_energy_dependent)
        this->_do_spend_energy(ENERGY_COST.at("to procreate"));
}

/** @brief Increase age 1 unit
*/
void Organism::_do_age() {
    this->_age += 1;
    if (this->_age > this->_death_age)
        this->_do_die("age");
}

/** @brief Do die
*
* Set is_alive to false and remove organism from ecosystem
*
* @param[in] cause_of_death Just for debug, indicates why did it die.
*/
void Organism::_do_die(const string &cause_of_death) {
    this->is_alive = false;
    this->_cause_of_death = cause_of_death;
    this->_parent_ecosystem->removeOrganism(this);
}


/*********************************************************
 * Exporter implementation
 */

/** @brief Exporter constructor
*
* @param[in] ecosystem Pointer to ecosystem to be exported
* @param[in] dst_path Path of destination folder
*/
Exporter::Exporter(Ecosystem* ecosystem, const string& dst_path) {
    this->ecosystem = ecosystem;
    // normalize dst_path to be in format: "histories/exp_name/."
    this->dst_path = fs::path(dst_path + fs::path::preferred_separator).normalize();
    if (!fs::is_directory(dst_path))
        fs::create_directories(dst_path);
    // Iterate over directory names to get experiment name (last name)
    // e.g. from "histories/exp_name/." get: exp_name
    vector<string> parts;
    for(auto& part : this->dst_path)
        parts.push_back(part.string());
    this->experiment_name = parts[parts.size() - 2];
}


/** @brief Export initial settings of experiment
*
* For compatibility issues, it exports them as a python module (by now)
*/
void Exporter::exportInitialSettings() {
    ofstream f_settings;
    fs::path dst_folder = this->dst_path / fs::path("settings");
    if (!fs::is_directory(dst_folder))
        fs::create_directories(dst_folder);
    fs::path dst_file = dst_folder / fs::path(this->experiment_name + ".py");
    f_settings.open(dst_file.string(), ios::out);
    f_settings << "PLANT = " << PLANT << endl;
    f_settings << "HERBIVORE = " << HERBIVORE << endl;
    f_settings << "CARNIVORE = " << CARNIVORE << endl;
    f_settings << "biotope_settings = {" << endl;
    f_settings << "    'size_x': " << BIOTOPE_SETTINGS.at("size_x") << "," << endl;
    f_settings << "    'size_y': " << BIOTOPE_SETTINGS.at("size_y") << endl;
    f_settings << "}" << endl;
    f_settings.close();
}

/** @brief Export biotope of current time slice as a JSON
*
*/
void Exporter::exportTimeSlice() {
    // get thousands intermediate folder
    int curr_time = this->ecosystem->time;
    int thousands = (curr_time / 1000) * 1000;
    ostringstream thousands_folder;
    thousands_folder << thousands << "_to_" << thousands + 999;
    if (!fs::is_directory(this->dst_path / fs::path(thousands_folder.str())))
        fs:create_directory(this->dst_path / fs::path(thousands_folder.str()));

    // get file name
    ostringstream dst_file_name;
    dst_file_name << curr_time << ".json";
    fs::path dst_file = this->dst_path /
                        fs::path(thousands_folder.str()) /
                        fs::path(dst_file_name.str());

    // export data
    ofstream f_data;
    f_data.open(dst_file.string(), ios::out);
    f_data << "{";
    bool first_organism = true;
    for (auto x:this->ecosystem->biotope) {
        if (!first_organism)
            f_data << ", ";
        else
            first_organism = false;
        tuple<int, int> position = x.first;
        Organism* organism  = x.second;
        f_data << "\"(" << get<0>(position) << ", " << get<1>(position) << ")\": " << organism->species;
    }
    f_data << "}" << endl;
    f_data.close();
}