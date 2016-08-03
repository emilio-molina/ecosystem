/*
  ==============================================================================

    ExperimentInterface.cpp
    Created: 16 May 2016 9:45:31pm
    Author:  Emilio Molina

  ==============================================================================
*/

#include "ExperimentInterface.h"

namespace bf=boost::filesystem;


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

string ExperimentInterface::getEcosystemJSONPath(int time_slice) {
    string thousands_folder_name = getThousandsFolder(time_slice);
    fs::path thousands_abs_path = (_dst_path / fs::path(thousands_folder_name));
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
bool ExperimentInterface::experimentAlreadyExists() {
    return fs::exists(getEcosystemJSONPath(0));
}

/** @brief Initializer
 *
 * @param[in] experiment_folder String with experiment_folder path
 */
ExperimentInterface::ExperimentInterface() {
    _ecosystem = new Ecosystem();
}

Ecosystem* ExperimentInterface::getEcosystemPointer() {
    return _ecosystem;
}

void ExperimentInterface::evolve() {
    _ecosystem->evolve();
}

void ExperimentInterface::loadEcosystem(int time_slice) {
    lockEcosystem();
    delete _ecosystem;
    _ecosystem = new Ecosystem(getEcosystemJSONPath(time_slice));
    unlockEcosystem();
}

/** @brief Set experiment folder
 *
 * If the folder doesn't exists, create it.
 *
 * @param[in] experiment_folder Path of experiment directory
 */
void ExperimentInterface::setExperimentFolder(string experiment_folder) {
    _dst_path = fs::path(experiment_folder + fs::path::preferred_separator).normalize();
    if (_dst_path.filename() == ".")
        _dst_path.remove_leaf();
    // Iterate over directory names to get experiment name (last name)
    // e.g. from "histories/exp_name/." get: exp_name
    vector<string> parts;
    for(auto& part : _dst_path)
        parts.push_back(part.string());
    _experiment_name = parts[parts.size() - 2];
    
    if (!fs::is_directory(_dst_path))
        fs::create_directories(_dst_path);
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

void ExperimentInterface::cleanFolder() {
    fs::remove_all(_dst_path);
    fs::create_directory(_dst_path);
}

void ExperimentInterface::saveEcosystem() {
    // get file name
    int curr_time = _ecosystem->time;
    string dst_file = getEcosystemJSONPath(curr_time);
    
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

string ExperimentInterface::getThousandsFolder(int time_slice) {
    int thousands = (time_slice / 1000) * 1000;
    ostringstream thousands_folder;
    thousands_folder << thousands << "_to_" << thousands + 999;
    return thousands_folder.str();
}