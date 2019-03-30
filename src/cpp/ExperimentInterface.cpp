/** @file ExperimentInterface.cpp
 * @brief ExperimentInterface definition
 *
 * @ingroup core
 */


#include "ExperimentInterface.h"
#include <regex>
#include <stdlib.h>
#include <algorithm>
#include <iterator>
#include <fstream>
#include <sstream>
#include <cstdio>
#include <boost/iostreams/filtering_stream.hpp>
#include <boost/iostreams/filter/gzip.hpp>
#include <boost/iostreams/copy.hpp>

namespace bf=boost::filesystem;
namespace bio=boost::iostreams;


void split(const string &s, char delim, vector<string> &elems) {
    stringstream ss(s);
    string item;
    while (getline(ss, item, delim)) {
        elems.push_back(item);
    }
}

/** @brief Auxiliar function to split a string
 *
 * @param[in] s Input string
 * @param[in] delim Delimiter
 *
 * @return Vector of strings
 */
vector<string> split(const string &s, char delim) {
    vector<string> elems;
    split(s, delim, elems);
    return elems;
}


/** @brief Auxiliar function to convert a string to a bs::path
 *
 * @param[in] experiment_folder String with full path
 *
 * @return Normalized path in type bs::path
 */
fs::path stringToPath(string path_str) {
    fs::path path_path = fs::path(path_str + fs::path::preferred_separator).normalize();
    if (path_path.filename() == ".")
    path_path.remove_leaf();
    return path_path;
}


/** @brief Get path of file containing ecosystem data
 *
 * @param[in] dst_path Path of destination folder
 * @param[in] time_slice Time slice for which ecosystem data will be get
 *
 * @returns Path of file
 */
string getEcosystemGenericPath(fs::path dst_path, int time_slice, string file_extension) {
    ostringstream dst_file_name;
    char time_slice_formatted[9];
    sprintf(time_slice_formatted, "%08d", time_slice);
    dst_file_name << "bk_"<< time_slice_formatted << file_extension;
    fs::path dst_file =  (dst_path /
                         fs::path(dst_file_name.str()));
    return dst_file.string();
}



/** @brief Get path of JSON containing ecosystem data
 *
 * @param[in] dst_path Path of destination folder
 * @param[in] time_slice Time slice for which ecosystem data will be get
 *
 * @returns Path of JSON file
 */
string getEcosystemJSONPath(fs::path dst_path, int time_slice) {
    return getEcosystemGenericPath(dst_path, time_slice, ".zjson");
}


/** @brief Get path of TGA containing ecosystem data
 *
 * @param[in] dst_path Path of destination folder
 * @param[in] time_slice Time slice for which ecosystem data will be get
 *
 * @returns Path of TGA file
 */
string getEcosystemTGAPath(fs::path dst_path, int time_slice) {
    return getEcosystemGenericPath(dst_path, time_slice, ".tga");
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

/** @brief Compress stringstream data using zlib
 *
 * @param[in] decompressed Input decompressed data
 * @param[out] compressed Output compressed data
 */
void compressData(stringstream &decompressed, stringstream &compressed)
{
    boost::iostreams::filtering_streambuf<boost::iostreams::input> out;
    out.push(boost::iostreams::zlib_compressor());
    out.push(decompressed);
    bio::copy(out, compressed);
}


/** @brief Decompress stringstream data using zlib
 *
 * @param[in] compressed Input compressed data
 * @param[out] decompressed Output decompressed data
 */
void decompressData(stringstream &compressed, stringstream &decompressed)
{
    boost::iostreams::filtering_streambuf<boost::iostreams::input> in;
    in.push(boost::iostreams::zlib_decompressor());
    in.push(compressed);
    bio::copy(in, decompressed);
}


/** @brief Initializer
 *
 * @param[in] experiment_folder String with experiment_folder path
 */
ExperimentInterface::ExperimentInterface(string experiment_folder,
                                         bool overwrite) {
    _setExperimentFolder(experiment_folder);
    vector<int> timesHavingCompleteBackups = getTimesHavingCompleteBackups();
    if (timesHavingCompleteBackups.size() == 0)
        overwrite = true;
    _ecosystem = new Ecosystem();
    if (overwrite) {
        _cleanFolder();
	drawEcosystem();
        saveEcosystem();  // _ecosystem->time is 0, so we save initial settings
    } else {
        loadEcosystem(timesHavingCompleteBackups.back());
    }
}


/** @brief Get pointer to ecosystem object
 */
Ecosystem* ExperimentInterface::getEcosystemPointer() {
    return _ecosystem;
}

/** @brief Get a copy of _ecosystem.settings_json
     */
json* ExperimentInterface::getSettings_json_ptr() {
    return _ecosystem->getSettings_json_ptr();
}

/** @brief Make ecosystem evolve one time slice
 */
void ExperimentInterface::evolve() {
    _ecosystem->evolve();
    if (getRunningTime() % getBackupPeriod() == 0)
        saveEcosystem();
    if (getRunningTime() % getDrawingPeriod() == 0)
        drawEcosystem();
}

/** @brief Lock ecosystem to avoid concurrency conflicts
 */
void ExperimentInterface::lockEcosystem() {
    _mtx.lock();
}

/** @brief Return true if locking operation succeeds
 */
bool ExperimentInterface::tryLockEcosystem() {
    return _mtx.try_lock();
}


/** @brief Unlock ecosystem
 */
void ExperimentInterface::unlockEcosystem() {
    _mtx.unlock();
}


/** @brief Save current time slice to disk
 */
void ExperimentInterface::saveEcosystem() {
    // get file name
    int curr_time = _ecosystem->time;
    string dst_file = getEcosystemJSONPath(_dst_path, curr_time);
    
    json data_json;
    _ecosystem->serialize(data_json);
    stringstream data_uncompressed;
    stringstream data_compressed;
    data_uncompressed << data_json;
    compressData(data_uncompressed, data_compressed);

    // export data
    ofstream f_data;
    f_data.open(dst_file, ios::out);
    f_data << data_compressed.rdbuf();
    f_data.close();
}


TGAColor organismToColour(Organism* o) {
    string ORGANISM_TYPE = o->species;
    float r = 0.0f;
    float g = 0.0f;
    float b = 0.0f;
    float a = 1.0f;
    if (ORGANISM_TYPE == "P") {
        // green
        r = 0.0f;
        g = 1.0f;
        b = 0.0f;
    } else if (ORGANISM_TYPE == "H1") {
        // grey
        r = 0.5f;
        g = 0.5f;
        b = 0.5f;
    } else if (ORGANISM_TYPE == "H2") {
        // blue
        r = 0.2f;
        g = 0.2f;
        b = 1.0f;
    } else if (ORGANISM_TYPE == "C1") {
        // red
        r = 1.0f;
        g = 0.0f;
        b = 0.0f;
    } else if (ORGANISM_TYPE == "C2") {
        // orange
        r = 1.0f;
        g = 0.5f;
        b = 0.0f;
    } else if (ORGANISM_TYPE == "C3") {
        // light blue
        r = 0.0f;
        g = 0.5f;
        b = 1.0f;
    }
    float energy_ratio = (float)o->energy_reserve / o->initial_energy_reserve;
    float age_ratio = 1.0f - (float)o->age / o->death_age;
    a = 0.5 * energy_ratio * age_ratio;
    r *= a;
    g *= a;
    b *= a;
    return TGAColor(
        (unsigned char)(r * 255),
        (unsigned char)(g * 255),
        (unsigned char)(b * 255));
}


/** @brief Draw current time slice to TGA image into disk
 */
void ExperimentInterface::drawEcosystem() {
    // get file name
    int zoom_factor = getDrawingZoomFactor();
    int curr_time = _ecosystem->time;
    string dst_file = getEcosystemTGAPath(_dst_path, curr_time);
    TGAImage frame(_ecosystem->biotope_size_x * zoom_factor,
                   _ecosystem->biotope_size_y * zoom_factor,
                   TGAImage::RGB);
    for (auto o:_ecosystem->biotope) {
        tuple<int, int> position = o.first;
        string ORGANISM_TYPE = o.second->species;
        int x = get<0>(position);
        int y = get<1>(position);
        TGAColor color_o = organismToColour(o.second);
	for (int fx=0; fx<zoom_factor; fx++)
	    for (int fy=0; fy<zoom_factor; fy++)
                frame.set(zoom_factor*x+fx, zoom_factor*y+fy, color_o);
    }
    frame.write_tga_file(dst_file.c_str());
}


/* @brief Get experiment size in MBs in format e.g. "321.16MB"
 *
 * @returns String with experiment size
 */
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


/** @brief Get experiment folder in string type
 */
string ExperimentInterface::getExperimentFolder() {
    return _dst_path.string();
}

/** @brief Delete content of experiment folder
 */
void ExperimentInterface::_cleanFolder() {
    fs::remove_all(_dst_path);
    fs::create_directory(_dst_path);
}

/** @brief Load a given time slice into ecosystem object
 *
 * @param[in] time_slice Time value to load
 */
void ExperimentInterface::loadEcosystem(int time_slice) {
    lockEcosystem();
    delete _ecosystem;
    
    // load json file
    ifstream f_data_json;
    f_data_json.open(getEcosystemJSONPath(_dst_path, time_slice));
    stringstream compressed;
    stringstream decompressed;
    compressed << f_data_json.rdbuf();
    decompressData(compressed, decompressed);
    json data_json;
    decompressed >> data_json;
    f_data_json.close();
    _ecosystem = new Ecosystem(data_json);
    unlockEcosystem();
}

/** @brief Get a list of time slices containing a complete backup of ecosystem
 *
 * @returns List of time slices allowing complete backup
 */
vector<int> ExperimentInterface::getTimesHavingCompleteBackups() {
    // TODO: Do it with a vector.
    vector<int> times;
    for(bf::recursive_directory_iterator it(_dst_path);
        it!=bf::recursive_directory_iterator();
        ++it)
    {
        if(!is_directory(*it)) {
            string path = (*it).path().string();
            std::string result;
            std::regex re("/bk_(\\d+)\\.zjson");
            std::smatch match;
            if (std::regex_search(path, match, re) && match.size() > 1) {
                result = match.str(1);
                int json_num = atoi(result.c_str());
                times.push_back(json_num);
            }
        }
    }
    sort(times.begin(), times.end());
    return times;
}

/** @brief Get time of running ecosystem
 */
int ExperimentInterface::getRunningTime() {
    if (_ecosystem == nullptr)
        return 0;
    else
        return _ecosystem->time;
}

/** @brief Get backup period
 */
int ExperimentInterface::getDrawingZoomFactor() {
    return (*getSettings_json_ptr())["constants"]["DRAWING_ZOOM_FACTOR"];
}

/** @brief Get drawing period
 */
int ExperimentInterface::getDrawingPeriod() {
    return (*getSettings_json_ptr())["constants"]["DRAWING_PERIOD"];
}

/** @brief Get backup period
 */
int ExperimentInterface::getBackupPeriod() {
    return (*getSettings_json_ptr())["constants"]["BACKUP_PERIOD"];
}
