#pragma once

#include<string>
#include<vector>
#include<map>
#include<iostream>
#include <stdint.h>

namespace NeuroUtils {

const int ISENTINEL = 0x7feffeff;
const double DSENTINEL = 483.5748294857202938475;


namespace detail {
    class NeuroParamVal;
}

/* These are the public-facing components of the params library. */

class Params {
public:
  enum Format
  {
    TXT,
    SON
  };

  Params();

  std::string Serialize();
  bool Deserialize(const std::string &serialization_orFileName, bool isfile);
  void Configure(const std::string &s);
  bool ReadStream(std::istream &stream);
  void WriteStream(std::ostream &stream);

  bool ReadSONStream(std::istream &stream);
  bool ReadTxtStream(std::istream &stream);
  void WriteSONStream(std::ostream &stream);
  void WriteTxtStream(std::ostream &stream);

  void SetName(const std::string &n);
  bool Exists(const std::string &key);
  void Clear();
  void Apply(Params &params);

  void SetDouble(const std::string &key, double value);
  void SetInt(const std::string &key, int value);
  void SetBool(const std::string &key, bool value);
  void SetString(const std::string &key, const std::string &value);
  void SetVector(const std::string &key, const std::vector<std::string> &value);

  double GetDouble(const std::string &key, bool exit_on_error);
  int GetInt(const std::string &key, bool exit_on_error);
  int GetBool(const std::string &key, bool exit_on_error);
  std::string GetString(const std::string &key, bool exit_on_error);
  std::vector <std::string> GetVector(const std::string &key, bool exit_on_error);

  std::string name; /* This name is printed out in error messages. */
  std::string errorMessage; /* This message is set whenever an error occurs. */
  Format inputFormat;
  Format outputFormat;
  std::map <std::string, NeuroUtils::detail::NeuroParamVal> paramsMap;

  inline std::map <std::string, NeuroUtils::detail::NeuroParamVal>::iterator GetParam(const std::string &key, int *found, bool exit_on_error);
};

double ParamsGetDouble(const std::string &stem, const std::string &key, bool exit_on_error);
int ParamsGetInt(const std::string &stem, const std::string &key, bool exit_on_error);
int ParamsGetBool(const std::string &stem, const std::string &key, bool exit_on_error);
std::string ParamsGetString(const std::string &stem, const std::string &key, bool exit_on_error);
std::vector <std::string> ParamsGetVector(const std::string &stem, const std::string &key, bool exit_on_error);
Params ParamsGetParams(const std::string &stem);
bool ParamsParseString(const std::string &defaultStem, const std::string &str);
std::string ParamsGetErrorMessage();
bool ParamsParseArgs(const std::string &defaultStem, int argc, char** argv);
void ParamsDumpParams(const std::string &filename);


/* These are the public-facing components of the SON library. */

bool SONParseObject(std::istream &in, std::map <std::string, std::vector <std::string> > &rv);
std::string SONGetErrorMessage();
std::string SONEscapeString(const std::string &s);
std::string SONSerializeString(const std::string &s);


/* This is the public-facing component of the MOA library. */

class MOA {
  public:
    double   Random_Double();                /* Returns a double in the interval [0, 1) */
    double   Random_DoubleI();               /* Returns a double in the interval [0, 1] */
    int      Random_Integer();               /* Returns an integer between 0 and 2^31-1 */
    uint32_t Random_32();                    /* Returns a random 32-bit number */
    uint64_t Random_64();                    /* Returns a random 64-bit number */
    void     Random_128(uint64_t *x);        /* Returns a random 128-bit number */
    uint32_t Random_W(int w, int zero_ok);   /* Returns a random w-bit number. (w <= 32)*/
    void     Fill_Random_Region (void *reg, int size);   /* reg should be aligned to 4 bytes, but
                                                                   size can be anything. */
    /* You can seed the RNG with an unsigned integer Seed().
       The state of the RNG is held in 20 bytes, which you can access with Get_State().
       We also maintain a counter, which is reset when you call Get_State(), and then incremented
       with every call (potentially multiple times).  You can get the counter with Get_Counter().
       Then, you can reset the RNG to a known state with Set_State(), that takes the 20-byte state
       plus a counter, and resets the state to that point. */

    void     Seed(uint32_t seed);            /* Seed the RNG */
    void     Get_State(void *buffer);        /* Copies internal state & resets an internal counter. 
                                                       The buffer should be >= 20 bytes. */
    uint64_t Get_Counter();                  /* Gets the counter.  Duh. */
    void     Set_State(void *buffer, uint64_t counter);  /* Resets the state to a saved place. */

  protected:
    uint32_t X[5];
    uint64_t Counter;
};

}
