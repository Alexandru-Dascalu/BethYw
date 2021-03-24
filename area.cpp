/*
  +---------------------------------------+
  | BETH YW? WELSH GOVERNMENT DATA PARSER |
  +---------------------------------------+

  AUTHOR: 965337

  This file contains the implementation for the Area class. Area is a relatively
  simple class that contains a local authority code, a container of names in
  different languages (perhaps stored in an associative container?) and a series
  of Measure objects (also in some form of container).
*/

#include <stdexcept>
#include <string>
#include <iostream>
#include <unordered_map>
#include <map>

#include "lib_json.hpp"
#include "area.h"

/*
  An alias for the imported JSON parsing library.
*/
using json = nlohmann::json;

/*
  Construct an Area with a given local authority code.

  @param localAuthorityCode
    The local authority code of the Area

  @example
    Area("W06000023");
*/
Area::Area(const std::string& localAuthorityCode) : authorityCode(localAuthorityCode), 
  names(std::unordered_map<std::string, std::string>()), 
  measures(std::map<std::string, Measure>()) {

}

/*
  Retrieve the local authority code for this Area. This function should be 
  callable from a constant context and not modify the state of the instance.
  
  @return
    The Area's local authority code
*/
const std::string& Area::getLocalAuthorityCode() const noexcept {
  return authorityCode;
}

/*
  Get a name for the Area in a specific language.  This function should be 
  callable from a constant context and not modify the state of the instance.

  @param lang
    A three-leter language code in ISO 639-3 format, e.g. cym or eng

  @return
    The name for the area in the given language

  @throws
    std::out_of_range if lang does not correspond to a language of a name stored
    inside the Area instance
*/
const std::string& Area::getName(const std::string& langCode) const {
  return names.at(langCode);
}

/*
  Check if area has a name for the given language.

  @param lang
    A three-leter language code in ISO 639-3 format, e.g. cym or eng

  @return
    True if the area has a name for that language code.
*/
bool Area::hasName(const std::string& langCode) const {
  return names.find(langCode) != names.end();
}

/*
  Set a name for the Area in a specific language.

  @param lang
    A three-letter (alphabetical) language code in ISO 639-3 format,
    e.g. cym or eng, which should be converted to lowercase

  @param name
    The name of the Area in `lang`

  @throws
    std::invalid_argument if lang is not a three letter alphabetic code
*/
void Area::setName(const std::string& lang, const std::string& name) {
  if(lang.length() != 3) {
    throw std::invalid_argument("Area::setName: Language code must be three alphabetical letters only");
  }

  bool isAlphabetic = true;
  for(auto it = lang.begin(); it != lang.end(); it++) {
    if(!std::isalpha(*it)) {
      isAlphabetic = false;
    }
  }

  if(!isAlphabetic) {
    throw std::invalid_argument("Area::setName: Language code must be three alphabetical letters only");
  }

  /*param reference could be to a string outside the function we should not 
   *change, therefore we make a copy before we make the string lower case.*/
  std::string lowerCaseCode = lang;
  names[Area::toLower(lowerCaseCode)] = name;
}


/*
  Retrieve a Measure object, given its codename. This function should be case
  insensitive when searching for a measure.

  @param key
    The codename for the measure you want to retrieve

  @return
    A Measure object

  @throws
    std::out_of_range if there is no measure with the given code, throwing
    the message:
    No measure found matching <codename>
*/
Measure& Area::getMeasure(const std::string& key) {
  /*param reference could be to a string outside the function we should not 
   *change, therefore we make a copy before we make the string lower case.*/
  std::string lowerCaseKey = key;
  Area::toLower(lowerCaseKey);

  try {
    return measures.at(key);
  } catch (std::out_of_range& ex) {
    throw std::out_of_range("No measure found matching " + key);
  }
}

std::string& Area::toLower(std::string& str) {
  std::for_each(str.begin(), str.end(), [](char& c) {
        c = ::tolower(c);
  });

  return str;
}

/*
  Add a particular Measure to this Area object. Note that the Measure's
  codename should be converted to lowercase.

  If a Measure already exists with the same codename in this Area, overwrite any
  values contained within the existing Measure with those in the new Measure
  passed into this function. The resulting Measure stored inside the Area
  instance should be a combination of the two Measures instances.

  @param codename
    The codename for the Measure

  @param measure
    The Measure object

  @return
    void
*/
void Area::setMeasure(const std::string& codename, const Measure& measure) noexcept {
  /*param reference could be to a string outside the function we should not 
   *change, therefore we make a copy before we make the string lower case.*/
  std::string lowerCaseName = codename;
  Area::toLower(lowerCaseName);

  if(measures.find(lowerCaseName) == measures.end()) {
    measures.insert(std::pair<std::string, Measure>(lowerCaseName, measure));
  } else {
    measures.find(lowerCaseName)->second = measure;
  }
}

/*
  Retrieve the number of Measures we have for this Area. This function should be 
  callable from a constant context, not modify the state of the instance, and
  must promise not throw an exception.

  @return
    The size of the Area (i.e., the number of Measures)
*/
int Area::size() const noexcept {
  return measures.size();
}

/*
  Overload the stream output operator as a free/global function.

  Output the name of the Area in English and Welsh, followed by the local
  authority code. Then output all the measures for the area (see the coursework
  worksheet for specific formatting).

  If the Area only has only one name, output this. If the area has no names,
  output the name "Unnamed".

  Measures should be ordered by their Measure codename. If there are no measures
  output the line "<no measures>" after you have output the area names.

  See the coursework specification for more examples.

  @param os
    The output stream to write to

  @param area
    Area to write to the output stream

  @return
    Reference to the output stream
*/
std::ostream& operator<<(std::ostream& stream, const Area& area) {
  bool hasEnglishName = area.hasName("eng");
  bool hasWelshName = area.hasName("cym");

  if(hasEnglishName && hasWelshName) {
    stream << area.getName("eng") << " / " << area.getName("cym");
  } else if (hasEnglishName) {
    stream << area.getName("eng");
  } else if (hasWelshName) {
    stream << area.getName("cym");
  } else {
    stream << "Unnamed";
  }

  stream << " (" << area.getLocalAuthorityCode() << ")" << std::endl;

  for(auto it = area.measures.begin(); it != area.measures.end(); it++) {
    stream << it->second << std::endl;
  }

  return stream;
}

/*
  Overload the == operator for two Area objects as a global/free function. Two
  Area objects are only equal when their local authority code, all names, and
  all data are equal.

  @param lhs
    An Area object

  @param rhs
    A second Area object

  @return
    true if both Area instanes have the same local authority code, names
    and data; false otherwise.
*/
bool operator==(const Area& lhs, const Area& rhs) {
  if(lhs.getLocalAuthorityCode() == rhs.getLocalAuthorityCode()) {
    if(lhs.names == rhs.names) {
      if(lhs.measures == rhs.measures) {
        return true;
      }
    }
  }

  return false;
}

/*
  Overload the copy assignment operator for two Area objects. This will ensure that any name or measure in the given
  area that is not already in this area will be copied over, and if both area have a name for the same language code or
  a measure with the same codename, the value from the given area will be copied to this area.

  @param other
    An area object to be copied into this one.

  @return
    reference to ths area
*/
Area& Area::operator=(const Area& other) {
    for(auto it = other.names.begin(); it != other.names.end(); it++) {
        this->names[it->first] = it->second;
    }

    for(auto it = other.measures.begin(); it != other.measures.end(); it++) {
        /*We need to check if each measure of other is already in this area. This is because Measure has no constructor
         * with no arguments, and therefore it can not be called if we just did this->measure[it->first] = it->second.
         * When the measure is not already in this area, we need to manually make a pair and insert it.*/
        if(this->measures.find(it->first) != this->measures.end()) {
            this->measures.at(it->first) = it->second;
        } else {
            this->measures.insert(std::make_pair(it->first, it->second));
        }
    }

    return *this;
}

void to_json(json& j, const Area& area) {
    j = json{{ {"names", area.names}, {"measures", area.measures} }};
}