/*
  +---------------------------------------+
  | BETH YW? WELSH GOVERNMENT DATA PARSER |
  +---------------------------------------+

  AUTHOR: 965337

  This file contains the code responsible for opening and closing file
  streams. The actual handling of the data from that stream is handled
  by the functions in data.cpp. See the header file for additional comments.
 */

#include "input.h"

/*
  Constructor for an InputSource.

  @param source
    A unique identifier for a source (i.e. the location).
*/
InputSource::InputSource(const std::string& source) : source(source) {

}

/*
  Gets the name of the source represented by this object.

  @return
    A non-modifiable value for the source passed into the constructor.
*/
const std::string& InputSource::getSource() const noexcept {
    return source;
}

/*
  Constructor for a file-based source.

  @param path
    The complete path for a file to import.
*/
/* here, if the value of filePath is changed by external code between when super constructor
 is called and file stream is made, what happens?*/
InputFile::InputFile(const std::string& filePath) : InputSource(filePath),
                                                    fileInputStream(std::ifstream(filePath)) {

}

/*
  Open a file stream to the file path retrievable from getSource()
  and return a reference to the stream.

  @return
    A standard input stream reference

  @throws
    std::runtime_error if there is an issue opening the file, with the message:
    InputFile::open: Failed to open file <file name>
*/
std::istream& InputFile::open() {
    if (!fileInputStream.is_open()) {
        throw std::runtime_error(std::string("InputFile::open: Failed to open file ") + getSource());
    }

    return fileInputStream;
}
