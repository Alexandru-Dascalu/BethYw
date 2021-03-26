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
Measure::Measure(const std::string& codename, const std::string& label) : code(BethYw::toLower(codename)), label(label) {

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
  if(!values.empty()) {
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
  if(!values.empty()) {
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
  if(!values.empty()) {
    double sum = 0;

    for(auto it = values.begin(); it != values.end(); it++) {
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
  should be included at the end of the output, correspodning to the average
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
  stream << measure.getLabel() << " (" << measure.getCodename() << ")" << std::endl;

  for(auto it = measure.values.begin(); it != measure.values.end(); it++) {
    stream << Measure::formatYear(measure, it->first); 
  }

  /*I chose to make a variable for the heading string so I could pass it in as
   * a reference to the format function, which is not possible for literal strings.*/
  std::string heading = "Average";
  stream << Measure::formatHeading(measure, heading);
  heading = "Diff.";
  stream << Measure::formatHeading(measure, heading);
  heading = "% Diff.";
  stream << Measure::formatHeading(measure, heading);
  stream << std::endl;

  for(auto it = measure.values.begin(); it != measure.values.end(); it++) {
    stream << Measure::formatValue(measure, it->second);
  }

  stream << Measure::formatValue(measure, measure.getAverage());
  stream << Measure::formatValue(measure, measure.getDifference());
  stream << Measure::formatValue(measure, measure.getDifferenceAsPercentage());
  stream << std::endl;

  return stream;
}

std::string Measure::formatYear(const Measure& measure, int year) {
  /*width is width before decimal point + 6 digits after decimal point + 1 for 
   * the decimal points*/
  int formattedYearWidth = measure.getMaxValueWidth() + 7;
  //add 2 to account for space after and end of string character
  std::vector<char> buffer = std::vector<char>(formattedYearWidth + 2);

  /*using string stream instead of making a stream directly with concatenation as 
   * in C++ you can not concatenate ints, doubles and C-style strings with strings
   * and I have to make some explicit conversions.*/
  std::stringstream formatStream;
  formatStream << "%" << formattedYearWidth << "d ";

  if(snprintf(buffer.data(), formattedYearWidth + 2, formatStream.str().c_str(), 
      year) < 0) {

    throw std::ios_base::failure(std::string("Formatting year string failed for ") + std::to_string(year));
  }

  return std::string(buffer.data());
}

std::string Measure::formatValue(const Measure& measure, double value) {
  /*width is width before decimal point + 6 digits after decimal point + 1 for 
   * the decimal points*/
  int formattedValueWidth = measure.getMaxValueWidth() + 7;
  //add 2 to account for space after and end of string character
  std::vector<char> buffer = std::vector<char>(formattedValueWidth + 2);

  /*using string stream instead of making a stream directly with concatenation as 
   * in C++ you can not concatenate ints, doubles and C-style strings with strings
   * and I have to make some explicit conversions.*/
  std::stringstream formatStream;
  formatStream << "%" << formattedValueWidth << ".6f ";

  if(snprintf(buffer.data(), formattedValueWidth + 2, formatStream.str().c_str(), 
      value) < 0) {

    throw std::ios_base::failure(std::string("Formatting value string failed for ") +
      std::to_string(value));
  }

  return std::string(buffer.data()); 
}

std::string Measure::formatHeading(const Measure& measure, std::string& heading) {
  /*width is width before decimal point + 6 digits after decimal point + 1 for 
   * the decimal points*/
  int formattedStringWidth = measure.getMaxValueWidth() + 7;
  //add 2 to account for space after and end of string character
  std::vector<char> buffer = std::vector<char>(formattedStringWidth + 2);
  
  if(snprintf(buffer.data(), formattedStringWidth + 2, "%s ", heading.c_str()) < 0) {

    throw std::ios_base::failure("Formatting string failed");
  }

  return std::string(buffer.data());
}

/*Calculates the width of the maximum value stored by this measure. By width, 
  we mean the number of digits before the decimal point.

  @return
    Width of maximum value of this measure, as an int. If the measure 
    contains no values, it will return 0.
*/
int Measure::getMaxValueWidth() const noexcept {
  /*If the measure has values, find the maximum one and calculate its width.*/
  if(!values.empty()) {
    double max = values.begin()->second;

    for(auto it = values.begin(); it != values.end(); it++) {
      if(max < it->second) {
        max = it->second;
      }
    }

    /*If max is 1 or larger, we can use log base 10 to calculate its width.*/
    if(max >= 1) {
      //log base 10 of 10 is 1, so we add 1 to get 2 digits 
      int numDigitsInMax = log10(max) + 1;
      return numDigitsInMax;
    /*If max is -1 or larger, we can use log base 10 to calculate its width,
     * but after we get its positive counterpart. We will add 1 to the width
     * to account for the minus sign before the number.*/
    } else if(max <= -1) {
      int numDigitsInMax = log10(max) + 1;
      return numDigitsInMax + 1;
    //if max is between -1 and 1, before the decimal point it just has the digit 0
    } else {
      return 1;
    }
  //if measure has no values, there is no width to calculate.
  } else {
    return 0;
  }
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
  for(auto it = other.values.begin(); it != other.values.end(); it++) {
    values[it->first] = it->second;
  }

  return *this;
}

void to_json(json& j, const Measure& measure) {
    j = json{{measure.values}};
}

