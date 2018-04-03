#include <iostream>
#include "ecosystem.h"
#include "ExperimentInterface.h"
#include <string>

using namespace std;

int main(int argc, char* argv[]) {
    cout << "Usage: ./ecosystem dst_directory [new]" << endl;
    cout << " --- " << endl;
    string dst_dir = argv[1];
    bool new_experiment = false;
    if (argc == 3) {
        string new_experiment_str = argv[2];
        new_experiment = (new_experiment_str == "new");
        if (not new_experiment) {
            cout << "unknown option!" << endl;
            exit(1);
        }
    }
    if (!experimentAlreadyExists(dst_dir))
        new_experiment = true;
    ExperimentInterface* ei = new ExperimentInterface(dst_dir, new_experiment);
    while (true) {
        auto num_organisms = ei->getEcosystemPointer()->biotope.size();
        auto num_free_locs = ei->getEcosystemPointer()->biotope_free_locs.size();
        cout << "Time: " << ei->getRunningTime() << endl;
        cout << "    num organism: " << num_organisms << endl;
        cout << "    num free locs: " << num_free_locs << endl;
        cout << "    sum previous numbers: " << num_organisms + num_free_locs << endl;
        cout << "\r\e[A\e[A\e[A\e[A";
        if (ei->getRunningTime() % 10 == 0) {
            ei->saveEcosystem();
        }
        ei->evolve();
    }
    return 0;
}
