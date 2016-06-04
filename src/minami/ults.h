#ifndef ULTS_H
#define ULTS_H

#include "Snap.h"

#include <string>
#include <string>
#include <map>

class MNM_Ults
{
public:
  TInt static round(TFlt in);
  TFlt static min(TFlt a, TFlt b);
};


class Chameleon {
    public:
      Chameleon() {};
      explicit Chameleon(const std::string&);
      explicit Chameleon(double);
      explicit Chameleon(const char*);

      Chameleon(const Chameleon&);
      Chameleon& operator=(Chameleon const&);

      Chameleon& operator=(double);
      Chameleon& operator=(std::string const&);

    public:
      operator std::string() const;
      operator double     () const;
    private:
      std::string value_;
};

class ConfigFile {
  std::map<std::string,Chameleon> content_;

public:
  ConfigFile(std::string const& configFile);

  Chameleon const& Value(std::string const& section, std::string const& entry) const;

  Chameleon const& Value(std::string const& section, std::string const& entry, double value);
  Chameleon const& Value(std::string const& section, std::string const& entry, std::string const& value);
};

class MNM_ConfReader
{
public:
  MNM_ConfReader(std::string, std::string);
  ~MNM_ConfReader();

  TInt get_int(std::string);
  std::string get_string(std::string);
  TFlt get_float(std::string);

  /* data */
  ConfigFile *m_configFile;
  std::string m_confKey;
};

#endif