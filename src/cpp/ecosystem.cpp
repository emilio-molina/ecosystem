/** @file ecosystem.cpp
 * @brief Implementation of ecosystem
 *
 * @ingroup core
 */
#include "ecosystem.h"

using namespace std;
using json = nlohmann::json;

default_random_engine eng((random_device())());

json default_settings;

vector<string> _SPECIES;
string PLANT;
string HERBIVORE1;
string HERBIVORE2;
string CARNIVORE1;
string CARNIVORE2;
string CARNIVORE3;
map<string, int> _BIOTOPE_SETTINGS;
map<string, float> _ENERGY_COST;
map<string, float> _MINIMUM_ENERGY_REQUIRED_TO;
map<string, float> _PHOTOSYNTHESIS_CAPACITY;
map<string, int> _INITIAL_NUM_OF_ORGANISMS;
map<string, vector<string>> _DEATH_AGE;
map<string, float> _PROCREATION_PROBABILITY;
map<string, vector<string>> _FOOD_WEB;
float _INITIAL_ENERGY_RESERVE;

/** @brief Set default settings for experiment
 *
 * This function specifies the default settings for the experiment
 *
 * @todo Decide definitive way of managing experiment settings
 * @ingroup core
 */
void set_default_settings()
{
    PLANT = "P";
    HERBIVORE1 = "H1";
    HERBIVORE2 = "H2";
    CARNIVORE1 = "C1";
    CARNIVORE2 = "C2";
    CARNIVORE3 = "C3";
    
    _SPECIES = {PLANT, HERBIVORE1, HERBIVORE2, CARNIVORE1, CARNIVORE2, CARNIVORE3};
    
    _ENERGY_COST = {
        {"to have the capability of moving", 0.0f},
        {"to move", 4.0f},
        {"to have the capability of hunting", 0.0f},
        {"to hunt", 0.0f},
        {"to have the capability of procreating", 0.002f},
        {"to procreate", 5.0f},
    };
    
    _MINIMUM_ENERGY_REQUIRED_TO = {
        {"move", 100.0f},
        {"hunt", 100.0f},
        {"procreate", 100.0f},
    };
    
    _PHOTOSYNTHESIS_CAPACITY = {
        {PLANT, 10.0f},
        {HERBIVORE1, 0.0f},
        {HERBIVORE2, 0.0f},
        {CARNIVORE1, 0.0f},
        {CARNIVORE2, 0.0f},
        {CARNIVORE3, 0.0f}
    };
    
    // Definition of gens grouped by species
    _DEATH_AGE = {
        {PLANT, {"uniform_int", "0", "30"}},
        {HERBIVORE1, {"uniform_int", "0", "50"}},
        {HERBIVORE2, {"uniform_int", "0", "50"}},
        {CARNIVORE1, {"uniform_int", "0", "100"}},
        {CARNIVORE2, {"uniform_int", "0", "100"}},
        {CARNIVORE3, {"uniform_int", "0", "100"}}
    };
    
    _PROCREATION_PROBABILITY = {
        {PLANT, 0.15f},
        {HERBIVORE1, 0.07f},
        {HERBIVORE2, 0.07f},
        {CARNIVORE1, 0.05f},
        {CARNIVORE2, 0.05f},
        {CARNIVORE3, 0.05f}
    };
    
    _INITIAL_NUM_OF_ORGANISMS = {
        {PLANT, 300},
        {HERBIVORE1, 30},
        {HERBIVORE2, 30},
        {CARNIVORE1, 30},
        {CARNIVORE2, 30},
        {CARNIVORE3, 30}
    };
    
    _BIOTOPE_SETTINGS = {
        {"size_x", 200},
        {"size_y", 200}
    };
    
    _FOOD_WEB = {
        {PLANT, {}},
        {HERBIVORE1, {PLANT}},
        {HERBIVORE2, {PLANT}},
        {CARNIVORE1, {HERBIVORE1, HERBIVORE2}},
        {CARNIVORE2, {HERBIVORE1, HERBIVORE2}},
        {CARNIVORE3, {HERBIVORE1, HERBIVORE2}}
    };

    _INITIAL_ENERGY_RESERVE = 30000.0f;

    
    default_settings["constants"]["SPECIES"] = _SPECIES;
    default_settings["constants"]["ENERGY_COST"] = _ENERGY_COST;
    default_settings["constants"]["MINIMUM_ENERGY_REQUIRED_TO"] = _MINIMUM_ENERGY_REQUIRED_TO;
    default_settings["constants"]["PHOTOSYNTHESIS_CAPACITY"] = _PHOTOSYNTHESIS_CAPACITY;
    default_settings["constants"]["INITIAL_NUM_OF_ORGANISMS"] = _INITIAL_NUM_OF_ORGANISMS;
    default_settings["constants"]["DEATH_AGE"] = _DEATH_AGE;
    default_settings["constants"]["PROCREATION_PROBABILITY"] = _PROCREATION_PROBABILITY;
    default_settings["constants"]["INITIAL_ENERGY_RESERVE"] = _INITIAL_ENERGY_RESERVE;
    default_settings["constants"]["BIOTOPE_SETTINGS"] = _BIOTOPE_SETTINGS;
    default_settings["constants"]["FOOD_WEB"] = _FOOD_WEB;
    default_settings["state"]["time"] = 0;
    default_settings["constants"]["BACKUP_PERIOD"] = 50;
    default_settings["constants"]["DRAWING_PERIOD"] = 1;
    default_settings["constants"]["DRAWING_ZOOM_FACTOR"] = 3;
    
    ostringstream str_random;
    str_random << eng;
    default_settings["state"]["RANDOM_ENG"] = str_random.str();

}

float evaluateRandomFunction(vector<string> definition) {
    string distributionName = definition[0];
    string valStr1 = definition[1];
    string valStr2 = definition[2];
    if (distributionName == "uniform_int") {
        int minVal = stoi(valStr1);
        int maxVal = stoi(valStr2);
        uniform_int_distribution<int> distribution(minVal, maxVal);
        return (float)distribution(eng);
    } else {
        cout << "unknown distribution!" << endl;
    }
    return 0.0f;
}
/*********************************************************
 * Ecosystem implementation
 */

/** @brief Ecosystem constructor
* 
* Initialize biotope and create organisms according to default settings.
*/
Ecosystem::Ecosystem() {
    
    set_default_settings();
    settings_json = default_settings;
    this->biotope_size_x = settings_json["constants"]["BIOTOPE_SETTINGS"]["size_x"];
    this->biotope_size_y = settings_json["constants"]["BIOTOPE_SETTINGS"]["size_y"];
    this->_initializeBiotope();
    this->_initializeOrganisms();
    this->time = settings_json["state"]["time"];
    istringstream srandom;
    string str_random = settings_json["state"]["RANDOM_ENG"];
    srandom.str(str_random);
    srandom >> eng;

}

/** @brief Ecosystem constructor using a JSON
*
* Initialize biotope and create organisms according to external JSON.
*
* @param[in] settings_json JSON data with ecosystem screenshot
*/
Ecosystem::Ecosystem(json data_json) {

    settings_json["constants"] = data_json["constants"];
    settings_json["state"] = data_json["state"];
    this->biotope_size_x = settings_json["constants"]["BIOTOPE_SETTINGS"]["size_x"];
    this->biotope_size_y = settings_json["constants"]["BIOTOPE_SETTINGS"]["size_y"];
    this->_initializeBiotope();
    this->_initializeOrganisms(data_json);
    this->time = settings_json["state"]["time"];
    istringstream srandom;
    string str_random = settings_json["state"]["RANDOM_ENG"];
    srandom.str(str_random);
    srandom >> eng;
}

/** @brief get the settings whithin a JSON variable
 *
 * @param[in] settings_json JSON data with ecosystem screenshot
 */
json* Ecosystem::getSettings_json_ptr() {
    json* sjp = new json(settings_json);
    return sjp;
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
* Organism must call this function when it moves to let Ecosystem know it
*
* Procedure:
* 1. delete organism's old_location from biotope
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
    shuffle(surrounding_free_locations.begin(), surrounding_free_locations.end(), eng);
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
    shuffle(surrounding_organisms.begin(), surrounding_organisms.end(), eng);
}

/** @brief Evolve one time unit in ecosystem
*
* 1. Delete dead organisms
* 2. For each organism in current biotope, run organism->act()
* 3. Increase ecosystem time in 1 unit
*/
void Ecosystem::evolve() {
    this->_deleteDeadOrganisms();

    // Create a vector of current organisms (needed because biotope is a map)
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
    // Create and add organisms
    for (string SPECIES : settings_json["constants"]["SPECIES"])
    {
        int      NUMBER_OF_ORGANISMS = int(settings_json["constants"]["INITIAL_NUM_OF_ORGANISMS"][SPECIES]);
        float INITIAL_ENERGY_RESERVE = int(settings_json["constants"]["INITIAL_ENERGY_RESERVE"]);
        for (int i = 0; i < NUMBER_OF_ORGANISMS; i++) {
            tuple<int, int> rand_location = this->_getRandomFreeLocation();
            this->addOrganism(new Organism(rand_location, this, SPECIES, INITIAL_ENERGY_RESERVE));
        }
    }
}

/** @brief Create organisms from a JSON and add them to ecosystem
*
* @param[in] data_json Variable with ecosystem info in JSON format
*/
void Ecosystem::_initializeOrganisms(json& data_json) {
    if (data_json.find("organisms") != data_json.end()) {
        int num_organisms = data_json["organisms"]["locations"].size();
        for (int i=0; i < num_organisms; i++) {
            tuple<int, int> location = make_tuple(data_json["organisms"]["locations"][i][0],
                                                  data_json["organisms"]["locations"][i][1]);
            string species = data_json["organisms"]["species"][i];
            float energy_reserve = data_json["organisms"]["energy_reserve"][i];
            Organism* o = new Organism(location, this, species, energy_reserve);
            // Set genes and state
            o->initial_energy_reserve = data_json["organisms"]["initial_energy_reserve"][i];
            o->age = data_json["organisms"]["age"][i];
            o->death_age = data_json["organisms"]["death_age"][i];
            o->is_energy_dependent = data_json["organisms"]["is_energy_dependent"][i];
            this->addOrganism(o);
        }
    }
    else _initializeOrganisms();
}

/** @brief Get random free location in biotope
* 
* It just takes a random value from biotope_free_locs set.
*/
tuple<int, int> Ecosystem::_getRandomFreeLocation() {
    uniform_int_distribution<int> distribution(0, this->biotope_free_locs.size() - 1);
    auto it = this->biotope_free_locs.begin();
    int r = distribution(eng);
    advance(it, r);
    return *it;
}

/** @brief Delete all objects queued in dead_organisms vector
*
* It is run at the beginning of each iteration
*/
void Ecosystem::_deleteDeadOrganisms() {
    for (auto dead_organism:this->_dead_organisms) {
        delete dead_organism;
    }
    this->_dead_organisms.clear();
}

/** @brief Serialize ecosystem to a JSON
*
* @param[out] data_json Variable where data will be stores as a json
*/
void Ecosystem::serialize(json& data_json) {
    // ecosystem data
    data_json["constants"] = settings_json["constants"];
    data_json["state"]["time"] = this->time;
    ostringstream str_random;
    str_random << eng;
    data_json["state"]["RANDOM_ENG"] = str_random.str();

    // living organisms data
    for (auto x:this->biotope) {
        tuple<int, int> position = x.first;
        Organism* organism = x.second;
        data_json["organisms"]["locations"].push_back({get<0>(position), get<1>(position)});
        data_json["organisms"]["species"].push_back(organism->species);
        data_json["organisms"]["age"].push_back(organism->age);
        data_json["organisms"]["death_age"].push_back(organism->death_age);
        data_json["organisms"]["initial_energy_reserve"].push_back(organism->initial_energy_reserve);
        data_json["organisms"]["energy_reserve"].push_back(organism->energy_reserve);
        data_json["organisms"]["is_energy_dependent"].push_back(organism->is_energy_dependent);
    }
    // dead organisms data
    for (Organism* organism:this->_dead_organisms) {
        tuple<int, int> position = organism->location;
        data_json["dead_organisms"]["locations"].push_back({get<0>(position), get<1>(position)});
        data_json["dead_organisms"]["species"].push_back(organism->species);
        data_json["dead_organisms"]["age"].push_back(organism->age);
        data_json["dead_organisms"]["death_age"].push_back(organism->death_age);
        data_json["dead_organisms"]["cause_of_death"].push_back(organism->cause_of_death);
        data_json["organisms"]["initial_energy_reserve"].push_back(organism->initial_energy_reserve);
        data_json["organisms"]["energy_reserve"].push_back(organism->energy_reserve);
        data_json["dead_organisms"]["is_energy_dependent"].push_back(organism->is_energy_dependent);
    }
}


/*********************************************************
* Organism implementation
*/


/** @brief Organism constructor
*
* @param[in] location Location of organism
* @param[in] parent_ecosystem Pointer to parent ecosystem
* @param[in] species Species identifier
* @param[in] energy_reserve Amount of initial energy
*/
Organism::Organism(tuple<int, int> location, Ecosystem* parent_ecosystem, string& species, float energy_reserve) {

    // Relative to parent_ecosystem:
    this->_parent_ecosystem = parent_ecosystem;
    this->location = location;
    this->old_location = location;

    // Genes:
    this->species = species;
    this->photosynthesis_capacity = float(parent_ecosystem->settings_json["constants"]["PHOTOSYNTHESIS_CAPACITY"][species]);
    auto death_age_definition = parent_ecosystem->settings_json["constants"]["DEATH_AGE"][species].get<vector<string>>();
    this->death_age = (int)evaluateRandomFunction(death_age_definition);

    // State:
    this->energy_reserve = energy_reserve;
    this->initial_energy_reserve = energy_reserve;
    this->is_alive = true;
    this->age = 0;
    this->cause_of_death = "";
    this->is_energy_dependent = true;
}


/** @brief Act
*
*/
void Organism::act() {

    this->_do_photosynthesis();

    this->_do_move();
    if (!this->is_alive)  // can die while moving
        return;

    this->_do_hunt();
    if (!this->is_alive)  // can die while hunting
        return;

    this->_do_procreate();
    if (!this->is_alive)  // can die while procreating
        return;

    this->_do_age();
}

/** @brief Do phosynthesis
*
* It just increases energy_reserve a constant value equals to photosynthesis_capacity
*/
void Organism::_do_photosynthesis() {
    if (this->is_energy_dependent)
        this->energy_reserve = this->energy_reserve + this->photosynthesis_capacity;
}

/** @brief true if organism has enough energy to perform a given action
*
* It is defined by constant MINIMUM_ENERGY_REQUIRED_TO, which is different than ENERGY_COST
*
* @param[in] action Action to be performed (e.g. "move", "hunt", ...)
*/
bool Organism::_has_enough_energy_to(const string &action) {
    float MINIMUM_ENERGY_REQUIRED = float(_parent_ecosystem->settings_json["constants"]["MINIMUM_ENERGY_REQUIRED_TO"][action]);
    return energy_reserve > MINIMUM_ENERGY_REQUIRED;
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
    if ((this->species==PLANT) || (this->species==CARNIVORE3))
        return;

    // If it is energy dependent
    if (is_energy_dependent) {
        if (this->_has_enough_energy_to("move")) {
            this->_do_spend_energy(
                _parent_ecosystem->settings_json["constants"]["ENERGY_COST"]["to have the capability of moving"]);
        }
        if (!this->is_alive)
            return;
    }
    
    vector<tuple<int, int>> surrounding_free_locations;
    this->_parent_ecosystem->getSurroundingFreeLocations(this->location, surrounding_free_locations);
    if (surrounding_free_locations.size() > 0) {
        if (this->is_energy_dependent) {
            this->_do_spend_energy(_parent_ecosystem->settings_json["constants"]["ENERGY_COST"]["to move"]);
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
    vector<string> food_web = default_settings["constants"]["FOOD_WEB"][this->species];
    // Check if prey->species in list of species this organism can eat
    return (find(food_web.begin(), food_web.end(), prey->species) != food_web.end());
}

/** @brief Do hunt
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
    
    if (this->is_energy_dependent) {
        if (this->_has_enough_energy_to("hunt")) {
            this->_do_spend_energy(
                _parent_ecosystem->settings_json["constants"]["ENERGY_COST"]["to have the capability of hunting"]);
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
    if (this->is_energy_dependent) {
        if (this->_has_enough_energy_to("procreate"))
            this->_do_spend_energy(
                _parent_ecosystem->settings_json["constants"]["ENERGY_COST"]["to have the capability of procreating"]);
        if (!this->is_alive)
            return;  // may have died because of starvation
    }
    uniform_real_distribution<float> fdis(0, 1.0);
    float random_value = fdis(eng);
    float PROCREATION_PROBABILITY = float(_parent_ecosystem->settings_json["constants"]["PROCREATION_PROBABILITY"][species]);
    if (random_value >= PROCREATION_PROBABILITY)  // do not procreate
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
    if (this->is_energy_dependent)
        this->_do_spend_energy(_parent_ecosystem->settings_json["constants"]["ENERGY_COST"]["to procreate"]);
}

/** @brief Increase age 1 unit
*/
void Organism::_do_age() {
    this->age += 1;
    if (this->age > this->death_age)
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
    this->cause_of_death = cause_of_death;
    this->_parent_ecosystem->removeOrganism(this);
}
