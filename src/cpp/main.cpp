#include <iostream>
#include "ecosystem.h"
#include "ExperimentInterface.h"
#include <string>
#include <signal.h>

using namespace std;

volatile sig_atomic_t save_and_exit = 0;

void sigint_handler(int s){
    save_and_exit = 1;
}

int main(int argc, char* argv[]) {
    struct sigaction sigIntHandler;
    sigIntHandler.sa_handler = sigint_handler;
    sigemptyset(&sigIntHandler.sa_mask);
    sigIntHandler.sa_flags = 0;
    sigaction(SIGINT, &sigIntHandler, NULL);

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
    ExperimentInterface* ei = new ExperimentInterface(dst_dir, new_experiment);
    while (true) {
        auto num_organisms = ei->getEcosystemPointer()->biotope.size();
        auto num_free_locs = ei->getEcosystemPointer()->biotope_free_locs.size();
        cout << "Time: " << ei->getRunningTime() << endl;
        cout << "    num organism: " << num_organisms << endl;
        cout << "    num free locs: " << num_free_locs << endl;
        cout << "    sum previous numbers: " << num_organisms + num_free_locs << endl;
        cout << endl;
        if (save_and_exit == 1) {
            ei->saveEcosystem();
            return 0;
        }
        ei->evolve();
    }
    return 0;
}
