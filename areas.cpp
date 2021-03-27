/*
  +---------------------------------------+
  | BETH YW? WELSH GOVERNMENT DATA PARSER |
  +---------------------------------------+

  AUTHOR: 965337

  The file contains the Areas class implementation. Areas are the top
  level of the data structure in Beth Yw? for now.

  Areas is also responsible for importing data from a stream (using the
  various populate() functions) and creating the Area and Measure objects.
*/

#include <stdexcept>
#include <iostream>
#include <string>
#include <sstream>
#include <stdexcept>
#include <tuple>
#include <unordered_set>
#include <map>

#include "lib_json.hpp"
#include "datasets.h"
#include "areas.h"
#include "measure.h"
#include "bethyw.h"

/*
  An alias for the imported JSON parsing library.
*/
using json = nlohmann::json;

/*
  Constructor for an Areas object.

  @example
    Areas data = Areas();
*/
Areas::Areas() : areas(std::map<std::string, Area>()) {

}

/*
  Add a particular Area to the Areas object.

  If an Area already exists with the same local authority code, overwrite all
  data contained within the existing Area with those in the new
  Area (i.e. they should be combined, but the new Area's data should take
  precedence, e.g. replace a name with the same language identifier).

  @param localAuthorityCode
    The local authority code of the Area

  @param area
    The Area object that will contain the Measure objects

  @return
    void
*/
void Areas::setArea(const std::string& localAuthorityCode, const Area& area) noexcept {
    if (areas.find(localAuthorityCode) == areas.end()) {
        areas.insert(std::pair<std::string, Area>(localAuthorityCode, area));
    } else {
        areas.find(localAuthorityCode)->second = area;
    }
}

/*
  Retrieve an Area instance with a given local authority code.

  @param localAuthorityCode
    The local authority code to find the Area instance of

  @return
    An Area object

  @throws
    std::out_of_range if an Area with the set local authority code does not
    exist in this Areas instance
*/
Area& Areas::getArea(const std::string& localAuthorityCode) {
    try {
        return areas.at(localAuthorityCode);
    } catch (std::out_of_range& ex) {
        throw std::out_of_range(std::string("No area found matching ") + localAuthorityCode);
    }
}

/*
  Retrieve the number of Areas within the container. This function should be
  callable from a constant context, not modify the state of the instance, and
  must promise not throw an exception.

  @return
    The number of Area instances

  @example
    Areas data = Areas();
    std::string localAuthorityCode = "W06000023";
    Area area(localAuthorityCode);
    data.setArea(localAuthorityCode, area);
    
    auto size = areas.size(); // returns 1
*/
int Areas::size() const noexcept {
    return areas.size();
}

/*
  This function specifically parses the compiled areas.csv file of local
  authority codes, and their names in English and Welsh.

  This is a simple dataset that is a comma-separated values file (CSV), where
  the first row gives the name of the columns, and then each row is a set of
  data.

  For this coursework, you can assume that areas.csv will always have the same
  three columns in the same order.

  Once the data is parsed, you need to create the appropriate Area objects and
  insert them in to a Standard Library container within Areas.

  @param is
    The input stream from InputSource

  @param cols
    A map of the enum BethyYw::SourceColumnMapping (see datasets.h) to strings
    that give the column header in the CSV file

  @param areasFilter
    An umodifiable pointer to set of umodifiable strings for areas to import,
    or an empty set if all areas should be imported

  @return
    void

  @throws 
    std::runtime_error if a parsing error occurs (e.g. due to a malformed file)
    std::out_of_range if there are not enough columns in cols
*/
void Areas::populateFromAuthorityCodeCSV(std::istream& is, const BethYw::SourceColumnMapping& cols,
                                         const StringFilterSet* const areasFilter) {

    if (cols.size() != 3) {
        throw std::out_of_range("Not enough columns in cols mapping!");
    }

    std::string line;
    std::stringstream lineStream;

    //read and throw away first line, containing headings
    std::getline(is, line);

    while (std::getline(is, line)) {
        lineStream.str(line);
        lineStream.clear();

        std::string authorityCode;
        std::string englishName;
        std::string welshName;

        std::getline(lineStream, authorityCode, ',');
        std::getline(lineStream, englishName, ',');
        std::getline(lineStream, welshName, ',');

        if (authorityCode.empty() || englishName.empty() || welshName.empty()) {
            throw std::runtime_error("Line does not have three comma separated values!");
        }

        /*We only add the area if we should all areas or if the filter specified this area code. We make sure to
         * put the condition for the null pointer first so we do not dereference it later on.*/
        if (areasFilter == nullptr || areasFilter->empty() || areasFilter->find(authorityCode) != areasFilter->end()) {
            Area newArea = Area(authorityCode);
            newArea.setName("eng", englishName);
            newArea.setName("cym", welshName);

            setArea(authorityCode, newArea);
        }
    }
}

/*
  Data from StatsWales is in the JSON format, and contains three
  top-level keys: odata.metadata, value, odata.nextLink. value contains the
  data we need. Rather than been hierarchical, it contains data as a
  continuous list (e.g. as you would find in a table). For each row in value,
  there is a mapping of various column headings and their respective vaues.

  In this function, you will have to parse the JSON datasets, extracting
  the local authority code, English name (the files only contain the English
  names), and each measure by year.

  If you encounter an Area that does not exist in the Areas container, you
  should create the Area object

  If areasFilter is a non-empty set only include areas matching the filter. If
  measuresFilter is a non-empty set only include measures matching the filter.
  If yearsFilter is not equal to <0,0>, only import years within the range
  specified by the tuple (inclusive).

  I've provided the column names for each JSON file that you need to parse
  as std::strings in datasets.h. This mapping should be passed through to the
  cols parameter of this function.

  Note that in the JSON format, years are stored as strings, but we need
  them as ints. When retrieving values from the JSON library, you will
  have to cast them to the right type.

  @param is
    The input stream from InputSource

  @param cols
    A map of the enum BethyYw::SourceColumnMapping (see datasets.h) to strings
    that give the column header in the CSV file

  @param areasFilter
    An umodifiable pointer to set of umodifiable strings of areas to import,
    or an empty set if all areas should be imported

  @param measuresFilter
    An umodifiable pointer to set of umodifiable strings of measures to import,
    or an empty set if all measures should be imported

  @param yearsFilter
    An umodifiable pointer to an umodifiable tuple of two unsigned integers,
    where if both values are 0, then all years should be imported, otherwise
    they should be treated as the range of years to be imported (inclusively)

  @return
    void

  @throws 
    std::runtime_error if a parsing error occurs (e.g. due to a malformed file)
    std::out_of_range if there are not enough columns in cols
*/
void Areas::populateFromWelshStatsJSON(std::istream& is, const BethYw::SourceColumnMapping& cols,
                                       const std::unordered_set<std::string>* const areasFilter,
                                       const std::unordered_set<std::string>* const measuresFilter,
                                       const std::tuple<unsigned int, unsigned int>* const yearsFilter) {
    json j;
    is >> j;

    bool isTrainDataset = cols == BethYw::InputFiles::TRAINS.COLS;
    auto& jsonUsefulData = Areas::safeGet(j, "value");

    for (auto& element: jsonUsefulData.items()) {
        auto& data = element.value();
        const std::string& authorityCode = Areas::safeGet(data, cols.at(BethYw::SourceColumn::AUTH_CODE));

        if (Areas::isIncludedInFilter(areasFilter, authorityCode, true)) {
            /*I wanted to have the measure code be a const reference, and thus it needs to be initialized when it is
             * declared. Therefore, I could only do it with a ternary operator. If the dataset file is the train one,
             * the measure code is a hardcoded value, else it is a value we need to read from the json data.*/
            const std::string& measureCode = isTrainDataset ? BethYw::InputFiles::TRAINS.COLS.at(BethYw::SourceColumn::SINGLE_MEASURE_CODE)
                    : (const std::string&) Areas::safeGet(data, cols.at(BethYw::SourceColumn::MEASURE_CODE));

            if (Areas::isIncludedInFilter(measuresFilter, measureCode, false)) {
                unsigned int year = Areas::parseYear(Areas::safeGet(data, cols.at(BethYw::SourceColumn::YEAR)));

                if (Areas::isInYearRange(yearsFilter, year)) {
                    const std::string& areaEngName = Areas::safeGet(data, cols.at(BethYw::SourceColumn::AUTH_NAME_ENG));

                    double value = 0;
                    //unlike the others, environment data set stores the double values as strings. We need to account for that.
                    if(cols == BethYw::InputFiles::AQI.COLS) {
                        std::string valueAsString = Areas::safeGet(data, cols.at(BethYw::SourceColumn::VALUE));
                        value = std::stod(valueAsString);
                    } else {
                        value = Areas::safeGet(data, cols.at(BethYw::SourceColumn::VALUE));
                    }

                    //same as before with the measure code, we need to check if the file is the train dataset
                    const std::string& measureLabel = isTrainDataset ? BethYw::InputFiles::TRAINS.COLS.at(BethYw::SourceColumn::SINGLE_MEASURE_NAME)
                            : (const std::string&) Areas::safeGet(data, cols.at(BethYw::SourceColumn::MEASURE_NAME));

                    /* we use the logic in the overloaded copy assignment operators to insert the new area/measure, or
                     * merge them with existing objects.*/
                    Area newArea = Area(authorityCode);
                    newArea.setName("eng", areaEngName);

                    Measure newMeasure = Measure(measureCode, measureLabel);
                    newMeasure.setValue(year, value);
                    newArea.setMeasure(measureCode, newMeasure);

                    this->setArea(authorityCode, newArea);
                }
            }
        }
    }
}

/*
 * Thin wrapper over the json.at method. The block comment for loadFromWelshStats requires that runtime_error be thrown
 * when the json file is malformed. The json.at() method throws out_of_range when trying to access a key that does not
 * exist. This method catches that error and throws runtime_error in its place, to conform to the specifications.*/
const json& Areas::safeGet(const json& data, const std::string& key) {
    try {
        return data.at(key);
    } catch (const std::out_of_range& ex) {
        throw std::runtime_error(std::string("Malformed JSON file! No value for key:") + key);
    }
}

bool Areas::isIncludedInFilter(const std::unordered_set<std::string>* const filter, const std::string& data,
                               bool toUpper) {
    if (filter == nullptr || filter->empty()) {
        return true;
    } else {
        if (toUpper) {
            std::string upperCaseData = BethYw::toUpper(data);

            return filter->find(upperCaseData) != filter->end();
        } else {
            std::string lowerCaseData = BethYw::toLower(data);

            return filter->find(lowerCaseData) != filter->end();
        }

        return false;
    }
}

bool Areas::isInYearRange(const std::tuple<unsigned int, unsigned int>* const yearRange, unsigned int year) {
    if (std::get<0>(*yearRange) == 0 && std::get<1>(*yearRange) == 0) {
        return true;
    } else if (year >= std::get<0>(*yearRange) && year <= std::get<1>(*yearRange)) {
        return true;
    } else {
        return false;
    }
}

unsigned int Areas::parseYear(const std::string& str) {
    /*strtol will put a value in end which is the first character after the
     * integer in the string. We can check if the whole string is an int by
     * seeing if end points to the end of string character.*/
    char* end;
    unsigned int year = strtol(str.c_str(), &end, 10);

    if (*end == '\0') {
        return year;
    } else {
        throw std::runtime_error(std::string("Year value can not be parsed as unsigned int: ") + str);
    }
}

/*
  This function imports CSV files that contain a single measure. The 
  CSV file consists of columns containing the authority code and years.
  Each row contains an authority code and values for each year (or no value
  if the data doesn't exist).

  Note that these files do not include the names for areas, instead you 
  have to rely on the names already populated through 
  Areas::populateFromAuthorityCodeCSV();

  The datasets that will be parsed by this function are
   - complete-popu1009-area.csv
   - complete-popu1009-pop.csv
   - complete-popu1009-opden.csv

  @param is
    The input stream from InputSource

  @param cols
    A map of the enum BethyYw::SourceColumnMapping (see datasets.h) to strings
    that give the column header in the CSV file

  @param areasFilter
    An umodifiable pointer to set of umodifiable strings for areas to import,
    or an empty set if all areas should be imported

  @param measuresFilter
    An umodifiable pointer to set of strings for measures to import, or an empty 
    set if all measures should be imported

  @param yearsFilter
    An umodifiable pointer to an umodifiable tuple of two unsigned integers,
    where if both values are 0, then all years should be imported, otherwise
    they should be treated as a the range of years to be imported

  @return
    void

  @throws 
    std::runtime_error if a parsing error occurs (e.g. due to a malformed file)
    std::out_of_range if there are not enough columns in cols
*/
void Areas::populateFromAuthorityByYearCSV(std::istream& is, const BethYw::SourceColumnMapping& cols,
                                           const std::unordered_set<std::string>* const areasFilter,
                                           const std::unordered_set<std::string>* const measuresFilter,
                                           const std::tuple<unsigned int, unsigned int>* const yearsFilter) {

    if (cols.size() != 3) {
        throw std::out_of_range("Not enough columns in cols mapping!");
    }

    const std::string& measureCode = cols.at(BethYw::SourceColumn::SINGLE_MEASURE_CODE);
    const std::string& measureLabel = cols.at(BethYw::SourceColumn::SINGLE_MEASURE_NAME);

    if (Areas::isIncludedInFilter(measuresFilter, measureCode, false)) {
        std::string line;
        std::stringstream lineStream;

        //read first row, load it in string stream and load years
        std::getline(is, line);
        if (line.empty()) {
            throw std::runtime_error("CSV file is empty!");
        }

        Areas::removeEndline(line);
        lineStream.str(line);
        lineStream.clear();
        std::vector<unsigned int> years = Areas::getYears(lineStream);

        while (std::getline(is, line)) {
            Areas::removeEndline(line);
            lineStream.str(line);
            lineStream.clear();

            std::string authorityCode;
            std::getline(lineStream, authorityCode, ',');

            if (Areas::isIncludedInFilter(areasFilter, authorityCode, true)) {
                Area newArea = Area(authorityCode);
                Measure newMeasure = Measure(measureCode, measureLabel);

                std::string value;
                for (auto it = years.begin(); it != years.end(); it++) {
                    if (Areas::isInYearRange(yearsFilter, *it)) {
                        std::getline(lineStream, value, ',');
                        if (value.empty()) {
                            throw std::runtime_error("Not enough values for all years in authority by year CSV file!");
                        }

                        if (BethYw::isDouble(value)) {
                            double numericalValue = std::stod(value);
                            newMeasure.setValue(*it, numericalValue);
                        }
                    }
                }

                newArea.setMeasure(measureCode, newMeasure);
                this->setArea(authorityCode, newArea);
            }
        }
    }
}

std::vector<unsigned int> Areas::getYears(std::stringstream& lineStream) {
    std::vector<unsigned int> years;
    std::string buffer;

    //throw away first element, it is just the heading for the authority code
    std::getline(lineStream, buffer, ',');

    while (std::getline(lineStream, buffer, ',')) {
        if (BethYw::isInt(buffer)) {
            int year = std::stoi(buffer);

            if (BethYw::is4DigitInt(year)) {
                years.push_back(year);

            } else {
                throw std::runtime_error(std::string("Year is not a 4 digit int") + std::to_string(year));
            }
        } else {
            throw std::runtime_error(std::string("Can not be parsed as year :") + buffer);
        }
    }

    return years;
}

void Areas::removeEndline(std::string& str) {
    char lastChar = *str.rbegin();
    if(lastChar == '\r' || lastChar == '\n') {
        str.pop_back();
    }
}

/*
  Parse data from an standard input stream `is`, that has data of a particular
  `type`, and with a given column mapping in `cols`.

  This function should look at the `type` and hand off to one of the three 
  functions populate………() functions.

  The function must check if the stream is in working order and has content.

  @param is
    The input stream from InputSource

  @param type
    A value from the BethYw::SourceDataType enum which states the underlying
    data file structure

  @param cols
    A map of the enum BethyYw::SourceColumnMapping (see datasets.h) to strings
    that give the column header in the CSV file

  @return
    void

  @throws 
    std::runtime_error if a parsing error occurs (e.g. due to a malformed file),
    the stream is not open/valid/has any contents, or an unexpected type
    is passed in.
    std::out_of_range if there are not enough columns in cols
*/
void Areas::populate(std::istream& is, const BethYw::SourceDataType& type,
                     const BethYw::SourceColumnMapping& cols) noexcept(false) {
    if (!is.good()) {
        throw std::runtime_error("Invalid input stream!");
    }

    if (type == BethYw::AuthorityCodeCSV) {
        populateFromAuthorityCodeCSV(is, cols);
    } else if (type == BethYw::WelshStatsJSON) {
        const std::unordered_set<std::string> emptySet;
        const std::tuple<unsigned int, unsigned int> emptyRange = std::make_tuple(0, 0);

        populateFromWelshStatsJSON(is, cols, &emptySet, &emptySet, &emptyRange);
    } else if (type == BethYw::AuthorityByYearCSV) {
        populateFromAuthorityByYearCSV(is, cols, nullptr, nullptr, nullptr);
    } else {
        throw std::runtime_error("Areas::populate: Unexpected data type");
    }
}

/*
  Parse data from an standard input stream, that is of a particular type,
  and with a given column mapping, filtering for specific areas, measures,
  and years, and fill the container.

  This function should look at the `type` and hand off to one of the three 
  functions you've implemented above.

  The function must check if the stream is in working order and has content.

  This overloaded function includes pointers to the three filters for areas,
  measures, and years.

  @param is
    The input stream from InputSource

  @param type
    A value from the BethYw::SourceDataType enum which states the underlying
    data file structure

  @param cols
    A map of the enum BethyYw::SourceColumnMapping (see datasets.h) to strings
    that give the column header in the CSV file

  @param areasFilter
    An umodifiable pointer to set of umodifiable strings for areas to import,
    or an empty set if all areas should be imported

  @param measuresFilter
    An umodifiable pointer to set of umodifiable strings for measures to import,
    or an empty set if all measures should be imported

  @param yearsFilter
    An umodifiable pointer to an umodifiable tuple of two unsigned integers,
    where if both values are 0, then all years should be imported, otherwise
    they should be treated as a the range of years to be imported

  @return
    void

  @throws 
    std::runtime_error if a parsing error occurs (e.g. due to a malformed file),
    the stream is not open/valid/has any contents, or an unexpected type
    is passed in.
    std::out_of_range if there are not enough columns in cols
*/
void Areas::populate(std::istream& is, const BethYw::SourceDataType& type, const BethYw::SourceColumnMapping& cols,
                     const StringFilterSet* const areasFilter, const StringFilterSet* const measuresFilter,
                     const YearFilterTuple* const yearsFilter) {

    if (!is.good()) {
        throw std::runtime_error("Invalid input stream!");
    }

    if (type == BethYw::AuthorityCodeCSV) {
        populateFromAuthorityCodeCSV(is, cols, areasFilter);
    } else if (type == BethYw::WelshStatsJSON) {
        populateFromWelshStatsJSON(is, cols, areasFilter, measuresFilter, yearsFilter);
    } else if (type == BethYw::AuthorityByYearCSV) {
        populateFromAuthorityByYearCSV(is, cols, areasFilter, measuresFilter, yearsFilter);
    } else {
        throw std::runtime_error("Areas::populate: Unexpected data type");
    }
}

/*
  Convert this Areas object, and all its containing Area instances, and
  the Measure instances within those, to values.

  Use the sample JSON library as above to create this. Construct a blank
  JSON object:
    json j;

  Convert this json object to a string:
    j.dump();

  You then need to loop through your areas, measures, and years/values
  adding this data to the JSON object.

  Read the documentation for how to convert your outcome code to JSON:
    https://github.com/nlohmann/json#arbitrary-types-conversions

  An empty JSON is "{}" (without the quotes), which you must return if your
  Areas object is empty.
  
  @return
    std::string of JSON
*/
std::string Areas::toJSON() const {
    if(!this->areas.empty()) {
        json j;
        to_json(j, *this);

        return j.dump();
    } else {
        return "{}";
    }
}

void to_json(json& j, const Areas& areas) {
    j = json(areas.areas);
}

/*
  Overload the << operator to print all of the imported data.

  Output should be formatted like the following to pass the tests. Areas should
  be printed, ordered alphabetically by their local authority code. Measures 
  within each Area should be ordered alphabetically by their codename.

  @param os
    The output stream to write to

  @param areas
    The Areas object to write to the output stream

  @return
    Reference to the output stream
*/
std::ostream& operator<<(std::ostream& stream, const Areas& data) {
    for (auto it = data.areas.begin(); it != data.areas.end(); it++) {
        stream << it->second;
    }

    return stream;
}
