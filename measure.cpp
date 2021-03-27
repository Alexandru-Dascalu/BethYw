/*
  +---------------------------------------+
  | BETH YW? WELSH GOVERNMENT DATA PARSER |
  +---------------------------------------+

  AUTHOR: 965337

  This file contains the implementation of the Measure class. Measure is a
  very simple class that needs to contain a few member variables for its name,
  codename, and a Standard Library container for data. The data you need to 
  store is values, organised by year. I'd recommend storing the values as 
  doubles.
*/

#include <stdexcept>
#include <string>
#include <cstdio>
#include <iostream>
#include <vector>
#include <map>
#include <algorithm>
#include <cmath>

#include "lib_json.hpp"
#include "measure.h"
#include "bethyw.h"

/*
  An alias for the imported JSON parsing library.
*/
using json = nlohmann::json;

/*
  Construct a single Measure, that has values across many years.

  All StatsWales JSON files have a codename for measures. You should convert 
  all codenames to lowercase.

  @param codename
    The codename for the measure.

  @param label
    Human-readable (i.e. nice/explanatory) label for the measure.
*/
Measure::Measure(const std::string& codename, const std::string& label) : code(BethYw::toLower(codename)),
                                                                          label(label) {

}

/*
  Retrieve the code for the Measure. This function should be callable from a 
  constant context and must promise to not modify the state of the instance or 
  throw an exception.

  @return
    The codename for the Measure.
*/
const std::string& Measure::getCodename() const noexcept {
    return code;
}

/*
  Retrieve the human-friendly label for the Measure. This function should be 
  callable from a constant context and must promise to not modify the state of 
  the instance and to not throw an exception.

  @return
    The human-friendly label for the Measure.
*/
const std::string& Measure::getLabel() const noexcept {
    return label;
}

/*
  Change the label for the Measure.

  @param label
    The new label for the Measure.
*/
void Measure::setLabel(const std::string& newLabel) noexcept {
    label = newLabel;
}

/*
  Retrieve a Measure's value for a given year.

  @param key
    The year to find the value for

  @return
    The value stored for the given year

  @throws
    std::out_of_range if year does not exist in Measure with the message
    No value found for year <year>

  @return
    The value.
*/
double Measure::getValue(int year) const {
    try {
        return values.at(year);
    } catch (const std::out_of_range& ex) {
        throw std::out_of_range(std::string("No value found for year ") + std::to_string(year));
    }
}

/*
  Add a particular year's value to the Measure object. If a value already
  exists for the year, replace it.

  @param key
    The year to insert a value at

  @param value
    The value for the given year.
*/
void Measure::setValue(const unsigned int& year, const double& value) noexcept {
    values[year] = value;
}

/*
  Retrieve the number of years data we have for this measure. This function
  should be callable from a constant context and must promise to not change
  the state of the instance or throw an exception.

  @return
    The size of the measure.
*/
int Measure::size() const noexcept {
    return values.size();
}

/*
  Calculate the difference between the first and last year imported. This
  function should be callable from a constant context and must promise to not
  change the state of the instance or throw an exception.

  @return
    The difference/change in value from the first to the last year, or 0 if it
    cannot be calculated.
*/
double Measure::getDifference() const noexcept {
    if (!values.empty()) {
        double firstValue = values.begin()->second;
        double secondValue = values.rbegin()->second;

        return secondValue - firstValue;
    } else {
        return 0;
    }
}

/*
  Calculate the difference between the first and last year imported as a 
  percentage. This function should be callable from a constant context and
  must promise to not change the state of the instance or throw an exception.

  @return
    The difference/change in value from the first to the last year as a decminal
    value, or 0 if it cannot be calculated.
*/
double Measure::getDifferenceAsPercentage() const noexcept {
    if (!values.empty()) {
        double firstValue = values.begin()->second;

        return (getDifference() / firstValue) * 100;
    } else {
        return 0;
    }
}

/*
  Calculate the average/mean value for all the values. This function should be
  callable from a constant context and must promise to not change the state of 
  the instance or throw an exception.

  @return
    The average value for all the years, or 0 if it cannot be calculated.
*/
double Measure::getAverage() const noexcept {
    if (!values.empty()) {
        double sum = 0;

        for (auto it = values.begin(); it != values.end(); it++) {
            sum += it->second;
        }

        double average = sum / values.size();
        return average;
    } else {
        return 0;
    }
}

/*
  Overload the << operator to print all of the Measure's imported data.

  We align the year and value outputs by padding the outputs with spaces,
  i.e. the year and values should be right-aligned to each other so they
  can be read as a table of numerical values.

  Years should be printed in chronological order. Three additional columns
  should be included at the end of the output, corresponding to the average
  value across the years, the difference between the first and last year,
  and the percentage difference between the first and last year.

  If there is no data in this measure, print the name and code, and 
  on the next line print: <no data>

  See the coursework specification for more information.

  @param os
    The output stream to write to

  @param measure
    The Measure to write to the output stream

  @return
    Reference to the output stream
*/
std::ostream& operator<<(std::ostream& stream, const Measure& measure) {
    stream << measure.getLabel() << " (" << measure.getCodename() << ") " << std::endl;

    for (auto it = measure.values.begin(); it != measure.values.end(); it++) {
        stream << Measure::formatYear(it->first, Measure::getValueWidth(it->second));
    }

    /*We get these values now because we need them to calculate the width for the formatted heading.*/
    double average = measure.getAverage();
    double difference = measure.getDifference();
    double differencePercentage = measure.getDifferenceAsPercentage();

    /*I chose to make a variable for the heading string so I could pass it in as
     * a reference to the format function, which is not possible for literal strings.*/
    std::string heading = "Average";
    stream << Measure::formatHeading(heading, Measure::getValueWidth(average));
    heading = "Diff.";
    stream << Measure::formatHeading(heading, Measure::getValueWidth(difference));
    heading = "% Diff.";
    stream << Measure::formatHeading(heading, Measure::getValueWidth(differencePercentage));
    stream << std::endl;

    for (auto it = measure.values.begin(); it != measure.values.end(); it++) {
        stream << Measure::formatValue(it->second, Measure::getValueWidth(it->second));
    }

    stream << Measure::formatValue(average, Measure::getValueWidth(average));
    stream << Measure::formatValue(difference, Measure::getValueWidth(difference));
    stream << Measure::formatValue(differencePercentage, Measure::getValueWidth(differencePercentage));
    stream << std::endl;

    return stream;
}

/*Formats the given year as a left aligned integer.*/
std::string Measure::formatYear(int year, int formatWidth) {
    //add 2 to account for space after and end of string character
    std::vector<char> buffer = std::vector<char>(formatWidth + 2);

    /*using string stream instead of making a stream directly with concatenation as
     * in C++ you can not concatenate ints, doubles and C-style strings with strings
     * and I have to make some explicit conversions.*/
    std::stringstream formatStream;
    formatStream << "%" << formatWidth << "d ";

    if (snprintf(buffer.data(), formatWidth + 2, formatStream.str().c_str(), year) < 0) {

        throw std::ios_base::failure(std::string("Formatting year string failed for ") + std::to_string(year));
    }

    return std::string(buffer.data());
}

/*Formats the given value as a left aligned floating point number with the given width and 6 digits after the
 * decimal point.*/
std::string Measure::formatValue(double value, int formatWidth) {
    //add 2 to account for space after and end of string character
    std::vector<char> buffer = std::vector<char>(formatWidth + 2);

    /*using string stream instead of making a stream directly with concatenation as
     * in C++ you can not concatenate ints, doubles and C-style strings with strings
     * and I have to make some explicit conversions.*/
    std::stringstream formatStream;
    formatStream << "%" << formatWidth << ".6f ";

    if (snprintf(buffer.data(), formatWidth + 2, formatStream.str().c_str(), value) < 0) {

        throw std::ios_base::failure(std::string("Formatting value string failed for ") +
                                     std::to_string(value));
    }

    return std::string(buffer.data());
}

/*Formats the given heading as a left aligned string with the given width.*/
std::string Measure::formatHeading(std::string& heading, int formatWidth) {
    //add 2 to account for space after and end of string character
    std::vector<char> buffer = std::vector<char>(formatWidth + 2);

    /*using string stream instead of making a stream directly with concatenation as
     * in C++ you can not concatenate ints, doubles and C-style strings with strings
     * String width is incremented by to account for space after.*/
    std::stringstream formatStream;
    formatStream << "%" << formatWidth << "s ";

    if (snprintf(buffer.data(), formatWidth + 2, formatStream.str().c_str(), heading.c_str()) < 0) {

        throw std::ios_base::failure("Formatting string failed");
    }

    return std::string(buffer.data());
}

/*Calculates the width of a double. By width, we mean the number of characters needed
 * to print this double value as a real number with 6 digits after the decimal point.*/
int Measure::getValueWidth(double value) noexcept {
    int digitsBeforeDecimalPoint = 0;

    /*If value is 1 or larger, we can use log base 10 to calculate its width.*/
    if (value >= 1) {
        //log base 10 of 10 is 1, so we add 1 to get 2 digits
        digitsBeforeDecimalPoint = log10(value) + 1;

    /*If max is -1 or larger, we can use log base 10 to calculate its width,
     * but after we get its positive counterpart. We will add 1 to the width
     * to account for the minus sign before the number.*/
    } else if (value <= -1) {
        /*here we add 2 to also account for the minus sign*/
        digitsBeforeDecimalPoint = log10(-value) + 2;

    //if max is between -1 and 1, before the decimal point it just has the digit 0
    } else {
        digitsBeforeDecimalPoint = 1;
    }

    //add 7 for decimal point and 6 digits after decimal point
    return digitsBeforeDecimalPoint + 7;
}

/*
  Overload the == operator for two Measure objects. Two Measure objects
  are only equal when their codename, label and data are all equal.

  @param lhs
    A Measure object

  @param rhs
    A second Measure object

  @return
    true if both Measure objects have the same codename, label and data; false
    otherwise
*/
bool operator==(const Measure& lhs, const Measure& rhs) {
    bool equalCodes = lhs.code == rhs.code;
    bool equalLabels = lhs.label == rhs.label;
    bool equalMeasureValues = lhs.values == rhs.values;

    return equalCodes && equalLabels && equalMeasureValues;
}

/*
  Overload the copy assignment operator for two Measure objects. This will ensure that any year-value pair in the given
  measure that is not already in this measure will be copied over, and if both measures have a value for the same year,
  the value from the given measure will be copied to this measure.

  @param other
    A Measure object to be copied into this one.

  @return
    reference to ths measure
*/
Measure& Measure::operator=(const Measure& other) {
    for (auto it = other.values.begin(); it != other.values.end(); it++) {
        values[it->first] = it->second;
    }

    return *this;
}

/*Function that converts this to json and saves it in the given json object. It is specified in the documentation of
 * the nlohmann::json library.*/
void to_json(json& j, const Measure& measure) {
    /*nlohmann json library by default only handles maps properly if the keys can be represented as strings. We will
     * make an identical map to the on in the measure object, but which will have the years as strings*/
    std::map<std::string, double> stringMap;

    for(auto it = measure.values.begin(); it != measure.values.end(); it++) {
        stringMap.insert(std::pair<std::string, double>(std::to_string(it->first), it->second));
    }

    j = json(stringMap);
}

