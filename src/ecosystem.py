"""
Multi-agent system emulating an ecosystem of virtual bugs eating each other
"""
import random
import sys
import os

"""
Species IDs:
"""
PLANT = 1
HERBIVORE = 2
CARNIVORE = 3
"""
Other IDs:
"""
_help = 'help'  # documentation

initial_num_of_organisms = {
    _help: """
    The number of organisms of each species that are created before
    the experiment starts
    """,
    PLANT: 1000,
    HERBIVORE: 300,
    CARNIVORE: 100
}

max_lifespan = {
    _help: """
    The maximum age an organism of each species can reach
    """,
    PLANT: 40,
    HERBIVORE: 25,
    CARNIVORE: 100
}

procreation_probability = {
    _help: """
    The probability that an organism procreate each time it acts
    """,
    PLANT: 0.50,
    HERBIVORE: 0.06,
    CARNIVORE: 0.02
}


class Ecosystem(object):
    """ Environment of the ecosystem where organisms can live and evolve

    Role:
        - create and initialize organisms
        - handle adding of new organisms to ecosystem
        - produce changes in ecosystem in each time step (evolve)
    """

    def __init__(self):
        self.time = 0
        self.biotope_settings = {
            'size_x': 200,
            'size_y': 200
        }
        self.size_x = self.biotope_settings['size_x']
        self.size_y = self.biotope_settings['size_y']
        self.initialize_biotope()
        self.initialize_organisms()

    def initialize_biotope(self):
        """ Initialize biotope
        """
        size_x = self.biotope_settings['size_x']
        size_y = self.biotope_settings['size_y']
        self.biotope = {}
        self.biotope_free_locs = set()
        for x in range(0, size_x):
            for y in range(0, size_y):
                self.biotope_free_locs.add((x, y))

    def initialize_organisms(self):
        """ Method to initialize a set of organisms

        It uses the settings stored in self.organisms_settings
        """
        num_plants = initial_num_of_organisms[PLANT]
        num_herbivores = initial_num_of_organisms[HERBIVORE]
        num_carnivores = initial_num_of_organisms[CARNIVORE]
        id = 0
        print "Creating plants..."
        # Create plants
        locations_list = random.sample(self.biotope_free_locs, num_plants)
        for random_location in locations_list:
            id += 1
            self.add_organism(Organism(species=PLANT,
                                       parent_ecosystem=self,
                                       location=random_location))

        print "Creating hervibores..."
        # Create hervibores
        locations_list = random.sample(self.biotope_free_locs, num_herbivores)
        for random_location in locations_list:
            id += 1
            self.add_organism(Organism(species=HERBIVORE,
                                       parent_ecosystem=self,
                                       location=random_location))

        print "Creating carnivores..."
        # Create carnivores
        locations_list = random.sample(self.biotope_free_locs, num_carnivores)
        for random_location in locations_list:
            id += 1
            self.add_organism(Organism(species=CARNIVORE,
                                       parent_ecosystem=self,
                                       location=random_location))

    def add_organism(self, organism):
        """ Add organism to ecosytem

        Args:
            organism (Organism): Organism objects to be added to ecosystem
            id (int): Id assigned to organism
        """
        (x, y) = organism.location
        self.biotope[(x, y)] = organism
        self.biotope_free_locs.remove((x, y))

    def remove_organism(self, organism):
        """ Remove organism from ecosysem

        Note: Organisms objects are not deleted manually. We rely on
              Python garbage collector.

        Args:
            organism (Organism): Organism to be removed from ecosystem
        """
        (x, y) = organism.location
        del self.biotope[(x, y)]  # delete reference from dict
        self.biotope_free_locs.add((x, y))

    def update_organism_location(self, organism):
        """ Update interval variables related to a given organism

        Args:
            organism (Organism): Organism to be removed from ecosystem
        """
        old_location = organism.old_location
        del self.biotope[old_location]
        self.biotope_free_locs.add(old_location)

        new_location = organism.location
        self.biotope[new_location] = organism
        self.biotope_free_locs.remove(new_location)

        organism.old_location = organism.location

    def get_surrounding_free_locations(self, center):
        """ Get a list of free locations around a given center

        Args:
            center (tuple): (x, y) coordinates of center
        Returns:
            (set): Set of free locations around center
        """
        (center_x, center_y) = center
        surrounding_free_locs = set()
        for dx in [-1, 0, 1]:
            for dy in [-1, 0, 1]:
                if (dx == 0) and (dy == 0):
                    break  # avoid checking center itself
                x = (center_x + dx) % self.size_x
                y = (center_y + dy) % self.size_y
                if (x, y) in self.biotope_free_locs:
                    surrounding_free_locs.add((x, y))
        return surrounding_free_locs

    def get_surrounding_organisms(self, center):
        """ Get a list of organisms around a given center

        Args:
            center (tuple): (x, y) coordinates of center
        Returns:
            (set): Set of organisms
        """
        (center_x, center_y) = center
        surrounding_organisms = []
        for dx in [-1, 0, 1]:
            for dy in [-1, 0, 1]:
                if (dx == 0) and (dy == 0):
                    break  # avoid checking center itself
                x = (center_x + dx) % self.size_x
                y = (center_y + dy) % self.size_y
                if (x, y) in self.biotope.keys():
                    organism = self.biotope[(x, y)]
                    surrounding_organisms.append(organism)
        return surrounding_organisms

    def evolve(self):
        """ Evolve one time step the whole ecosystem
        """
        curr_organisms = self.biotope.values()
        for organism in curr_organisms:
            if organism.is_alive:  # still alive
                organism.do_move()
                organism.do_hunt()
                organism.do_procreate_if_possible()
                organism.do_age()
        self.time += 1


class Organism(object):
    """ Organism: agent of ecosystem
    """

    def __init__(self, species, parent_ecosystem, location):
        """ Initialize organism

        Args:
            species (str): Species of the organism
            parent_ecosystem (Ecosystem): Parent ecosystem
            location (tuple): (x, y) coordinates of organism
        """
        # Relative to ecosystem
        self.parent_ecosystem = parent_ecosystem
        self.location = location
        self.old_location = location  # useful for parent_ecosystem to track it
        # Genes and state
        self.species = species
        self.death_age = random.randint(0, max_lifespan[species])
        self.age = 0
        self.is_alive = True

    def do_age(self):
        """ Increase self.age and die if reach self.death_age
        """
        self.age += 1
        if self.age > self.death_age:
            self.do_die()

    def do_move(self):
        """ Move organism to a free location in ecosystem
        """
        if self.species == PLANT:  # Plants don't move
            return

        free_locs = self.parent_ecosystem.get_surrounding_free_locations(
            self.location)
        if len(free_locs) > 0:
            new_location = random.sample(free_locs, 1)[0]
            self.location = new_location
        self.parent_ecosystem.update_organism_location(self)

    def do_die(self):
        """ Make organism dissapear from ecosystem
        """
        self.is_alive = False
        self.parent_ecosystem.remove_organism(self)

    def is_eatable(self, pray):
        """ True if pray can be eaten by self

        Args:
            pray (Organism): Organism to be eaten
        Returns:
            (bool): True if pray can be eaten
        """
        eatable = False
        if self.species == CARNIVORE and pray.species == HERBIVORE:
            eatable = True
        if self.species == HERBIVORE and pray.species == PLANT:
            eatable = True
        return eatable

    def do_hunt(self):
        """ Find food nearby and eat it
        """
        if self.species == PLANT:
            return   # plants don't eat. This save computing time
        surr_organisms = self.parent_ecosystem.get_surrounding_organisms(
            self.location)
        random.shuffle(surr_organisms)
        for surr_organism in surr_organisms:
            if self.is_eatable(surr_organism):
                pray = surr_organism
                self.age = 0  # The benefit of eating is getting younger
                pray.do_die()
                break

    def do_procreate_if_possible(self):
        """ Procreate if possible
        """
        if random.random() < procreation_probability[self.species]:
            free_locs = self.parent_ecosystem.get_surrounding_free_locations(
                self.location)
            if len(free_locs) == 0:
                return  # no empty space for procreation!
            baby_location = random.sample(free_locs, 1)[0]
            baby = Organism(self.species, self.parent_ecosystem, baby_location)
            self.parent_ecosystem.add_organism(baby)


class Exporter(object):
    """ Class to export ecosystem history to a folder.

    It exports a .json file for time slice containing all the organisms.
    """
    def __init__(self, parent_ecosystem, dst_folder):
        """ Initialize Exporter

        Args:
            parent_ecosystem (Ecosystem): Ecosystem to be exported
            dst_folder (str): Destination folder
        """
        self.dst_folder = dst_folder
        self.parent_ecosystem = parent_ecosystem

    def export_time_slice(self):
        """ Export data for current time slice of parent_ecosystem
        """
        import json
        curr_time = self.parent_ecosystem.time
        file_name = str(curr_time) + '.json'
        thousands = int(curr_time / 1000) * 1000
        thousands_folder = '{0}_to_{1}'.format(str(thousands),
                                               str(thousands + 999))
        dst_file_path = os.path.join(self.dst_folder, thousands_folder,
                                     file_name)
        if not os.path.isdir(os.path.dirname(dst_file_path)):
            os.makedirs(os.path.dirname(dst_file_path))
        dict_organisms = {}
        for location, organism in self.parent_ecosystem.biotope.iteritems():
            dict_organisms[str(location)] = organism.species
        with open(dst_file_path, 'w') as f:
            json.dump(dict_organisms, f)


def main():
    """ Main function of ecosystem
    """
    ecosystem = Ecosystem()
    exporter = Exporter(ecosystem, sys.argv[1])

    while True:
        exporter.export_time_slice()
        ecosystem.evolve()
        print "Time:", ecosystem.time

if __name__ == '__main__':
    if len(sys.argv) != 2:
        print "Usage: python ecosystem.py dst_folder"
        sys.exit(1)
    main()
