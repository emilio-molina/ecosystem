#include <algorithm>
#include <vector>
#include <tuple>
#include <map>

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
  map<tuple<int, int>, *Organism> biotope;


  /** @brief Constructor
  */
  Ecosystem() {
    this.num_plants = 40;
    this.num_herbivores = 40;
    this.num_carnivores = 40;
    this.biotope_size_x = 200;
    this.biotope_size_y = 200;
  }

  /** @brief Initialize biotope
  */
  void initializeBiotope() {

  }

  /** @brief Initialize organisms
  */
  void initializeOrganisms() {

  }

  /** @brief Add organism to ecosystem
  *
  * Note: organism is supposed to be already initialized.
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
