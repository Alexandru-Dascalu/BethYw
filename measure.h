#ifndef MEASURE_H_
#define MEASURE_H_

/*
  +---------------------------------------+
  | BETH YW? WELSH GOVERNMENT DATA PARSER |
  +---------------------------------------+

  AUTHOR: 965337

  This file contains the declaration of the Measure class.
 */

#include <stdexcept>
#include <string>
#include <sstream>
#include <cstdio>
#include <iostream>
#include <map>
#include <algorithm>
#include <math.h>

/*
  The Measure class contains a measure code, label, and a container for readings
  from across a number of years.
*/
class Measure {
private:
  const std::string code;
  std::string label;
  /*Chose map as the container as it has logarithmic complexity for accessing 
   * and inserting elements, and because it is ordered and thus I can iterate 
   * over the years in ascending order automatically.*/
  std::map<int, double> values;

  //private methods I made to help me
  static std::string& toLower(std::string& str);

  //these ones are used to format the string output of the measure object
  static std::string formatYear(const Measure& measure, int year);
  static std::string formatValue(const Measure& measure, double value);
  static std::string formatHeading(const Measure& measure, std::string& heading);

  int getMaxValueWidth() const noexcept;
public:
  Measure(std::string code, const std::string& label);

  const std::string& getCodename() const noexcept;
  const std::string& getLabel() const noexcept;
  void setLabel(const std::string& newLabel) noexcept;

  double getValue(int year) const;
  void setValue(const int& year, const double& value) noexcept;

  int size() const noexcept;
  double getDifference() const noexcept;
  double getDifferenceAsPercentage() const noexcept;
  double getAverage() const noexcept;

  friend std::ostream& operator<<(std::ostream& stream, const Measure& measure);
  friend bool operator==(const Measure& lhs, const Measure& rhs);
  Measure& operator=(const Measure& other);
};

#endif // MEASURE_H_