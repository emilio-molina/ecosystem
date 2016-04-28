#include <iostream>
#include "ecosystem.h"

using namespace std;

int main(int argc, char* argv[]) {
    srand( static_cast<unsigned int>(time(NULL)));
    Ecosystem ecosystem = Ecosystem();
    Exporter exporter = Exporter(&ecosystem, argv[1]);
    while (true) {
        ecosystem.evolve();
        auto num_organisms = ecosystem.biotope.size();
        auto num_free_locs = ecosystem.biotope_free_locs.size();
        cout << "Time: " << ecosystem.time << endl;
        cout << "    num organism: " << num_organisms << endl;
        cout << "    num free locs: " << num_free_locs << endl;
        cout << "    sum previous numbers: " << num_organisms + num_free_locs << endl;
        exporter.exportTimeSlice();
    }
    return 0;
}