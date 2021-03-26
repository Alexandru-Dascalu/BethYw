#ifndef BETHYW_H_
#define BETHYW_H_

/*
  +---------------------------------------+
  | BETH YW? WELSH GOVERNMENT DATA PARSER |
  +---------------------------------------+

  AUTHOR: 965337

  This file contains declarations for the helper functions for initialising and
  running Beth Yw?
 */

#include <string>
#include <unordered_set>
#include <vector>

#include "lib_cxxopts.hpp"

#include "datasets.h"
#include "areas.h"

const char DIR_SEP =
#ifdef _WIN32
        '\\';
#else
        '/';
#endif

namespace BethYw {

    const std::string STUDENT_NUMBER = "965337";

    /*
      Run Beth Yw?, parsing the command line arguments and acting upon them.
    */
    int run(int argc, char* argv[]);

    /*
      Create a cxxopts instance.
    */
    cxxopts::Options cxxoptsSetup();

    /*
      Parse the datasets argument and return a std::vector of all the datasets
      to import. InputFileSource is declared in datasets.h.
    */
    std::vector<BethYw::InputFileSource> parseDatasetsArg(
            cxxopts::ParseResult& args);

    //functions I wrote to help me parse dataset argument, meant to be private
    const BethYw::InputFileSource* getInputSource(const std::string& datasetArg);
    bool containsAllArgument(const std::vector<std::string>& argument);
    void addAllDatasets(std::vector<BethYw::InputFileSource>& datasetsToImport);

    /*
      Parse the areas argument and return a std::unordered_set of all the
      areas to import, or an empty set if all areas should be imported.
    */
    std::unordered_set<std::string> parseAreasArg(cxxopts::ParseResult& args);

    /*
      Parse the measures argument and return a std::unordered_set of all the
      measures to import, or an empty set if all measures should be imported.
    */
    std::unordered_set<std::string> parseMeasuresArg(cxxopts::ParseResult& args);

    /*
      Parse the years argument and return a std::unordered_set of all the
      years for which we want values, or an empty set if values for all years should be imported.
    */
    std::tuple<unsigned int, unsigned int> parseYearsArg(cxxopts::ParseResult& args);

    /*other helper functions I made to help with parsing years, they are also used in areas.cpp in
     * populateFromAuthorityByYearCSV*/
    bool is4DigitInt(const int num);
    bool isInt(const std::string& str);
    bool isDouble(const std::string& str);

    //helper function used in other files, should be public
    std::string toLower(const std::string& str);
    std::string toUpper(const std::string& str);

    void loadAreas(Areas& areas, const std::string& filePath, const StringFilterSet& filters);
    void loadDatasets(Areas& areas, const std::string& dir,
                      const std::vector<BethYw::InputFileSource>& datasetsFilter,
                      const std::unordered_set<std::string>& areasFilter,
                      const std::unordered_set<std::string>& measuresFilter,
                      const std::tuple<unsigned int, unsigned int>& yearsFilter) noexcept;
} // namespace BethYw

#endif // BETHYW_H_