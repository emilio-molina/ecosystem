/*
  ==============================================================================

    ExperimentInterface.cpp
    Created: 16 May 2016 9:45:31pm
    Author:  Emilio Molina

  ==============================================================================
*/

#include "ExperimentInterface.h"

namespace bf=boost::filesystem;


fs::path stringToPath(string experiment_folder) {
    fs::path dst_path = fs::path(experiment_folder + fs::path::preferred_separator).normalize();
    if (dst_path.filename() == ".")
    dst_path.remove_leaf();
    return dst_path;
}

string getThousandsFolder(int time_slice) {
    int thousands = (time_slice / 1000) * 1000;
    ostringstream thousands_folder;
    thousands_folder << thousands << "_to_" << thousands + 999;
    return thousands_folder.str();
}

string getEcosystemJSONPath(fs::path dst_path, int time_slice) {
    string thousands_folder_name = getThousandsFolder(time_slice);
    fs::path thousands_abs_path = (dst_path / fs::path(thousands_folder_name));
    if (!fs::is_directory(thousands_abs_path))
        fs::create_directories(thousands_abs_path);
    ostringstream dst_file_name;
    dst_file_name << time_slice << ".json";
    fs::path dst_file = (thousands_abs_path /
                         fs::path(dst_file_name.str()));
    return dst_file.string();
}


/** @brief Return true if experiment initial settings already exist
 *
 */
bool experimentAlreadyExists(string experiment_folder) {
    fs::path dst_path = stringToPath(experiment_folder);
    return fs::exists(getEcosystemJSONPath(dst_path, 0));
}



/** @brief Convert float / double to string with n digits of precision
 *
 * @param[in] a_value Float or double input value
 * @param[in] n Number of decimal digits
 * @returns String containing number with the desired number of decimals
 */
template <typename T>
std::string to_string_with_precision(const T a_value, const int n)
{
    std::ostringstream out;
    out << std::fixed << std::setprecision(n) << a_value;
    return out.str();
}


/** @brief Initializer
 *
 * @param[in] experiment_folder String with experiment_folder path
 */
ExperimentInterface::ExperimentInterface(string experiment_folder,
                                         bool overwrite) {
    _setExperimentFolder(experiment_folder);
    _ecosystem = new Ecosystem();
    if (overwrite) {
        _cleanFolder();
        saveEcosystem();  // _ecosystem->time is 0, so we save initial settings
    } else {
        loadEcosystem(0); // load initial settings
    }
}

Ecosystem* ExperimentInterface::getEcosystemPointer() {
    return _ecosystem;
}

void ExperimentInterface::evolve() {
    _ecosystem->evolve();
}


void ExperimentInterface::lockEcosystem() {
    _mtx.lock();
}

bool ExperimentInterface::tryLockEcosystem() {
    return _mtx.try_lock();
}

void ExperimentInterface::unlockEcosystem() {
    _mtx.unlock();
}

void ExperimentInterface::saveEcosystem() {
    // get file name
    int curr_time = _ecosystem->time;
    string dst_file = getEcosystemJSONPath(_dst_path, curr_time);
    
    // export data
    ofstream f_data;
    f_data.open(dst_file, ios::out);
    json data_json;
    _ecosystem->serialize(data_json);
    f_data << data_json;
    f_data.close();
}

string ExperimentInterface::getExperimentSize() {
    double size = 0.0;
    for(bf::recursive_directory_iterator it(_dst_path);
        it!=bf::recursive_directory_iterator();
        ++it)
    {
        if(!is_directory(*it))
            size+=bf::file_size(*it);
    }
    double _directory_size = size / 1000000;
    return to_string_with_precision(_directory_size, 2);
}


/** @brief Set experiment folder
 *
 * If the folder doesn't exists, create it.
 *
 * @param[in] experiment_folder Path of experiment directory
 */
void ExperimentInterface::_setExperimentFolder(string experiment_folder) {
    _dst_path = stringToPath(experiment_folder);
    // Iterate over directory names to get experiment name (last name)
    // e.g. from "histories/exp_name" get: exp_name
    vector<string> parts;
    for(auto& part : _dst_path)
        parts.push_back(part.string());
    _experiment_name = parts[parts.size() - 1];
    
    if (!fs::is_directory(_dst_path))
        fs::create_directories(_dst_path);
}

string ExperimentInterface::getExperimentFolder() {
    return _dst_path.string();
}


void ExperimentInterface::_cleanFolder() {
    fs::remove_all(_dst_path);
    fs::create_directory(_dst_path);
}

void ExperimentInterface::loadEcosystem(int time_slice) {
    lockEcosystem();
    delete _ecosystem;
    // load json file
    ifstream f_data_json;
    f_data_json.open(getEcosystemJSONPath(_dst_path, time_slice));
    json data_json;
    f_data_json >> data_json;
    f_data_json.close();
    _ecosystem = new Ecosystem(data_json);
    unlockEcosystem();
}

int ExperimentInterface::getTimesHavingCompleteBackups() {
    // TODO: Do it with a vector.
    return _ecosystem->time;
}
