import Tkinter as Tk
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg
from matplotlib.backends.backend_tkagg import NavigationToolbar2TkAgg
from matplotlib.figure import Figure
import json
import os
import sys

"""
Species IDs:  # TO DO: Read this from a file
"""
PLANT = 1
HERBIVORE = 2
CARNIVORE = 3

size_x = 300  # TO DO: Read this from a file
size_y = 300

"""
Colors RGB:
"""
BGCOLOR = (0, 0, 0)
color_mapping = {
    PLANT: (0, 0.75, 0),
    HERBIVORE: (0.5, 0.5, 0.5),
    CARNIVORE: (0.75, 0, 0)
}


class GUI(object):
    """ Class for reading an ecosystem from a folder drawing it
    """

    def __init__(self, history_folder):
        """ Initializes GUI

        Args:
            history_folder (str): Path of folder with experiment history
        """
        self.history_folder = history_folder

        # Initialize Tk
        self.root = Tk.Tk()
        self.root.title("Ecosystems")

        # Initialize plot
        self.f = Figure(figsize=(5, 4), dpi=100)
        self.a = self.f.add_subplot(111)
        self.img = self.a.imshow([[0]], animated=True,
                                 interpolation='nearest')

        # a tk.DrawingArea
        self.canvas = FigureCanvasTkAgg(self.f, master=self.root)
        self.canvas.show()
        self.canvas.get_tk_widget().pack(side=Tk.TOP, fill=Tk.BOTH, expand=1)

        self.toolbar = NavigationToolbar2TkAgg(self.canvas, self.root)
        self.toolbar.update()
        self.canvas._tkcanvas.pack(side=Tk.TOP, fill=Tk.BOTH, expand=1)

        self.canvas.mpl_connect('key_press_event',
                                self.callback_key_event)

        # Initialize time
        self.time = 0
        self.draw_ecosystem()
        self.playing = False

    # Handle key events
    def callback_key_event(self, event):
        """ Callback for key event
        """
        if event.key == 'left':
            self.time -= 1
        if event.key == 'down':
            self.time -= 10
        if event.key == 'right':
            self.time += 1
        if event.key == 'up':
            self.time += 10
        if event.key == ' ':
            self.playing = not self.playing
            self.timer_update_data(time_interval=1)
        self.draw_ecosystem()

    def process_json(self, json_dict):
        """ Read a dict with keys as strings and convert it to tuples

        e.g. in = {'(2, 3)': 1} -> out = {(2, 3): 1}

        Args:
            json_dict (dict): Input dictionary with key as strings
        """
        new_dict = {}
        for key, value in json_dict.iteritems():
            new_key = tuple(map(int, key[1:-1].split(',')))
            # convert string '(3, 2)' to tuple (3, 2)
            new_dict[new_key] = value
        return new_dict

    def draw_ecosystem(self):
        """ Read time slice from disk and draw ecosystem
        """
        # Read data from ecosystem history folder
        file_name = str(self.time) + '.json'
        thousands = int(self.time / 1000) * 1000
        thousands_folder = '{0}_to_{1}'.format(str(thousands),
                                               str(thousands + 999))
        dst_file_path = os.path.join(self.history_folder,
                                     thousands_folder,
                                     file_name)

        # Process data
        with open(dst_file_path, 'r') as f:
            json_dict = json.load(f)
        biotope = self.process_json(json_dict)

        # Create pixel map
        pixels_map = [[BGCOLOR for i in range(size_x)]
                      for j in range(size_y)]

        for location, species in biotope.iteritems():
            (x, y) = location
            pixels_map[x][y] = color_mapping[species]

        # Draw ecosystem by replacing the data of current image
        self.img.set_data(pixels_map)
        self.a.set_title('TIME = %d' % self.time)
        self.f.canvas.draw()

    def timer_update_data(self, time_interval):
        """ Function used as timer for automatically playing the ecosystem

        Args:
            time_interval (int): Time interval in ms
        """
        if self.playing:
            self.time += 1
            self.draw_ecosystem()
            self.root.after(time_interval,
                            self.timer_update_data,
                            time_interval)


if __name__ == '__main__':
    print "\nFirst click on GUI to give it focus."
    print "\nThen use keys to control the view:"
    print "    right: time + 1"
    print "    up: time + 10"
    print "    left: time - 1"
    print "    down: time - 10"
    print "    space: toogle playing"
    gui = GUI(sys.argv[1])
    gui.root.mainloop()
