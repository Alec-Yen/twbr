#pragma once

#include "universal.h"
#include "SON.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <pthread.h>
#include <sys/time.h>
#include <unistd.h>
#include <sys/param.h>
#include <sys/types.h>
#include <vector>
#include <map>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

/* ------------------------------------------------------------
   This header defines a Params class that can be used to store and read parameters
   of differing types.

   It also provides a global interface to access read parameters set in files. 
    - ParamsGetDouble() returns a double.
    - ParamsGetInt() returns an int.
    - ParamsGetBool() returns a bool (0 or 1).
    - ParamsGetString() returns a string.
    - ParamsGetVector() returns a vector of strings.

   Give it a stem (e.g. "eo"), and it will read from five files, in this order:

   - First, it will find the closest parent directory named "neuro",
     and it will look in the "defaults" directory and use stem.txt
   - Next it will look in ./defaults/stem.txt
   - Next $NEURODEFAULTS/stem.txt
   - Next $NEURODEFAULTS/.stemrc
   - And then ./.stemrc
   Finally, it will parse $NEUROPARAMS_stem as if it were parameters file contents.

   If any of these files or environment variables does not exist, then 
   NeuroReadParams() ignores it.  Parameters are specified on one line, 
   as key and values, which are space separated.  You can have blank lines, 
   and comment lines that begin with '#'. If you specify the same key
   twice (in the same or different files), then the vals are overwritten.

   If none of these files are specified, then the calls below will fail if
   (exit_on_error) is true.

   If (exit_on_error) is true and the given key doesn't exist, then an error
   statement will be printed and the program will exit.

   If (exit_on_error) is false, then if the given key doesn't exist, then
   the procedure will return with:

      - -1 if a bool was requested.
      - ISENTINEL if an integer was requested.
      - DSENTINEL if a double was requested.
      - Empty string if a string was requested.
      - Empty vector if a vector was requested.

   Regardless of the value of (exit_on_error), if the given key exists and is the
   wrong type, then an error statement will be printed and the program will exit.
 */

namespace NeuroUtils {

/* The internals are in NeuroUtils::detail to avoid variable name conflicts. */

namespace detail {

/* Adds params from the given stream to the given map. */
inline void RPTxtStream(std::istream &stream, std::map <std::string, std::vector <std::string> > &rv)
{
  std::istringstream ss;
  std::vector <std::string> sv;
  std::string key;
  std::string s;
  std::string line;

  while (std::getline(stream, line)) {
    ss.clear();
    ss.str(line);
    if (ss >> key && key[0] != '#') {
      sv.clear();
      while (ss >> s) sv.push_back(s);
      rv[key] = sv;
    }
  }
}


/* Adds params from the given file in the given directory to the given map. 
   Does nothing if the file is not found. */
inline void RPF(const std::string &fn, const std::string &dir, std::map <std::string, std::vector <std::string> > &rv)
{
  std::ifstream f;
  std::string path;

  f.clear();
  path = dir;
  path += "/";
  path += fn;

  f.open(path.c_str());
  if (f.fail()) return;
  if(!SONParseObject(f, rv)) {
    fprintf(stderr, "Error parsing params file (must be in SON format): %s\n", path.c_str());
    fprintf(stderr, "SON error: %s\n", SONGetErrorMessage().c_str());
    exit(1);
  }
  f.close();
}

/* Adds params from the given string to the given map. */
inline void RPString(const std::string &paramsString, std::map <std::string, std::vector <std::string> > &rv)
{
  std::istringstream ss;
  ss.str(paramsString);
  if(!SONParseObject(ss, rv)) {
    fprintf(stderr, "Error parsing params string (must be in SON format): %s\n", paramsString.c_str());
    fprintf(stderr, "SON error: %s\n", SONGetErrorMessage().c_str());
    exit(1);
  }
}

/* Reads all possible params with the given stem. Returns a map of any
   params found for this stem. */
inline std::map <std::string, std::vector <std::string> > NeuroReadParams(const std::string &stem)
{
  char *cwd, *x;
  std::string scwd, st, d, wtxt, en;
  std::map <std::string, std::vector <std::string> > rv;

  /* Put the "." and "rc" onto stem */

  if (stem.size() == 0) return rv;
  if (stem[0] == '.' || stem.find(".txt") != std::string::npos) {
    fprintf(stderr, "ReadParamFile: stem shouldn't start with . or end with .txt");
    fprintf(stderr, "-- the dot & rc will be added, as will the appropriate .txt suffix.\n");
    exit(1);
  }

  st = ".";
  st += stem;
  st += "rc";
  wtxt = stem + ".txt";

  /* Find the "/neuro" parent. I'm using C to keep this efficient. */
  /* Put that directory into d */

  cwd = getcwd(NULL, MAXPATHLEN);
  if (cwd == NULL) { perror("getcwd"); exit(1); }
  if (cwd[0] != '/') { fprintf(stderr, "getcwd() doesn't start with /: %s\n", cwd); exit(1); }

  x = cwd + strlen(cwd);

  d = "";
  while (*cwd != '\0') {
    for (x--; *x != '/'; x--) ;
    if (strcmp(x, "/neuro") == 0) {
      d = cwd;
      *cwd = 0;
    } else {
      *x = '\0';
    }
  }
  free(cwd);

  if (d != "") {
    d += "/defaults";
    RPF(wtxt, d, rv);
  }
  /* Do the other three directories */

  RPF(wtxt, "./defaults", rv);
  x = getenv("NEURODEFAULTS");
  if (x != NULL) RPF(wtxt, x, rv);
  if (x != NULL) RPF(st, x, rv);
  RPF(st, ".", rv);

  /* Do the environment variable params string */
  en = "NEUROPARAMS_" + stem;
  x = getenv(en.c_str());
  if (x != NULL) RPString(x, rv);
  return rv;
}

  class NeuroParamVal {
    public:
      int b;
      int i;
      double d;
      std::string s;
      std::vector <std::string> v;

      int used;
  };
} // End of namespace detail


inline Params::Params() {
  inputFormat = SON;
  outputFormat = SON;
}

inline void Params::SetName(const std::string &n) {
  name = n;
}

inline bool Params::Exists(const std::string &key) {
  return paramsMap.find(key) != paramsMap.end();
}

inline void Params::Clear() {
  paramsMap.clear();
}

inline void Params::Apply(Params &params) {
  std::map <std::string, detail::NeuroParamVal>::iterator pit;
  for (pit = params.paramsMap.begin(); pit != params.paramsMap.end(); pit++) {
    paramsMap[pit->first] = pit->second;
  }
}

inline void Params::SetDouble(const std::string &key, double value) {
  std::ostringstream oss;
  oss << value;
  SetString(key, oss.str());
}

inline void Params::SetInt(const std::string &key, int value) {
  std::ostringstream oss;
  oss << value;
  SetString(key, oss.str());
}

inline void Params::SetBool(const std::string &key, bool value) {
  SetString(key, value ? "TRUE" : "FALSE");
}

inline void Params::SetString(const std::string &key, const std::string &value) {
  SetVector(key, std::vector<std::string>(1, value));
}

inline void Params::SetVector(const std::string &key, const std::vector<std::string> &value) {
  std::map <std::string, detail::NeuroParamVal>::iterator vit;
  int i;

  vit = paramsMap.insert(make_pair(key, detail::NeuroParamVal())).first;

  vit->second.v = value;
  vit->second.s = "";
  for (i = 0; i < (int) value.size(); i++) {
    if (i > 0) vit->second.s += " ";
    vit->second.s += value[i];
  }

  vit->second.i = ISENTINEL;
  vit->second.d = DSENTINEL;
  sscanf(vit->second.s.c_str(), "%d", &(vit->second.i));
  sscanf(vit->second.s.c_str(), "%lg", &(vit->second.d));

  if (vit->second.s == "TRUE") vit->second.b = 1;
  else if (vit->second.s == "FALSE") vit->second.b = 0;
  else vit->second.b = -1;

  vit->second.used = false;
}

/* This looks for the given key, returning an iterator pointing to its 
   location in the paramsMap, or paramsMap.end() if the key is not found. 
   Found is set to 1 if the key is found and set to 0 if not. */
inline std::map <std::string, detail::NeuroParamVal>::iterator 
Params::GetParam(const std::string &key, int *found, bool exit_on_error)
{
  std::map <std::string, detail::NeuroParamVal>::iterator vit;

  vit = paramsMap.find(key);
  *found = (vit != paramsMap.end());
  if (!(*found) && exit_on_error) {
      fprintf(stderr, "ERROR.  Required parameter missing: stem: %s key: %s\n",
              name.c_str(), key.c_str());
      exit(1);
  }
  if (*found) {
    vit->second.used = true;
  }
  return vit;
}

inline double Params::GetDouble(const std::string &key, bool exit_on_error)
{
  std::map <std::string, detail::NeuroParamVal>::iterator vit;
  int fnd;
  double rv;

  vit = GetParam(key, &fnd, exit_on_error);
  if (fnd) rv = vit->second.d; 
  else rv = DSENTINEL;

  if (exit_on_error && rv == DSENTINEL) {
    fprintf(stderr, "ERROR.  Parameter not a double: stem: %s key: %s\n",
            name.c_str(), key.c_str());
    exit(1);
  }
  return rv;
}

inline int Params::GetInt(const std::string &key, bool exit_on_error)
{
  std::map <std::string, detail::NeuroParamVal>::iterator vit;
  int fnd;
  int rv;

  vit = GetParam(key, &fnd, exit_on_error);
  if (fnd) rv = vit->second.i; 
  else rv = ISENTINEL;

  if (exit_on_error && rv == ISENTINEL) {
    fprintf(stderr, "ERROR.  Parameter not an integer: stem: %s key: %s\n",
            name.c_str(), key.c_str());
    exit(1);
  }
  return rv;
}

inline int Params::GetBool(const std::string &key, bool exit_on_error)
{
  std::map <std::string, detail::NeuroParamVal>::iterator vit;
  int fnd;
  int rv;

  vit = GetParam(key, &fnd, exit_on_error);
  if (fnd) rv = vit->second.b;
  else rv = -1;

  if (exit_on_error && rv == -1) {
    fprintf(stderr, "ERROR.  Parameter not TRUE|FALSE: stem: %s key: %s\n",
            name.c_str(), key.c_str());
    exit(1);
  }
  return rv;
}

inline std::string Params::GetString(const std::string &key, bool exit_on_error)
{
  std::map <std::string, detail::NeuroParamVal>::iterator vit;
  int fnd;
  std::string rv;

  vit = GetParam(key, &fnd, exit_on_error);
  if (fnd) rv = vit->second.s;
  return rv;
}

inline std::vector <std::string> Params::GetVector(const std::string &key, bool exit_on_error)
{
  std::vector <std::string> rv;
  std::map <std::string, detail::NeuroParamVal>::iterator vit;
  int fnd;

  vit = GetParam(key, &fnd, exit_on_error);
  if (fnd) rv = vit->second.v;
  return rv;
}

inline std::string Params::Serialize() {
  std::ostringstream ss;
  WriteStream(ss);
  return ss.str();
}

inline void Params::WriteStream(std::ostream &stream) {
  switch(outputFormat) {
    case TXT:
      WriteTxtStream(stream);
      break;
    case SON:
      WriteSONStream(stream);
      break;
  }
}

inline bool Params::Deserialize(const std::string &serialization_orFileName, bool isfile) {
  bool success;
  if (isfile) {
    std::ifstream fin(serialization_orFileName.c_str());
    if (fin.fail()) {
      fprintf(stderr, "Params::Deserialize(): error opening file: %s\n", serialization_orFileName.c_str());
      return false;
    }
    success = ReadStream(fin);
    fin.close();
    return success;
  }
  else {
    std::istringstream ss(serialization_orFileName);
    return ReadStream(ss);
  }
}

inline void Params::Configure(const std::string &s) {
  std::istringstream ss(s);
  if (ReadStream(ss)) return;
  fprintf(stderr, "%s\n", errorMessage.c_str());
  exit(1);
}

inline bool Params::ReadStream(std::istream &stream) {
  switch(inputFormat) {
    case TXT:
      return ReadTxtStream(stream);
    case SON:
      return ReadSONStream(stream);
    default:
      std::stringstream ss;
      ss << "Params::ReadStream error: unknown input format  " << inputFormat << std::endl;
      errorMessage = ss.str();
      return false;
  }
}

inline void Params::WriteSONStream(std::ostream &stream) {
  std::map <std::string, detail::NeuroParamVal>::iterator vit;
  std::size_t i;
  std::string s;
  stream << "{" << std::endl;
  for (vit = paramsMap.begin(); vit != paramsMap.end(); vit++) {
    stream << "  " << SONSerializeString(vit->first) << ": ";
    if (vit->second.v.size() == 1) {
      stream << SONSerializeString(vit->second.s);
    }
    else {
      stream << "[";
      for (i = 0; i < vit->second.v.size(); i++) {
        stream << SONSerializeString(vit->second.v[i]) << ", ";
      }
      stream << "]";
    }
    stream << ", " << std::endl;
  }
  stream << "}" << std::endl;
}

inline bool Params::ReadSONStream(std::istream &stream) {
  std::map<std::string, std::vector <std::string> > values;
  std::map<std::string, std::vector <std::string> >::iterator kit;
  if (!SONParseObject(stream, values)) {
    std::stringstream ss;
    ss << "Params::ReadSONStream parse error: " << detail::son_global::GetErrorMessage();
    errorMessage = ss.str();
    return false;
  }
  for (kit = values.begin(); kit != values.end(); kit++) {
    SetVector(kit->first, kit->second);
  }
  return true;
}

inline void Params::WriteTxtStream(std::ostream &stream) {
  std::map <std::string, detail::NeuroParamVal>::iterator vit;
  for (vit = paramsMap.begin(); vit != paramsMap.end(); vit++) {
    stream << vit->first << " " << vit->second.s << std::endl;
  }
}

/* Lines that start with # are ignored. */
inline bool Params::ReadTxtStream(std::istream &stream) {
  std::istringstream ss;
  std::vector <std::string> val;
  std::string key;
  std::string s;
  std::string line;

  while (std::getline(stream, line)) {
    ss.clear();
    ss.str(line);
    if (ss >> key && key[0] != '#') {
      val.clear();
      while (ss >> s) val.push_back(s);
      SetVector(key, val);
    }
  }
  return true;
}

namespace detail {

class params_global {
public:
  // This is the way to make a global variable in a header-only library in C++98
  static std::string& GetErrorMessage() {
    static std::string msg;
    return msg;
  }
  static std::map <std::string, Params> &GetParamsMap() {
    static std::map <std::string, Params> pmap;
    return pmap;
  }

  static pthread_rwlock_t &InitParamsLock() {
    static pthread_rwlock_t lock;
    if (pthread_rwlock_init(&lock, NULL) != 0) {
      perror("pthread_rwlock_init in params.h");
      exit(1);
    }
    return lock;
  }

  static pthread_rwlock_t* GetParamsLock() {
    /* The lock is only initialized once, although C++98 doesn't guarantee the
      initialization is thread safe. */
    static pthread_rwlock_t& lock = InitParamsLock();
    return &lock;
  }

};

/* Map from stems to their key-value pairs. */
static std::map <std::string, Params> &ParamsMap = params_global::GetParamsMap();

static pthread_rwlock_t *ParamLock = params_global::GetParamsLock();

/* This returns an iterator pointing to the given stem's location in the 
   ParamsMap. If this stem hasn't been read yet, it first reads in the parameters 
   from this stem's parameters files and puts them in the ParamsMap. Whoever
   calls this should own ParamLock as a reader. */
inline std::map <std::string, Params>::iterator GetParams(const std::string &stem)
{
  std::map <std::string, Params>::iterator nit;
  std::map <std::string, std::vector <std::string> > P;
  std::map <std::string, std::vector <std::string> >::iterator pit;

  nit = ParamsMap.find(stem);
  if (nit == ParamsMap.end()) {
    if (pthread_rwlock_unlock(ParamLock) != 0) {
      perror("pthread_rwlock_unlock in params.h");
      exit(1);
    }
    P = NeuroReadParams(stem);
    if (pthread_rwlock_wrlock(ParamLock) != 0) {
      perror("pthread_rwlock_wrlock in params.h");
      exit(1);
    }
    nit = ParamsMap.find(stem);

    // See if another thread already read in these params.
    if (nit != ParamsMap.end()) return nit;

    nit = ParamsMap.insert(make_pair(stem, Params())).first;
    nit->second.SetName(stem);
    for (pit = P.begin(); pit != P.end(); pit++) {
      nit->second.SetVector(pit->first, pit->second);
    }
  }
  return nit;
}
} // End of namespace detail


inline double ParamsGetDouble(const std::string &stem, const std::string &key, bool exit_on_error)
{
  std::map <std::string, Params>::iterator nit;
  double rv;
  if (pthread_rwlock_rdlock(detail::ParamLock) != 0) {
    perror("pthread_rwlock_rdlock in params.h");
    exit(1);
  }
  nit = detail::GetParams(stem);

  rv = nit->second.GetDouble(key, false);
  if (rv == DSENTINEL) {
    if (nit->second.Exists(key)) {
      fprintf(stderr, "ERROR: Parameter not a double: stem: %s key: %s\n",
              stem.c_str(), key.c_str());
      exit(1);
    }
    else if (exit_on_error) {
      fprintf(stderr, "ERROR: Required parameter missing: stem: %s key: %s\n",
              stem.c_str(), key.c_str());
      exit(1);
    }
  }
  if (pthread_rwlock_unlock(detail::ParamLock) != 0) {
    perror("pthread_rwlock_unlock in params.h");
    exit(1);
  }
  return rv;
}

inline int ParamsGetInt(const std::string &stem, const std::string &key, bool exit_on_error)
{
  std::map <std::string, Params>::iterator nit;
  int rv;
  if (pthread_rwlock_rdlock(detail::ParamLock) != 0) {
    perror("pthread_rwlock_rdlock in params.h");
    exit(1);
  }
  nit = detail::GetParams(stem);
  rv = nit->second.GetInt(key, false);
  if (rv == ISENTINEL) {
    if (nit->second.Exists(key)) {
      fprintf(stderr, "ERROR: Parameter not an integer: stem: %s key: %s\n",
              stem.c_str(), key.c_str());
      exit(1);
    }
    else if (exit_on_error) {
      fprintf(stderr, "ERROR: Required parameter missing: stem: %s key: %s\n",
              stem.c_str(), key.c_str());
      exit(1);
    }
  }
  if (pthread_rwlock_unlock(detail::ParamLock) != 0) {
    perror("pthread_rwlock_unlock in params.h");
    exit(1);
  }
  return rv;
}

inline int ParamsGetBool(const std::string &stem, const std::string &key, bool exit_on_error)
{
  std::map <std::string, Params>::iterator nit;
  int rv;
  if (pthread_rwlock_rdlock(detail::ParamLock) != 0) {
    perror("pthread_rwlock_rdlock in params.h");
    exit(1);
  }
  nit = detail::GetParams(stem);

  rv = nit->second.GetBool(key, false);
  if (rv == -1) {
    if (nit->second.Exists(key)) {
      fprintf(stderr, "ERROR: Parameter not TRUE|FALSE: stem: %s key: %s\n",
              stem.c_str(), key.c_str());
      exit(1);
    }
    else if (exit_on_error) {
      fprintf(stderr, "ERROR: Required parameter missing: stem: %s key: %s\n",
              stem.c_str(), key.c_str());
      exit(1);
    }
  }
  if (pthread_rwlock_unlock(detail::ParamLock) != 0) {
    perror("pthread_rwlock_unlock in params.h");
    exit(1);
  }
  return rv;
}

inline std::string ParamsGetString(const std::string &stem, const std::string &key, bool exit_on_error)
{
  std::map <std::string, Params>::iterator nit;
  std::string rv;
  if (pthread_rwlock_rdlock(detail::ParamLock) != 0) {
    perror("pthread_rwlock_rdlock in params.h");
    exit(1);
  }
  nit = detail::GetParams(stem);

  rv = nit->second.GetString(key, false);
  if (rv == "") {
    if (!nit->second.Exists(key) && exit_on_error) {
      fprintf(stderr, "ERROR: Required parameter missing: stem: %s key: %s\n",
              stem.c_str(), key.c_str());
      exit(1);
    }
  }
  if (pthread_rwlock_unlock(detail::ParamLock) != 0) {
    perror("pthread_rwlock_unlock in params.h");
    exit(1);
  }
  return rv;
}

inline std::vector <std::string> ParamsGetVector(const std::string &stem, const std::string &key, bool exit_on_error)
{
  std::map <std::string, Params>::iterator nit;
  std::vector <std::string> rv;
  if (pthread_rwlock_rdlock(detail::ParamLock) != 0) {
    perror("pthread_rwlock_rdlock in params.h");
    exit(1);
  }
  nit = detail::GetParams(stem);

  rv = nit->second.GetVector(key, false);
  if (rv.size() == 0) {
    if (!nit->second.Exists(key) && exit_on_error) {
      fprintf(stderr, "ERROR: Required parameter missing: stem: %s key: %s\n",
              stem.c_str(), key.c_str());
      exit(1);
    }
  }
  if (pthread_rwlock_unlock(detail::ParamLock) != 0) {
    perror("pthread_rwlock_unlock in params.h");
    exit(1);
  }
  return rv;
}

inline Params ParamsGetParams(const std::string &stem)
{
  std::map <std::string, Params>::iterator nit;
  if (pthread_rwlock_rdlock(detail::ParamLock) != 0) {
    perror("pthread_rwlock_rdlock in params.h");
    exit(1);
  }
  nit = detail::GetParams(stem);
  Params params = nit->second;
  if (pthread_rwlock_unlock(detail::ParamLock) != 0) {
    perror("pthread_rwlock_unlock in params.h");
    exit(1);
  }
  return params;
}

inline void ParamsDumpParams(const std::string &filename)
{
  std::map <std::string, Params>::iterator pit;
  std::map <std::string, detail::NeuroParamVal>::iterator pvit;
  FILE *pfile;

  pfile = fopen(filename.c_str(), "w");

  if (pthread_rwlock_rdlock(detail::ParamLock) != 0) {
    perror("pthread_rwlock_rdlock in params.h");
    exit(1);
  }

  for (pit = detail::ParamsMap.begin(); pit != detail::ParamsMap.end(); pit++) {
    for (pvit = pit->second.paramsMap.begin(); pvit != pit->second.paramsMap.end(); pvit++) {
      if (pvit->second.used == true) {
        fprintf(pfile, "%s.txt %s: %s\n", pit->first.c_str(), pvit->first.c_str(), pvit->second.s.c_str());
      }
    }
  }

  if (pthread_rwlock_unlock(detail::ParamLock) != 0) {
    perror("pthread_rwlock_unlock in params.h");
    exit(1);
  }

  fclose(pfile);
}

inline bool ParamsParseString(const std::string &defaultStem, const std::string &str) {
  std::stringstream in;
  std::string key;
  int err;
  int c;
  std::map <std::string, Params>::iterator nit;
  std::map <std::string, std::vector <std::string> > defaultStemValues;
  std::map <std::string, std::vector <std::string> > values;
  std::map <std::string, std::vector <std::string> >::iterator kit;

  in << str;

  in >> std::ws;
  if (in.eof()) return true;

  while(1) {
    // Read key
    key.clear();
    err = detail::SONReadKeyToken(in, key);
    if (err == -1) {
      if (key.size() == 0) break;
      detail::params_global::GetErrorMessage() = "Reached end of input before finishing reading key '" + key + "'";
      return false;
    }
    if (err != 1) {
        std::stringstream ss;
        ss << "Error reading key: " << detail::son_global::GetErrorMessage();
        detail::params_global::GetErrorMessage() = ss.str();
        return false;
    }

    // See if next value is an object or a normal value
    in >> std::ws;
    c = in.peek();
    if (c == '{') {
      // The key is a stem and the value is a serialized object
      values.clear();
      if (!SONParseObject(in, values)) {
        std::stringstream ss;
        ss << "Error reading object value: " << detail::son_global::GetErrorMessage();
        detail::params_global::GetErrorMessage() = ss.str();
        return false;
      }

      if (pthread_rwlock_wrlock(detail::ParamLock) != 0) {
        perror("pthread_rwlock_wrlock in params.h");
        exit(1);
      }
      nit = detail::GetParams(key);
      for (kit = values.begin(); kit != values.end(); kit++) {
        nit->second.SetVector(kit->first, kit->second);
      }
      if (pthread_rwlock_unlock(detail::ParamLock) != 0) {
        perror("pthread_rwlock_unlock in params.h");
        exit(1);
      }

      // Skip the next non-whitespace character if it's a comma
      in >> std::ws;
      if (in.peek() == ',') in.get();
    }
    else {
      // Read value
      if (!detail::SONReadValueToken(in, defaultStemValues[key])) {
          std::stringstream ss;
          ss << "Error reading value for key \"" << key << "\" : " << detail::son_global::GetErrorMessage();
          detail::params_global::GetErrorMessage() = ss.str();
          return false;
      }

      // Skip till right after the comma or EOF
      in >> std::ws;
      c = in.get();
      if (c == EOF) break;
      if (c != ',') {
          std::stringstream ss;
          ss << "Unexpected character after value for key \"" << key << "\" : " << (char) c << " (" << c << ")";
          detail::params_global::GetErrorMessage() = ss.str();
          return false;
      }
    }
  }
  if (defaultStemValues.size() != 0) {
    if (defaultStem == "") {
      detail::params_global::GetErrorMessage() = "There are orphaned key-value pairs (and no default stem)";
      return false;
    }

    if (pthread_rwlock_wrlock(detail::ParamLock) != 0) {
      perror("pthread_rwlock_wrlock in params.h");
      exit(1);
    }
    nit = detail::GetParams(defaultStem);
    for (kit = defaultStemValues.begin(); kit != defaultStemValues.end(); kit++) {
      nit->second.SetVector(kit->first, kit->second);
    }
    if (pthread_rwlock_unlock(detail::ParamLock) != 0) {
      perror("pthread_rwlock_unlock in params.h");
      exit(1);
    }
  }
  return true;
}

inline std::string ParamsGetErrorMessage() {
  return detail::params_global::GetErrorMessage();
}

inline bool ParamsParseArgs(const std::string &defaultStem, int argc, char** argv) {
  int i;
  std::string s;

  if (argc <= 1) {
    return true;
  }
  s = argv[1];
  for (i = 2; i < argc; i++) {
    s.push_back(' ');
    s += argv[i];
  }
  if (!ParamsParseString(defaultStem, s)) {
    fprintf(stderr, "Error parsing argc/argv: %s\n", ParamsGetErrorMessage().c_str());
    exit(1);
  }
  return true;
}

}
