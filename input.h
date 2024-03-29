#ifndef INPUT_H_
#define INPUT_H_

/*
  +---------------------------------------+
  | BETH YW? WELSH GOVERNMENT DATA PARSER |
  +---------------------------------------+

  AUTHOR: 965337

  This file contains declarations for the input source handlers. There are
  two classes: InputSource and InputFile. InputSource is abstract (i.e. it
  contains a pure virtual function). InputFile is a concrete derivation of
  InputSource, for input from files.

  Although only one class derives from InputSource, we have implemented our
  code this way to support future expansion of input from different sources
  (e.g. the web).
 */

#include <string>
#include <fstream>

/*
  InputSource is an abstract/purely virtual base class for all input source 
  types. In future versions of our application, we may support multiple input 
  data sources such as files and web pages. Therefore, this virtual class 
  will allow us to mix/match sources as needed. 
*/
class InputSource {
private:
    const std::string source;
protected:
    InputSource(const std::string& source);

public:
    const std::string& getSource() const noexcept;

    virtual std::istream& open() = 0;
};

/*
  Source data that is contained within a file. For now, our application will
  only work with files (and in particular, the files in the datasets directory).
*/
class InputFile : public InputSource {
private:
    std::ifstream fileInputStream;
public:
    InputFile(const std::string& filePath);

    virtual std::istream& open();
};

#endif // INPUT_H_