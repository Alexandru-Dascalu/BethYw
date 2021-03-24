#ifndef AREA_H_
#define AREA_H_

/*
  +---------------------------------------+
  | BETH YW? WELSH GOVERNMENT DATA PARSER |
  +---------------------------------------+

  AUTHOR: 965337

  This file contains the Area class declaration. Area objects contain all the
  Measure objects for a given local area, along with names for that area and a
  unique authority code.
 */

#include <string>
#include <unordered_map>
#include <map>
#include <iostream>
#include "measure.h"

/*
  An Area object consists of a unique authority code, a container for names
  for the area in any number of different languages, and a container for the
  Measures objects.
*/
class Area {
private:
  const std::string authorityCode;
  std::unordered_map<std::string, std::string> names;
  std::map<std::string, Measure> measures;

  //private function to help me
  bool hasName(const std::string& langCode) const;
  static std::string& toLower(std::string& str);

public:
  Area(const std::string& localAuthorityCode);

  //public method required by the cw
  const std::string& getLocalAuthorityCode() const noexcept;
  const std::string& getName(const std::string& langCode) const;
  void setName(const std::string& lang, const std::string& name);

  Measure& getMeasure(const std::string& key);
  void setMeasure(const std::string& codename, const Measure& measure) noexcept;

  int size() const noexcept;

  Area& operator=(const Area& other);
  friend std::ostream& operator<<(std::ostream& stream, const Area& area);
  friend bool operator==(const Area& lhs, const Area& rhs); 
};

#endif // AREA_H_