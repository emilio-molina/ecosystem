/** @file ExperimentInterface.h
 * @brief Header of ExperimentInterface
 *
 */

#ifndef EXPERIMENTINTERFACE_H_INCLUDED
#define EXPERIMENTINTERFACE_H_INCLUDED

#include <mutex>
#include "ecosystem.h"
#include <boost/filesystem.hpp>

using namespace std;


// Auxiliar functions (documentation in ExperimentInterface.cpp)
void decompressData(stringstream &compressed, stringstream &decompressed);
void compressData(stringstream &decompressed, stringstream &compressed);
bool experimentAlreadyExists(string experiment_folder);
string getEcosystemJSONPath(fs::path dst_path, int time_slice);
string getThousandsFolder(int time_slice);
fs::path stringToPath(string path_str);
template <typename T>
std::string to_string_with_precision(const T a_value, const int n);


/** @brief Class to easily interact with experiment folder
 *
 * @ingroup
 */
class ExperimentInterface {
public:
    ExperimentInterface(string experiment_folder, bool overwrite);
    void evolve();
    Ecosystem* getEcosystemPointer();
    void lockEcosystem();
    bool tryLockEcosystem();
    void unlockEcosystem();
    void saveEcosystem();
    void loadEcosystem(int time_slice);
    json* getSettings_json_ptr();
    string getExperimentFolder();
    int getRunningTime();
    int getBackupPeriod();
    string getExperimentSize();
    vector<int> getTimesHavingCompleteBackups();
private:
    string _path;
    mutex _mtx;
    fs::path _dst_path;
    string _experiment_name;
    Ecosystem* _ecosystem;
    void _setExperimentFolder(string experiment_folder);
    void _cleanFolder();
};



#endif  // EXPERIMENTINTERFACE_H_INCLUDED
