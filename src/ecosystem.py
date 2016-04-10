"""
Agent-based system emulating an ecosystem of virtual bugs eating each other
"""
import random

PLANT = 'plant'
HERBIVORE = 'herbivore'
CARNIVORE = 'carnivore'


class Ecosystem(object):
    """ Conductor of the ecosystem

    Role:
        - create and initialize organisms
        - handle adding of new organisms to ecosystem
        - produce changes in ecosystem in each time step (evolve)
    """

    def __init__(self):
        self.organisms_settings = {
            'num_plants': 50,
            'num_herbivores': 50,
            'num_carnivores': 50
        }
        self.biotope_settings = {
            'size_x': 400,
            'size_y': 400
        }
        self.initialize_biotope()
        self.organisms = {}
        self.initialize_organisms()

    def initialize_biotope(self):
        """ Initialize biotope
        """
        size_x = self.biotope_settings['size_x']
        size_y = self.biotope_settings['size_y']
        self.biotope = [[None] * size_x] * size_y

    def initialize_organisms(self):
        """ Method to initialize a set of organisms

        It uses the settings stored in self.organisms_settings
        """
        num_plants = self.organisms_settings['num_plants']
        num_herbivores = self.organisms_settings['num_herbivores']
        num_carnivores = self.organisms_settings['num_carnivores']

        # Create plants
        for i in range(0, num_plants):
            random_location = self.seek_free_location()
            self.add_organism(Organism(type=PLANT, parent_ecosystem=self,
                                       location=random_location))

        # Create hervibores
        for i in range(0, num_herbivores):
            random_location = self.seek_free_location()
            self.add_organism(Organism(type=HERBIVORE, parent_ecosystem=self,
                                       location=random_location))

        # Create carnivores
        for i in range(0, num_carnivores):
            random_location = self.seek_free_location()
            self.add_organism(Organism(type=CARNIVORE, parent_ecosystem=self,
                                       location=random_location))

    def add_organism(self, organism):
        """ Add organism to ecosytem

        Args:
            organism (Organism): Organism objects to be added to ecosystem
        """
        self.organisms[organism.id] = organism
        (x, y) = organism.location
        self.biotope[x][y] = organism.id

    def delete_organism(self, organism):
        """ Remove organism from ecosysem

        Args:
            organism (Organism): Organism to be removed from ecosystem
        """
        (x, y) = organism.location
        del self.organisms[organism.id]
        self.biotope[x][y] = None

    def seek_free_location(self, reference_location=None):
        """ Seek a free location in biotope

        If reference_location parameter is specified, the closest free location
        to it is returned. If several equally close free locations are found,
        it is selected randomly among them.


        Args:
            close_to (Optional[tuple]): Tuple with (x, y) coordinates of
                                        reference location.
        Returns:
            (tuple): (x, y) coordinates of a free location. Is not available,
                     returns None.
        """
        # First finds free locations
        # TODO: make this list global and update it when needed.
        free_locations = []
        for x in range(0, len(self.biotope)):
            for y in range(0, len(self.biotope[x])):
                if reference_location is None:
                    d = None
                else:
                    (ref_x, ref_y) = reference_location
                    d = abs(ref_x - x) + abs(ref_y - y)  # Manhattan distance
                if self.biotope[x][y] is None:
                    free_locations.append((x, y, d))

        # Then shuffle or order list of free locations
        free_locations = random.shuffle(free_locations)
        if reference_location is None:
            free_location = free_locations[0]
            return (free_location[0], free_location[1])  # (x, y)
        else:
            free_locations.sort(key=lambda x: x[2])
            free_location = free_locations[0]
            return (free_location[0], free_location[1])  # (x, y)


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
        DEFAULT_AGE = 500
        self.type = type
        self.parent_ecosystem = parent_ecosystem
        self.location = location
        self.age = DEFAULT_AGE
        self.alive = True

    def move(self):
        """ Move organism to a free location in ecosystem
        """
        self.location = self.parent_ecosystem.seek_free_location(
            self.location)

    def is_eatable(self, target_organism):
        """ Returns true if target_organism can be eaten by self

        Args:
            target_organism (Organism): Organism to be eaten
        """
        eatable = False
        if self.type == CARNIVORE and target_organism.type == HERBIVORE:
            eatable = True
        if self.type == HERBIVORE and target_organism.type == PLANT:
            eatable = True
        return eatable

    def eat(self):
        """ Check if some food is around, and eat it.

        In this first version, the predator 'sucks' the pray's age and kill it.
        """
        for dx in [-1, 0, 1]:
            for dy in [-1, 0, 1]:
                if (dx == 0) and (dy == 0):
                    break  # can not eat itself
                target_x = self.location[0] + dx
                target_y = self.location[1] + dy
                target_organism_id = \
                    self.parent_ecosystem.biotope[target_x][target_y]

                if target_organism_id is not None:
                    target_organism = \
                        self.parent_ecosystem.organisms[target_organism_id]

                if self.is_eatable(target_organism):
                    # Bingo! It is food!
                    self.age += target_organism.age
                    target_organism.kill()

    def kill(self):
        """ Kill organism (e.g. when it is eaten)
        """
        self.alive = False


class GUI(object):
    pass


def main():
    """ Main function of ecosystem
    """
    import time
    step_time = 1.0
    ecosystem = Ecosystem()
    gui = GUI(ecosystem)

    while True:
        ecosystem.evolve()
        gui.draw_ecosystem()
        if step_time > 0.0:
            time.sleep(step_time)

if __name__ == '__main__':
    """ Code to be run if script is run from commandline
    """
    main()
