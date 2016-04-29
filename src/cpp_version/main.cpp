#include <iostream>
#include "ecosystem.h"

using namespace std;
using json = nlohmann::json;


int main(int argc, char* argv[]) {
    /*
    Hack to read json:
    srand( static_cast<unsigned int>(time(NULL)));
    ifstream f_data_json;
    f_data_json.open("histories/try2/0_to_999/101.json");
    json data_json;
    f_data_json >> data_json;
    f_data_json.close();
    Ecosystem ecosystem = Ecosystem(data_json);
    */

    Ecosystem ecosystem = Ecosystem();
    Exporter exporter = Exporter(&ecosystem, argv[1]);
    exporter.exportInitialSettings();
    while (true) {
        exporter.exportTimeSlice();
        ecosystem.evolve();
        auto num_organisms = ecosystem.biotope.size();
        auto num_free_locs = ecosystem.biotope_free_locs.size();
        cout << "Time: " << ecosystem.time << endl;
        cout << "    num organism: " << num_organisms << endl;
        cout << "    num free locs: " << num_free_locs << endl;
        cout << "    sum previous numbers: " << num_organisms + num_free_locs << endl;
    }
    return 0;
}