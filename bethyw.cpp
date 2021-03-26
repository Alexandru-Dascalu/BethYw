/*
  +---------------------------------------+
  | BETH YW? WELSH GOVERNMENT DATA PARSER |
  +---------------------------------------+

  AUTHOR: 965337

  This file contains all the helper functions for initialising and running
  Beth Yw? In languages such as Java, this would be a class, but we really
  don't need a class here. Classes are for modelling data, and so forth, but
  here the code is pretty much a sequential block of code (BethYw::run())
  calling a series of helper functions.
*/

#include <iostream>
#include <string>
#include <tuple>
#include <unordered_set>
#include <vector>
#include <cstdlib>

#include "lib_cxxopts.hpp"

#include "areas.h"
#include "datasets.h"
#include "bethyw.h"
#include "input.h"

/*
  Run Beth Yw?, parsing the command line arguments, importing the data,
  and outputting the requested data to the standard output/error.

  Hint: cxxopts.parse() throws exceptions you'll need to catch. Read the cxxopts
  documentation for more information.

  @param argc
    Number of program arguments

  @param argv
    Program arguments

  @return
    Exit code
*/
int BethYw::run(int argc, char *argv[]) {
  auto cxxopts = BethYw::cxxoptsSetup();
  auto args = cxxopts.parse(argc, argv);

  // Print the help usage if requested
  if (args.count("help")) {
    std::cerr << cxxopts.help() << std::endl;
    return 0;
  }

  // Parse data directory argument
  std::string dir = args["dir"].as<std::string>() + DIR_SEP;

  // Parse other arguments and import data
  auto datasetsToImport = BethYw::parseDatasetsArg(args);
  auto areasFilter      = BethYw::parseAreasArg(args);
  auto measuresFilter   = BethYw::parseMeasuresArg(args);
  auto yearsFilter      = BethYw::parseYearsArg(args);

  Areas data = Areas();

   BethYw::loadAreas(data, dir, areasFilter);
//
//   BethYw::loadDatasets(data,
//                        dir,
//                        datasetsToImport,
//                        areasFilter,
//                        measuresFilter,
//                        yearsFilter);

  if (args.count("json")) {
    // The output as JSON
    std::cout << data.toJSON() << std::endl;
  } else {
    // The output as tables
     std::cout << data << std::endl;
  }

  return 0;
}

/*
  This function sets up and returns a valid cxxopts object. You do not need to
  modify this function.

  @return
     A constructed cxxopts object

  @example
    auto cxxopts = BethYw::cxxoptsSetup();
    auto args = cxxopts.parse(argc, argv);
*/
cxxopts::Options BethYw::cxxoptsSetup() {
  cxxopts::Options cxxopts(
        "bethyw",
        "Student ID: " + STUDENT_NUMBER + "\n\n"
        "This program is designed to parse official Welsh Government"
        " statistics data files.\n");
    
  cxxopts.add_options()(
      "dir",
      "Directory for input data passed in as files",
      cxxopts::value<std::string>()->default_value("datasets"))(

      "d,datasets",
      "The dataset(s) to import and analyse as a comma-separated list of codes "
      "(omit or set to 'all' to import and analyse all datasets)",
      cxxopts::value<std::vector<std::string>>())(

      "a,areas",
      "The areas(s) to import and analyse as a comma-separated list of "
      "authority codes (omit or set to 'all' to import and analyse all areas)",
      cxxopts::value<std::vector<std::string>>())(

      "m,measures",
      "Select a subset of measures from the dataset(s) "
      "(omit or set to 'all' to import and analyse all measures)",
      cxxopts::value<std::vector<std::string>>())(

      "y,years",
      "Focus on a particular year (YYYY) or "
      "inclusive range of years (YYYY-ZZZZ)",
      cxxopts::value<std::string>()->default_value("0"))(

      "j,json",
      "Print the output as JSON instead of tables.")(

      "h,help",
      "Print usage.");

  return cxxopts;
}

/*
  Parse the datasets argument passed into the command line. 

  The datasets argument is optional, and if it is not included, all datasets 
  should be imported. If it is included, it should be a comma-separated list of 
  datasets to import. If the argument contains the value "all"
  (case-insensitive), all datasets should be imported.

  This function validates the passed in dataset names against the codes in
  DATASETS array in the InputFiles namespace in datasets.h. If an invalid code
  is entered, throw a std::invalid_argument with the message:
  No dataset matches key: <input code>
  where <input name> is the name supplied by the user through the argument.

  @param args
    Parsed program arguments

  @return
    A std::vector of BethYw::InputFileSource instances to import

  @throws
    std::invalid_argument if the argument contains an invalid dataset with
    message: No dataset matches key <input code>

  @example
    auto cxxopts = BethYw::cxxoptsSetup();
    auto args = cxxopts.parse(argc, argv);

    auto datasetsToImport = BethYw::parseDatasetsArg(args);
 */
std::vector<BethYw::InputFileSource> BethYw::parseDatasetsArg(cxxopts::ParseResult& args) {

  // Create the container for the return type
  std::vector<InputFileSource> datasetsToImport;

  // You can get the std::vector of arguments from cxxopts like this.
  // Note that this function will throw an exception if datasets is not set as 
  // an argument. Check the documentation! Read it and understand it.
  try {
    auto inputDatasets = args["datasets"].as<std::vector<std::string>>();

    if(containsAllArgument(inputDatasets)) {
      addAllDatasets(datasetsToImport);
    } else {
      for(size_t i = 0; i < inputDatasets.size(); i++) {

        const InputFileSource* dataset = getInputSource(inputDatasets[i]);
        if(dataset != nullptr) {
          datasetsToImport.push_back(*dataset);
        } else {
          throw std::invalid_argument("No dataset matches key: invalid");
        }
      }
    }
  //catch exception thrown when dataset arguments are nor given
  } catch(const cxxopts::OptionParseException& ex) {
    addAllDatasets(datasetsToImport);
  } catch(const std::domain_error& ex) {
      addAllDatasets(datasetsToImport);
  }

  return datasetsToImport;
}

const BethYw::InputFileSource* BethYw::getInputSource(const std::string& datasetArg) {
  const size_t numDatasets = BethYw::InputFiles::NUM_DATASETS;
  const auto& allDatasets = BethYw::InputFiles::DATASETS;

  for(size_t i = 0; i < numDatasets; i++) {
    if(allDatasets[i].CODE == datasetArg) {
      return &allDatasets[i];
    }
  }

  return nullptr;
}

//why does this work if I pass in an unordered set?
bool BethYw::containsAllArgument(const std::vector<std::string>& arguments) {
  for(auto it = arguments.begin(); it != arguments.end(); it++) {
    if(*it == "all") {
      return true;
    }
  }

  return false;
}

void BethYw::addAllDatasets(std::vector<BethYw::InputFileSource>& datasetsToImport) {
  const auto& allDatasets = BethYw::InputFiles::DATASETS;
  const size_t numDatasets = BethYw::InputFiles::NUM_DATASETS;

  for(size_t i = 0; i < numDatasets; i++) {
      datasetsToImport.push_back(allDatasets[i]);
    }
}

/*
  Parses the areas command line argument, which is optional. If it doesn't 
  exist or exists and contains "all" as value (any case), all areas should be
  imported, i.e., the filter should be an empty set.

  Unlike datasets we can't check the validity of the values as it depends
  on each individual file imported (which hasn't happened until runtime).
  Therefore, we simply fetch the list of areas and later pass it to the
  Areas::populate() function.

  The filtering of inputs should be case insensitive.

  @param args
    Parsed program arguments

  @return 
    An std::unordered_set of std::strings corresponding to specific areas
    to import, or an empty set if all areas should be imported.

  @throws
    std::invalid_argument if the argument contains an invalid areas value with
    message: Invalid input for area argument
*/
std::unordered_set<std::string> BethYw::parseAreasArg(cxxopts::ParseResult& args) {
  std::unordered_set<std::string> areas;

  try{
    auto inputAreas = args["areas"].as<std::vector<std::string>>();
    
    //if arguments does not include all, add args from CLI to set
    if(!containsAllArgument(inputAreas)) {
      for(auto it = inputAreas.begin(); it != inputAreas.end(); it++) {
        areas.insert(*it);
      }
      return areas;
    }

    //set might be empty or not, depending on if arguments includes all
    return areas;
  } catch(const cxxopts::OptionParseException& ex) {
      //return set that is empty
      return areas;
  } catch(const std::domain_error& ex) {
      return areas;
  }
}

/*
  Parse the measures command line argument, which is optional. If it doesn't
  exist or exists and contains "all" as value (any case), all measures should
  be imported.

  Unlike datasets we can't check the validity of the values as it depends
  on each individual file imported (which hasn't happened until runtime).
  Therefore, we simply fetch the list of measures and later pass it to the
  Areas::populate() function.

  The filtering of inputs should be case insensitive.

  @param args
    Parsed program arguments

  @return 
    An std::unordered_set of std::strings corresponding to specific measures
    to import, or an empty set if all measures should be imported.

  @throws
    std::invalid_argument if the argument contains an invalid measures value
    with the message: Invalid input for measures argument
*/
std::unordered_set<std::string> BethYw::parseMeasuresArg(cxxopts::ParseResult& args) {
  std::unordered_set<std::string> measures;

  try{
    auto inputMeasures = args["measures"].as<std::vector<std::string>>();

    //if arguments does not include all, add args from CLI to set
    if(!containsAllArgument(inputMeasures)) {
      for(auto it = inputMeasures.begin(); it != inputMeasures.end(); it++) {
        measures.insert(*it);
      }
    }

    //set might be empty or not, depending on if arguments includes all
    return measures;
  } catch(const cxxopts::OptionParseException& ex) {
    //return set that is empty
    return measures;
  } catch(const std::domain_error& ex) {
      return measures;
  }
}

/*
  Parse the years command line argument. Years is either a four digit year 
  value, or two four digit year values separated by a hyphen (i.e. either 
  YYYY or YYYY-ZZZZ).

  This should be parsed as two integers and inserted into a std::tuple,
  representing the start and end year (inclusive). If one or both values are 0,
  then there is no filter to be applied. If no year argument is given return
  <0,0> (i.e. to import all years). You will have to search
  the web for how to construct std::tuple objects! 

  @param args
    Parsed program arguments

  @return
    A std::tuple containing two unsigned ints

  @throws
    std::invalid_argument if the argument contains an invalid years value with
    the message: Invalid input for years argument
*/
std::tuple<unsigned int, unsigned int> BethYw::parseYearsArg(cxxopts::ParseResult& args) {
  try{
    auto inputYears = args["years"].as<std::string>();

    //check if string represents an int
    if(isInt(inputYears)) {
      int year = std::stoi(inputYears);

      if(is4DigitInt(year)) {
        return std::tuple<unsigned int, unsigned int>(year, year);

      } else if (year == 0) {
        return std::tuple<unsigned int, unsigned int>(0, 0);

      } else {
        throw std::invalid_argument("Invalid input for years argument");
      }
    //if that fails, then the argument is not an int, check if it is a range
    } else {
      size_t hyphenIndex = inputYears.find('-');

      //if it does not have a hyphen, it is invalid
      if(hyphenIndex == std::string::npos) {
        throw std::invalid_argument("Invalid input for years argument");
      }

      std::string beforeHyphen = inputYears.substr(0, hyphenIndex);
      std::string afterHyphen = inputYears.substr(hyphenIndex + 1);

      if(isInt(beforeHyphen) && isInt(afterHyphen)) {
        int firstYear = std::stoi(inputYears.substr(0, hyphenIndex));
        int secondYear = std::stoi(inputYears.substr(hyphenIndex + 1));

        if(is4DigitInt(firstYear) && is4DigitInt(secondYear)) {
          return std::tuple<unsigned int, unsigned int>(firstYear, secondYear);

        } else if(firstYear == 0 || secondYear == 0) {
          return std::tuple<unsigned int, unsigned int>(0, 0);

        } else {
          throw std::invalid_argument("Invalid input for years argument");
        }
      //if not both parts of string before and after hyphen are ints, then arg is invalid
      } else {
        throw std::invalid_argument("Invalid input for years argument");
      }
    }
  } catch(const cxxopts::OptionParseException& ex) {
    return std::tuple<unsigned int, unsigned int>(0, 0);
  } catch(const std::domain_error& ex) {
      return std::tuple<unsigned int, unsigned int>(0, 0);
  }
}

bool BethYw::isInt(const std::string& str) {
  /*strtol will put a value in end which is the first character after the
   * integer in the string. We can check if the whole string is an int by 
   * seeing if end points to the end of string character.*/
  char* end;
  strtol(str.c_str(), &end, 10);

  return *end == '\0';
}

bool BethYw::is4DigitInt(const int num) {
    return 999 < num && num < 10000;
}

/*
  Load the areas.csv file from the directory `dir`. Parse the file and
  create the appropriate Area objects inside the Areas object passed to
  the function in the `areas` argument.

  areas.csv is guaranteed to be formatted as:
    Local authority code,Name (eng),Name (cym)

  Hint: To implement this function. First you will need create an InputFile 
  object with the filename of the areas file, open it, and then pass reference 
  to the stream to the Areas::populate() function.

  Hint 2: you can retrieve the specific filename for a dataset, e.g. for the 
  areas.csv file, from the InputFileSource's FILE member variable

  @param areas
    An Areas instance that should be modified (i.e. the populate() function
    in the instance should be called)

  @param dir
    Directory where the areas.csv file is

  @param areasFilter
    An unordered set of areas to filter, or empty to import all areas

  @return
    void
*/
void BethYw::loadAreas(Areas& areas, const std::string& filePath, const StringFilterSet& filters) {
    InputFile file(filePath);
    areas.populate(file.open(), BethYw::AuthorityCodeCSV, BethYw::InputFiles::AREAS.COLS, &filters);
}

/*
  Import datasets from `datasetsToImport` as files in `dir` into areas, and
  filtering them with the `areasFilter`, `measuresFilter`, and `yearsFilter`.

  The actual filtering will be done by the Areas::populate() function, thus 
  you need to merely pass pointers on to these flters.

  This function should promise not to throw an exception. If there is an
  error/exception thrown in any function called by thus function, catch it and
  output 'Error importing dataset:', followed by a new line and then the output
  of the what() function on the exception.

  @param areas
    An Areas instance that should be modified (i.e. datasets loaded into it)

  @param dir
    The directory where the datasets are

  @param datasetsToImport
    A vector of InputFileSource objects

  @param areasFilter
    An unordered set of areas (as authority codes encoded in std::strings)
    to filter, or empty to import all areas

  @param measuresFilter
    An unordered set of measures (as measure codes encoded in std::strings)
    to filter, or empty to import all measures

  @param yearsFilter
    An two-pair tuple of unsigned ints corresponding to the range of years 
    to import, which should both be 0 to import all years.

  @return
    void

  @example
    Areas areas();

    BethYw::loadDatasets(
      areas,
      "data",
      BethYw::parseDatasetsArgument(args),
      BethYw::parseAreasArg(args),
      BethYw::parseMeasuresArg(args),
      BethYw::parseYearsArg(args));
*/


/*Code inspired from https://thispointer.com/converting-a-string-to-upper-lower-case-in-c-using-stl-boost-library/#:~:text=Convert%20a%20String%20to%20Lower%20Case%20using%20STL&text=int%20tolower%20(%20int%20c%20)%3B,function%20each%20of%20them%20i.e.*/
std::string BethYw::toLower(const std::string& str) {
    std::string copy = str;
    std::for_each(copy.begin(), copy.end(), [](char& c) {
        c = ::tolower(c);
    });

    return copy;
}
