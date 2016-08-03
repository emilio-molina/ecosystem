// @TODO: Fix because it is broken.

#include <iostream>
#include <unistd.h>
#include "ecosystem.h"

using namespace std;
using json = nlohmann::json;


int main(int argc, char* argv[]) {
    Ecosystem* ecosystem;
    if (argc == 3)
        ecosystem = new Ecosystem(argv[2]);
    else
        ecosystem = new Ecosystem();
    Exporter exporter = Exporter(ecosystem, argv[1]);
    exporter.exportInitialSettings();
    while (true) {
        exporter.exportTimeSlice();
        auto num_organisms = ecosystem->biotope.size();
        auto num_free_locs = ecosystem->biotope_free_locs.size();
        cout << "Time: " << ecosystem->time << endl;
        cout << "    num organism: " << num_organisms << endl;
        cout << "    num free locs: " << num_free_locs << endl;
        cout << "    sum previous numbers: " << num_organisms + num_free_locs << endl;
        usleep(500000);
        ecosystem->evolve();
    }
    return 0;
}
