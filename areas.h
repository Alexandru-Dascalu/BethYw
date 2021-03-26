#ifndef AREAS_H
#define AREAS_H

/*
  +---------------------------------------+
  | BETH YW? WELSH GOVERNMENT DATA PARSER |
  +---------------------------------------+

  AUTHOR: <STUDENT NUMBER>

  This file contains the Areas class, which is responsible for parsing data
  from a standard input stream and converting it into a series of objects:

  Measure       — Represents a single measure for an area, e.g.
   |              population. Contains a human-readable label and a map of
   |              the measure accross a number of years.
   |
   +-> Area       Represents an area. Contains a unique local authority code
        |         used in national statistics, a map of the names of the area 
        |         (i.e. in English and Welsh), and a map of various Measure 
        |         objects.
        |
        +-> Areas A class that contains all Area objects.
 */

#include <iostream>
#include <string>
#include <tuple>
#include <unordered_set>
#include <map>

#include "lib_json.hpp"
#include "datasets.h"
#include "area.h"

/*
  An alias for the imported JSON parsing library.
*/
using json = nlohmann::json;

/*
  An alias for filters based on strings such as categorisations e.g. area,
  and measures.
*/
using StringFilterSet = std::unordered_set<std::string>;

/*
  An alias for a year filter.
*/
using YearFilterTuple = std::tuple<unsigned int, unsigned int>;

/*
  An alias for the data within an Areas object stores Area objects.
*/
using AreasContainer = std::map<std::string, Area>;

/*
  Areas is a class that stores all the data categorised by area. The 
  underlying Standard Library container is customisable using the alias above.

  To understand the functions declared below, read the comments in areas.cpp
  and the coursework worksheet. Briefly: populate() is called by bethyw.cpp to
  populate data inside an Areas instance. This function will hand off the
  specific parsing of code to other functions, based on the value of 
  BethYw::SourceDataType.
*/
class Areas {
private:
    AreasContainer areas;

    //private functions to help with calculations
    static unsigned int parseYear(const std::string& str);

    static bool isIncludedInFilter(const std::unordered_set<std::string>* const filter, const std::string& data,
                                   bool caseSensitive);

    static bool isInYearRange(const std::tuple<unsigned int, unsigned int>* const yearRange, unsigned int year);
    static const json& safeGet(const json& data, const std::string& key);
    static std::vector<unsigned int> getYears(std::stringstream& lineStream);

public:
    Areas();

    void setArea(const std::string& localAuthorityCode, const Area& area) noexcept;

    Area& getArea(const std::string& localAuthorityCode);

    int size() const noexcept;

    void populateFromAuthorityCodeCSV(
            std::istream& is,
            const BethYw::SourceColumnMapping& cols,
            const StringFilterSet* const areas = nullptr)
    noexcept(false);

    void populateFromWelshStatsJSON(std::istream& is, const BethYw::SourceColumnMapping& cols,
                                    const std::unordered_set<std::string>* const areasFilter,
                                    const std::unordered_set<std::string>* const measuresFilter,
                                    const std::tuple<unsigned int, unsigned int>* const yearsFilter);

    void populateFromAuthorityByYearCSV(std::istream& is, const BethYw::SourceColumnMapping& cols,
                                               const std::unordered_set<std::string>* const areasFilter = nullptr,
                                               const std::unordered_set<std::string>* const measuresFilter = nullptr,
                                               const std::tuple<unsigned int, unsigned int>* const yearsFilter = nullptr);

    void populate(
            std::istream& is,
            const BethYw::SourceDataType& type,
            const BethYw::SourceColumnMapping& cols) noexcept(false);

    void populate(
            std::istream& is,
            const BethYw::SourceDataType& type,
            const BethYw::SourceColumnMapping& cols,
            const StringFilterSet* const areasFilter = nullptr,
            const StringFilterSet* const measuresFilter = nullptr,
            const YearFilterTuple* const yearsFilter = nullptr) noexcept(false);

    std::string toJSON() const;

    friend std::ostream& operator<<(std::ostream& stream, const Areas& data);
    friend void to_json(json& j, const Areas& areas);
};

#endif // AREAS_H