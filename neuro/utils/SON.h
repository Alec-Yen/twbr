#pragma once
#include "utils/universal.h"
#include <cstdio>
#include <map>
#include <sstream>
#include <iostream>
#include <string>
#include <vector>

namespace NeuroUtils {

namespace detail {

class son_global {
public:
  // This is the way to make a global variable in a header-only library in C++98
  static std::string& GetErrorMessage() {
    static std::string msg;
    return msg;
  }
};

class FilterCommentsStreambuf : public std::streambuf
{
protected:
  std::istream& myOwner;
  std::streambuf* mySource;
  char myCommentChar;
  char myBuffer;

  int underflow()
  {
    int const eof = std::istream::traits_type::eof();
    int results = mySource->sbumpc();
    if (results == myCommentChar && myBuffer != '\\') {
        // printf("Found comment char\n");
        while (results != eof && results != '\n') {
          // printf("Skipping %c\n", results);
          results = mySource->sbumpc();
        }
    }
    if (results != eof) {
      myBuffer = (char) results;
      setg(&myBuffer, &myBuffer, &myBuffer + 1);
    }
    return results;
  }

public:
  FilterCommentsStreambuf(std::istream& source,
                           char comment = '#')
    : myOwner(source)
    , mySource(source.rdbuf())
    , myCommentChar(comment)
  {
    myOwner.rdbuf(this);
    myBuffer = '\0';
  }
  ~FilterCommentsStreambuf()
  {
    myOwner.rdbuf(mySource);
  }
};

/*
  SONReadKeyToken()
  This reads a key, which can be quoted or not quoted. To help avoid errors,
  an unquoted key is not allowed to contain a comma. When this function
  returns successfuly, it will have read up to and including the colon.
  Return value:
    1 means successful
    0 means syntax error
    -1 means EOF
*/
inline int SONReadKeyToken(std::istream &in, std::string &key) {
  int c;
  char endChar;
  size_t i;

  in >> std::ws;
  c = in.peek();
  if (c == '"') {
    endChar = '"';
    // Skip this character
    in.get();
  }
  else endChar = ':';

  // Parse a string
  while(1) {
    c = in.get();
    switch(c) {
      case '\\': // Escaped character
        c = in.get();
        switch(c) {
          case 'b':
            key.push_back('\b');
            break;
          case 'f':
            key.push_back('\f');
            break;
          case 'n':
            key.push_back('\n');
            break;
          case 'r':
            key.push_back('\r');
            break;
          case 't':
            key.push_back('\t');
            break;
          case '"':
            key.push_back('\"');
            break;
          case '\\':
            key.push_back('\\');
            break;
          case '\'':
            key.push_back('\'');
            break;
          case '#':
            key.push_back('#');
            break;
          default:
            std::stringstream ss;
            ss << "Invalid escaped character: " << (char) c << " (" << c << ")";
            son_global::GetErrorMessage() = ss.str();
            return 0;
        }
        break;
      case EOF:
        son_global::GetErrorMessage() = "Reached EOF before finishing reading key";
        return -1;
      case ',':
        if (endChar != '"') {
          son_global::GetErrorMessage() = "Found ',' in unquoted string key. Key so far is '" + key + ",'. Perhaps you're missing a colon?";
          return false;
        }
        key.push_back((char) c);
        break;
      default:
        // Check for end character
        if (c == endChar) {
          if (endChar == '"') {
            // Get the colon
            in >> std::ws;
            c = in.get();
            if (c == ':') return 1;

            std::stringstream ss;
            ss << "Expected ':' after token. Found '" << (char) c << "' (" << c << ")";
            son_global::GetErrorMessage() = ss.str();
            return 0;
          }
          else {
            // Remove extra whitespace at end
            i = key.find_last_not_of(" \n\t\r");
            if (i == std::string::npos) {
              son_global::GetErrorMessage() = "No non-whitespace characters found";
              return false;
            }
            key.resize(i+1);
            return true;
          }
          return 1;
        }
        key.push_back((char) c);
    }
  }
}

/* SONParseQuotedString()
  String begins and ends with quotation marks 
*/
inline bool SONParseQuotedString(std::istream &in, std::vector <std::string> &value) {
  int c;

  // Jump to quotation mark
  do { c = in.get(); }
  while (c != '"' && c != EOF);

  // Parse a quoted string
  if (value.size() == 0) value.resize(1);
  std::string &s = value[value.size()-1];
  while(1) {
    c = in.get();
    switch(c) {
      case '\\': // Escaped character
        c = in.get();
        switch(c) {
          case 'b':
            s.push_back('\b');
            break;
          case 'f':
            s.push_back('\f');
            break;
          case 'n':
            s.push_back('\n');
            break;
          case 'r':
            s.push_back('\r');
            break;
          case 't':
            s.push_back('\t');
            break;
          case '"':
            s.push_back('\"');
            break;
          case '\\':
            s.push_back('\\');
            break;
          case '\'':
            s.push_back('\'');
            break;
          case '#':
            s.push_back('#');
            break;
          default:
            std::stringstream ss;
            ss << "Invalid escaped character: " << (char) c << " (" << c << ")";
            son_global::GetErrorMessage() = ss.str();
            return false;
        }
        break;
      case EOF:
        son_global::GetErrorMessage() = "Reached EOF before finishing reading string";
        return false;
      case '"':
        return true;
      default:
        s.push_back((char) c);
    }
  }
}

/* SONParseString()
  This string is not quoted. It ends at a comma, ']', '}', or EOF. 
  Whitespace delimits separate values. 
  An unquoted string is not allowed to have ':', '{', or '['.
*/
inline bool SONParseString(std::istream &in, std::vector <std::string> &value) {
  std::string totalVal;
  int c;
  size_t i;

  // Skip starting whitespace
  in >> std::ws;

  // Parse a string
  if (value.size() == 0) value.resize(1);
  std::string &s = value[value.size()-1];
  while(1) {
    c = in.peek();
    if (c == ',' || c == '}' || c == ']' || c == EOF) {
      // Remove extra whitespace at end
      i = s.find_last_not_of(" \n\t\r");
      if (i == std::string::npos) {
        std::stringstream ss;
        ss << "No non-whitespace characters found in value " << value.size();
        son_global::GetErrorMessage() = ss.str();
        return false;
      }
      s.resize(i+1);
      return true;
    }
    in.get();
    // Whitespace delimits new values
    if (isspace(c)) {
      in >> std::ws;
      c = in.peek();
      if (c == ',' || c == '}' || c == ']' || c == EOF) return true;
      value.push_back("");
      return SONParseString(in, value);
    }
    switch(c) {
      case '\\': // Escaped character
        c = in.get();
        switch(c) {
          case 'b':
            s.push_back('\b');
            break;
          case 'f':
            s.push_back('\f');
            break;
          case 'n':
            s.push_back('\n');
            break;
          case 'r':
            s.push_back('\r');
            break;
          case 't':
            s.push_back('\t');
            break;
          case '"':
            s.push_back('\"');
            break;
          case '\\':
            s.push_back('\\');
            break;
          case '\'':
            s.push_back('\'');
            break;
          case '#':
            s.push_back('#');
            break;
          default:
            std::stringstream ss;
            ss << "Invalid escaped character: " << (char) c << " (" << c << ")";
            son_global::GetErrorMessage() = ss.str();
            return false;
        }
        break;
      case ':':
          totalVal = value[0];
          for (i = 1; i < value.size(); i++) totalVal += " " + value[i];
          son_global::GetErrorMessage() = "Found ':' in unquoted string value. Value so far is '"
                                      + totalVal + ":' Perhaps you're missing a comma?";
          return false;
      case '{':
      case '[':
          {
            std::stringstream ss;
            ss << "An unquoted string cannot contain '" << (char) c << "'. Please wrap string in quotation marks (\"). The string is \"" + s + (char) c + "\"";
            son_global::GetErrorMessage() = ss.str();
            return false;
          }
      default:
        s.push_back((char) c);
    }
  }
}
inline bool SONReadValueToken(std::istream &in, std::vector <std::string> &value);

/* SONParseArray()
  Arrays begin with '['. This returns after reading the ending ']'. 
*/
inline bool SONParseArray(std::istream &in, std::vector <std::string> &value) {
  int c;

  // Jump to array-begin character
  do { c = in.get(); }
  while (c != '[' && c != EOF);

  // Check for empty array
  in >> std::ws;
  c = in.peek();
  if (c == ']') {
    in.get();
    return true;
  }

  while(1) {
    SONReadValueToken(in, value);

    // Skip till after the comma
    do { c = in.get(); }
    while (c != ',' && c != ']' && c != EOF);
    if (c == ']') return true;

    // See if next non-whitespace character is a square brace (to allow for a comma after the last element)
    in >> std::ws;
    if (in.peek() == ']') {
      in.get();
      return true;
    }

  }
}

/* SONReadValueToken() reads a value. It returns after reading the end of the 
   value, which is different for each value type:
     - array: ']'
     - quoted string: '"'
     - unquoted string: right before a comma, ']', '}', or EOF
*/
inline bool SONReadValueToken(std::istream &in, std::vector <std::string> &value) {
  in >> std::ws;
  switch(in.peek()) {
    case '"':
      value.push_back("");
      return SONParseQuotedString(in, value);
    case '[':
      return SONParseArray(in, value);
    default:
      value.push_back("");
      return SONParseString(in, value);
  }
}

} // end namespace detail

/*
  This SONParseObject() expects something like 
    { num_epochs: 100, best_net_file_name:"awesomenet.txt" }
  It returns after reading the closing '}'.
*/
inline bool SONParseObject(std::istream &in, std::map <std::string, std::vector <std::string> > &rv) {
  int c;
  char k;
  std::string key;
  std::string value;
  int err;
  
  detail::FilterCommentsStreambuf filter(in);

  in >> k;
  if (in.eof()) {
    detail::son_global::GetErrorMessage() = "Reached EOF before finding '{'";
    return false;
  }
  if (k != '{') {
    std::stringstream ss;
    ss << "Expected '{'. Found '" << k << "' (" << (int) k << ")";
    detail::son_global::GetErrorMessage() = ss.str();
    return false;
  }
  in >> std::ws;
  if (in.peek() == '}') {
    in.get();
    return true;
  }

  while(1) {
    // Read key
    key.clear();
    err = detail::SONReadKeyToken(in, key);
    if (err == -1) {
        detail::son_global::GetErrorMessage() = "Reached EOF before finding '}'";
        return false;
    }
    if (err != 1) {
        std::stringstream ss;
        ss << "Error reading key: " << detail::son_global::GetErrorMessage();
        detail::son_global::GetErrorMessage() = ss.str();
        return false;
    }

    // Overwrite previous values
    rv[key].clear();

    // Read value
    if (!detail::SONReadValueToken(in, rv[key])) {
        std::stringstream ss;
        ss << "Error reading value for key \"" << key << "\" : " << detail::son_global::GetErrorMessage();
        detail::son_global::GetErrorMessage() = ss.str();
        return false;
    }
    // Skip till right after the comma or curly brace
    in >> std::ws;
    c = in.get();
    switch(c) {
      case '}': 
        return true;
      case EOF:
        detail::son_global::GetErrorMessage() = "Reached EOF before finding '}'";
        return false;
      case ',':
        // See if next non-whitespace character is a curly brace (to allow for a comma after the last element)
        in >> std::ws;
        if (in.peek() == '}') {
          in.get();
          return true;
        }
        break;
      default:
        std::stringstream ss;
        ss << "Unexpected character after value for key \"" << key << "\" : " << (char) c << " (" << c << ")";
        detail::son_global::GetErrorMessage() = ss.str();
        return false;
    }
  }
}

inline std::string SONGetErrorMessage() {
  return detail::son_global::GetErrorMessage();
}

inline std::string SONEscapeString(const std::string &s) {
  static const char *escapedCharacters = "\"\\#";
  std::size_t last;
  std::size_t found;
  std::string rv;

  last = 0;
  found = s.find_first_of(escapedCharacters);
  if (found == std::string::npos) return s;

  rv.reserve(s.size());
  do {
    // Copy everything since the last special character and add a slash
    rv.append(s, last, found - last);
    rv.push_back('\\');
    last = found;
    found = s.find_first_of(escapedCharacters, found+1);
  } while (found != std::string::npos);
  rv.append(s, last, found);
  return rv;
}

inline std::string SONSerializeString(const std::string &s) {
  bool addQuotes;
  const std::string quotedChars = ",\t\n\v\f\r:{}[]";
  if (s.size() == 0) return "\"\"";

  /* Add quotes if the value starts or ends with a space, or if it
     has any whitespace more complicated than a single space. */

  addQuotes = isspace(s[0])
    || isspace(s[s.size()-1])
    || s.find_first_of(quotedChars) != std::string::npos
    || (s.find("  ") != std::string::npos);
  if (addQuotes) return '\"' + SONEscapeString(s) + '\"';
  else return SONEscapeString(s);
}

}
