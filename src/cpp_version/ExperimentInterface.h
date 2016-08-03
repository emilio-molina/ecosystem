/*
  ==============================================================================

    ExperimentInterface.h
    Created: 16 May 2016 9:45:31pm
    Author:  Emilio Molina

  ==============================================================================
*/

#ifndef EXPERIMENTINTERFACE_H_INCLUDED
#define EXPERIMENTINTERFACE_H_INCLUDED

#include <mutex>
#include "ecosystem.h"
#include <boost/filesystem.hpp>

using namespace std;

/** @brief Class to easily interact with experiment folder
 */
class ExperimentInterface {
public:

    ExperimentInterface();
    

    void setExperimentFolder(string experiment_folder);
    

    bool experimentAlreadyExists();
    
    
    /** @brief Get a list of time slices containing a complete backup of ecosystem
     *
     * @returns List of time slices allowing complete backup
     */
    vector<int> getTimesHavingCompleteBackups();
    
    
    /** @brief Get path of JSON containing ecosystem data
     *
     * @param[in] time_slice Time slice for which ecosystem data will be get
     *
     * @returns Path of JSON file
     */
    string getEcosystemJSONPath(int time_slice);
    
    /** @brief Get a list of time slices containing a given organism parameter
     *
     * @param[in] parameter Parameter to read from JSON files
     *
     * @returns Vector of time values
     */
    vector<int> getTimesHavingOrganismParameter(string parameter);
    
    
    /** @brief Get a list of time slices containing a given ecosystem parameter
     *
     * @param[in] parameter Parameter to read from JSON files
     *
     * @returns Vector of time values
     */
    vector<int> getTimesHavingEcosystemParameter(string parameter);
    
    
    /** @brief Get a list of available ecosystem parameters
     *
     * @returns Vector of parameters available
     */
    vector<string> getAvailableEcosystemParameters();
    
    
    /** @brief Get a list of available organism parameters
     *
     * @returns Vector of parameters available
     */
    vector<string> getAvailableOrganismParameters();
    
    /** @brief Get ecosystem data for a given time slice
     *
     * @param[in] time_slice Target time slice
     */
    json getEcosystemData(int time_slice);
    
    /** @brief Get ecosystem data for a given time slice
     *
     * @param[in] organism_id Organism ID
     * @param[in] time_slice Target time slice
     */
    json getOrganismData(int organism_id, int time_slice);
    
    
    /** @brief Get a list of avaiable living organisms ids
     *
     * @param[in] time_slice Target time slice
     */
    vector<int> getLivingOrganismsIds(int time_slice);
    
    /** @brief Get a list of avaiable dead organisms ids
     *
     * @param[in] time_slice Target time slice
     */
    vector<int> getDeadOrganismsIds(int time_slice);
    
    /* @brief Get experiment size in MBs in format e.g. "321.16MB"
     *
     * @returns String with experiment size
     */
    string getExperimentSize();
    
    /* @brief Evolve one time slice ecosystem
     */
    void evolve();
    
    Ecosystem* getEcosystemPointer();
    
    void lockEcosystem();
    
    bool tryLockEcosystem();
    
    void unlockEcosystem();
    
    void loadEcosystem(int time_slice);
    
    void cleanFolder();
    
    void saveEcosystem();
    
    string getThousandsFolder(int time_slice);
    
private:
    string _path;
    mutex _mtx;
    fs::path _dst_path;
    string _experiment_name;
    Exporter* _exporter;
    Ecosystem* _ecosystem;
};



#endif  // EXPERIMENTINTERFACE_H_INCLUDED