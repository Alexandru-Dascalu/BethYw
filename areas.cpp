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
    if(areas.find(localAuthorityCode) == areas.end()) {
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
        throw std::out_of_range("No area found matching " + localAuthorityCode);
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
void Areas::populateFromAuthorityCodeCSV(std::istream &is, const BethYw::SourceColumnMapping &cols,
    const StringFilterSet* const areasFilter) {

    if(cols.size() != 3) {
        throw std::out_of_range("Not enough columns in cols mapping!");
    }

    std::string line;
    std::stringstream lineStream;

    //read and throw away first line, containing headings
    std::getline(is, line);

    while(std::getline(is, line)) {
        lineStream.str(line);
        lineStream.clear();

        std::string authorityCode;
        std::string englishName;
        std::string welshName;

        std::getline(lineStream, authorityCode, ',');
        std::getline(lineStream, englishName, ',');
        std::getline(lineStream, welshName, ',');

        if(authorityCode.empty() || englishName.empty() || welshName.empty()) {
            throw std::runtime_error("Line does not have three comma separated values!");
        }

        /*We only add the area if we should all areas or if the filter specified this area code. We make sure to
         * put the condition for the null pointer first so we do not dereference it later on.*/
        if(areasFilter == nullptr || areasFilter->empty() || areasFilter->find(authorityCode) != areasFilter->end()) {
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

  Therefore, you need to go through the items in value (in a loop)
  using a JSON library. To help you, I've selected the nlohmann::json
  library that you must use for your coursework. Read up on how to use it here:
  https://github.com/nlohmann/json

  Example of using this library:
    - Reading/parsing in from a stream is very simply using the >> operator:
        json j;
        stream >> j;

    - Looping through parsed JSON is done with a simple for each loop. Inside
      the loop, you can access each using the array syntax, with the key/
      column name, e.g. data["Localauthority_ItemName_ENG"] gives you the
      local authority name:
        for (auto& el : j["value"].items()) {
           auto &data = el.value();
           std::string localAuthorityCode = data["Localauthority_ItemName_ENG"];
           // do stuff here...
        }

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

  @see
    See datasets.h for details of how the variable cols is organised

  @see
    See bethyw.cpp for details of how the variable areasFilter is created

  @example
    InputFile input("data/popu1009.json");
    auto is = input.open();

    auto cols = InputFiles::DATASETS["popden"].COLS;

    auto areasFilter = BethYw::parseAreasArg();
    auto measuresFilter = BethYw::parseMeasuresArg();
    auto yearsFilter = BethYw::parseMeasuresArg();

    Areas data = Areas();
    areas.populateFromWelshStatsJSON(
      is,
      cols,
      &areasFilter,
      &measuresFilter,
      &yearsFilter);
*/
void Areas::populateFromWelshStatsJSON(std::istream &is, const BethYw::SourceColumnMapping & cols,
                                       const std::unordered_set<std::string>* const areasFilter,
                                       const std::unordered_set<std::string>* const measuresFilter,
                                       const std::tuple<unsigned int, unsigned int>* const yearsFilter) {
    json j;
    is >> j;

    bool isTrainDataset = cols == BethYw::InputFiles::TRAINS.COLS;

    for(auto& element: j["value"].items()) {
        auto& data = element.value();
        const std::string& authorityCode = data.at(cols.at(BethYw::SourceColumn::AUTH_CODE));

        if(areasFilter->empty() || areasFilter->find(authorityCode) != areasFilter->end()) {

            /*I wanted to have the measure code be a const reference, and thus it needs to be initialized when it is
             * declared. Therefore, I could only do it with a ternary operator. If the dataset file is the train one,
             * the measure code is a hardcoded value, else it is a value we need to read from the json data.*/
            const std::string& measureCode = isTrainDataset ? BethYw::InputFiles::TRAINS.COLS.at(BethYw::SourceColumn::SINGLE_MEASURE_CODE)
                    : (const std::string&) data.at(cols.at(BethYw::SourceColumn::MEASURE_CODE));

            if(measuresFilter->empty() || measuresFilter->find(measureCode) != measuresFilter->end()) {
                int year = data.at(cols.at(BethYw::SourceColumn::YEAR));

                if(year >= std::get<0>(*yearsFilter) && year <= std::get<1>(*yearsFilter)) {
                    const std::string& areaEngName = data.at(cols.at(BethYw::SourceColumn::AUTH_NAME_ENG));
                    const double value = data.at(cols.at(BethYw::SourceColumn::VALUE));

                    //same as before with the measure code, we need to check if the file is the train dataset
                    const std::string& measureLabel = isTrainDataset ? BethYw::InputFiles::TRAINS.COLS.at(BethYw::SourceColumn::SINGLE_MEASURE_NAME)
                            : (const std::string&) data.at(cols.at(BethYw::SourceColumn::MEASURE_NAME));

                    try {
                        /*We use getArea as it already checks if this object has an area with this code.*/
                        Area& area = this->getArea(authorityCode);

                        /*Same story, since getMeasure throws an exception, we can use it to check if the area has this
                         * measure already.*/
                        try {
                            //in this case, we just need to add/update data for that year
                            Measure& measure = area.getMeasure(measureCode);
                            measure.setValue(year, value);
                        } catch (const std::out_of_range& ex) {
                            //if the area does not have a measure with this code, make new measure and add it
                            Measure newMeasure = Measure(measureCode, measureLabel);
                            newMeasure.setValue(year, value);
                            area.setMeasure(measureCode, newMeasure);
                        }

                    } catch (const std::out_of_range& ex) {
                        /*if out_of_range was thrown, then an area with that code does not already exist, we need to
                         * create it.*/
                        Area newArea = Area(authorityCode);
                        newArea.setName("eng", areaEngName);

                        Measure newMeasure = Measure(measureCode, measureLabel);
                        newMeasure.setValue(year, value);
                        newArea.setMeasure(measureCode, newMeasure);
                    }
                }
            }
        }
    }
}

/*
  TODO: Areas::populateFromAuthorityByYearCSV(is,
                                              cols,
                                              areasFilter,
                                              measuresFilter,
                                              yearFilter)

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

  @see
    See datasets.h for details of how the variable cols is organised

  @see
    See bethyw.cpp for details of how the variable areasFilter is created

  @example
    InputFile input("data/complete-popu1009-pop.csv");
    auto is = input.open();

    auto cols = InputFiles::DATASETS["complete-pop"].COLS;

    auto areasFilter = BethYw::parseAreasArg();
    auto yearsFilter = BethYw::parseYearsArg();

    Areas data = Areas();
    areas.populateFromAuthorityCodeCSV(is, cols, &areasFilter, &yearsFilter);

  @throws 
    std::runtime_error if a parsing error occurs (e.g. due to a malformed file)
    std::out_of_range if there are not enough columns in cols
*/


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
void Areas::populate(std::istream& is,const BethYw::SourceDataType& type,
                     const BethYw::SourceColumnMapping& cols) noexcept(false) {
  if (type == BethYw::AuthorityCodeCSV) {
      populateFromAuthorityCodeCSV(is, cols);
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

  @see
    See datasets.h for details of the values variable type can have

  @see
    See datasets.h for details of how the variable cols is organised

  @see
    See bethyw.cpp for details of how the variables areasFilter, measuresFilter,
    and yearsFilter are created

  @example
    InputFile input("data/popu1009.json");
    auto is = input.open();

    auto cols = InputFiles::DATASETS["popden"].COLS;

    auto areasFilter = BethYw::parseAreasArg();
    auto measuresFilter = BethYw::parseMeasuresArg();
    auto yearsFilter = BethYw::parseMeasuresArg();

    Areas data = Areas();
    areas.populate(
      is,
      DataType::WelshStatsJSON,
      cols,
      &areasFilter,
      &measuresFilter,
      &yearsFilter);
*/
void Areas::populate(std::istream &is, const BethYw::SourceDataType &type, const BethYw::SourceColumnMapping &cols,
    const StringFilterSet* const areasFilter, const StringFilterSet* const measuresFilter,
    const YearFilterTuple* const yearsFilter) {

    if (type == BethYw::AuthorityCodeCSV) {
        populateFromAuthorityCodeCSV(is, cols, areasFilter);
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

  @example
    InputFile input("data/popu1009.json");
    auto is = input.open();

    auto cols = InputFiles::DATASETS["popden"].COLS;

    auto areasFilter = BethYw::parseAreasArg();
    auto measuresFilter = BethYw::parseMeasuresArg();
    auto yearsFilter = BethYw::parseMeasuresArg();

    Areas data = Areas();
    std::cout << data.toJSON();
*/
std::string Areas::toJSON() const {
  json j;
  to_json(j, *this);

  return j.dump();
}

void to_json(json& j, const Areas& areas) {
    j = json{ areas.areas };
}

/*
  Overload the << operator to print all of the imported data.

  Output should be formatted like the following to pass the tests. Areas should
  be printed, ordered alphabetically by their local authority code. Measures 
  within each Area should be ordered alphabetically by their codename.

  With real data, your output should start like this for the command
  bethyw --dir <dir> -p popden -y 1991-1993 (truncated for readability):

    Isle of Anglesey / Ynys Môn (W06000001)
    Land area (area) 
          1991       1992       1993    Average    Diff.  % Diff. 
    711.680100 711.680100 711.680100 711.680100 0.000000 0.000000 

    Population density (dens) 
         1991      1992      1993   Average    Diff.  % Diff. 
    97.126504 97.486216 98.038430 97.550383 0.911926 0.938905 

    Population (pop) 
            1991         1992         1993      Average      Diff.  % Diff. 
    69123.000000 69379.000000 69772.000000 69424.666667 649.000000 0.938906 


    Gwynedd / Gwynedd (W06000002)
    Land area (Area)
    ...

  @param os
    The output stream to write to

  @param areas
    The Areas object to write to the output stream

  @return
    Reference to the output stream
*/
std::ostream& operator<<(std::ostream& stream, const Areas& data) {
    for(auto it = data.areas.begin(); it != data.areas.end(); it++) {
        stream << it->second << std::endl;
    }

    return stream;
}
