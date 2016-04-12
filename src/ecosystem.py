"""
Multi-agent system emulating an ecosystem of virtual bugs eating each other
"""
import random

PLANT = 'plant'
HERBIVORE = 'herbivore'
CARNIVORE = 'carnivore'


class Ecosystem(object):
    """ Environment of the ecosystem where organisms can live and evolve

    Role:
        - create and initialize organisms
        - handle adding of new organisms to ecosystem
        - produce changes in ecosystem in each time step (evolve)
    """

    def __init__(self):
        self.organisms_settings = {
            'num_plants': 40,
            'num_herbivores': 20,
            'num_carnivores': 10
        }
        self.biotope_settings = {
            'size_x': 200,
            'size_y': 200
        }
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
        num_plants = self.organisms_settings['num_plants']
        num_herbivores = self.organisms_settings['num_herbivores']
        num_carnivores = self.organisms_settings['num_carnivores']
        id = 0
        print "Creating plants..."
        # Create plants
        for i in range(0, num_plants):
            id += 1
            random_location = random.sample(self.biotope_free_locs, 1)[0]
            self.add_organism(Organism(type=PLANT, parent_ecosystem=self,
                                       location=random_location))

        print "Creating hervibores..."
        # Create hervibores
        for i in range(0, num_herbivores):
            id += 1
            random_location = random.sample(self.biotope_free_locs, 1)[0]
            self.add_organism(Organism(type=HERBIVORE, parent_ecosystem=self,
                                       location=random_location))

        print "Creating carnivores..."
        # Create carnivores
        for i in range(0, num_carnivores):
            id += 1
            random_location = random.sample(self.biotope_free_locs, 1)[0]
            self.add_organism(Organism(type=CARNIVORE, parent_ecosystem=self,
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
                x = center_x + dx
                y = center_y + dy
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
        surrounding_organisms = set()
        for dx in [-1, 0, 1]:
            for dy in [-1, 0, 1]:
                if (dx == 0) and (dy == 0):
                    break  # avoid checking center itself
                x = center_x + dx
                y = center_y + dy
                if (x, y) in self.biotope.keys():
                    organism = self.biotope[(x, y)]
                    surrounding_organisms.add(organism)
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


class Organism(object):
    """ Organism: agent of ecosystem
    """

    def __init__(self, type, parent_ecosystem, location):
        """ Initialize organism

        Args:
            type (str): Type of organism
            parent_ecosystem (Ecosystem): Parent ecosystem
            location (tuple): (x, y) coordinates of organism
        """
        # Relative to ecosystem
        self.parent_ecosystem = parent_ecosystem
        self.location = location
        self.old_location = location  # useful for parent_ecosystem to track it
        # Genes and state
        self.type = type
        self.death_age = random.randint(0, 500)  # TODO: Parametrize
        if type == PLANT:
            self.procreation_prob = 0.98
        elif type == HERBIVORE:
            self.procreation_prob = 0.99
        elif type == CARNIVORE:
            self.procreation_prob = 0.995
        self.age = 0
        self.is_alive = True

    def do_age(self):
        """ Increase self.age and die if reach self.death_age
        """
        self.age += 1
        if self.age == self.death_age:
            self.do_die()

    def do_move(self):
        """ Move organism to a free location in ecosystem
        """
        if self.type == PLANT:  # Plants don't move
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
        if self.type == CARNIVORE and pray.type == HERBIVORE:
            eatable = True
        if self.type == HERBIVORE and pray.type == PLANT:
            eatable = True
        return eatable

    def do_hunt(self):
        """ Find food nearby and eat it
        """
        surr_organisms = self.parent_ecosystem.get_surrounding_organisms(
            self.location)
        for surr_organism in surr_organisms:
            if self.is_eatable(surr_organism):
                pray = surr_organism
                self.age = 0  # The benefit of eating is getting younger
                pray.do_die()
                break

    def do_procreate_if_possible(self):
        """ Procreate if possible
        """
        free_locs = self.parent_ecosystem.get_surrounding_free_locations(
            self.location)
        if len(free_locs) == 0:
            return  # no empty space for procreation!
        baby_location = random.sample(free_locs, 1)[0]

        # Procreate if desired
        if random.random() > self.procreation_prob:
            baby = Organism(self.type, self.parent_ecosystem, baby_location)
            self.parent_ecosystem.add_organism(baby)


class GUI(object):
    """ Class for drawing an ecosystem
    """

    def __init__(self, parent_ecosystem):
        """ Initializes GUI
        """
        import matplotlib.pyplot as plt
        self.parent_ecosystem = parent_ecosystem
        self.img = plt.imshow([[0.0]], interpolation='nearest')
        plt.ion()
        plt.show()

    def draw_ecosystem(self):
        """ Draw ecosystem using Matplotlib
        """
        import matplotlib.pyplot as plt
        BGCOLOR = (0, 0, 0)
        color_mapping = {
            PLANT: (0, 0.75, 0),
            HERBIVORE: (0.5, 0.5, 0.5),
            CARNIVORE: (0.75, 0, 0)
        }
        size_x = self.parent_ecosystem.biotope_settings['size_x']
        size_y = self.parent_ecosystem.biotope_settings['size_y']
        pixels_map = [[BGCOLOR for i in range(size_x)]
                      for j in range(size_y)]

        for location, organism in self.parent_ecosystem.biotope.iteritems():
            (x, y) = location
            pixels_map[x][y] = color_mapping[organism.type]

        self.img.set_data(pixels_map)
        plt.pause(0.01)  # needed to avoid overloading of GUI


def main():
    """ Main function of ecosystem
    """
    ecosystem = Ecosystem()
    gui = GUI(ecosystem)

    while True:
        gui.draw_ecosystem()
        ecosystem.evolve()

if __name__ == '__main__':
    main()
