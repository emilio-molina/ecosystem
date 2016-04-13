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
* @bug Hay un bug!
*
*/

#include <algorithm>
#include <vector>
#include <tuple>
#include <map>
#include <set>
#include <time.h>

using namespace std;

class Organism;

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
  }

  /** @brief Initialize biotope
  */
  void initializeBiotope() {
    for (int x; x < size_x; x++) {
      for (int y; y < size_y; y++) {
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
    r = rand() % this->biotope_free_locs.size();
    advance(it, r);
    return *it;
  }

  /** @brief Initialize organisms
  */
  void initializeOrganisms() {
    // Create plants
    for (int i; i < num_plants; i++) {
      tuple<int, int> rand_location = this->getRandomFreeLocation();
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
  void addOrganism(Organism& organism) {

  }

  /** @brief Remove organism from ecosystem and queue it for further deletion
  *
  *
  * @param[in] organism Organism object to be removed from ecosystem
  */
  void removeOrganism(Organism& organism) {

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
};


/** @brief Organism: agent of ecosystem
*/
class Organism {
public:
  /** @brief Initializer
  */
  Organism();

  /** @brief Move organism to a free location in ecosystem
  */
  void do_move();

  /** @brief Find a pray nearby and eat it
  */
  void do_hunt();

  /** @brief Procreate if possible
  */
  void do_procreate_if_possible();

  /** @brief Increase this.age and die if reach this.death_age
  */
  void do_age() {

  }

  /** @brief Set this.alive to false and notify ecosystem to make it dissapear
  */
  void do_die() {

  }

private:
  /** @brief Returns true if pray can be eaten
  *
  * 
  */
  void pray_is_eatable(Organism& pray){

  }
};

class Exporter {

};

int main(int argc, char* argv[]) {

  return 0;
}
