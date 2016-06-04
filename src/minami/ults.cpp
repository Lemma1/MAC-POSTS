#include "ults.h"

#include <string>
#include <sstream>
#include <stdlib.h>
#include <fstream>

TInt MNM_Ults::round(TFlt in)
{
  TFlt rdNum = TFlt(std::rand()/(1.0*RAND_MAX));
  TFlt floorN = TFlt(TInt(in));
  if((in - floorN) >rdNum)
    return TInt(floorN + 1);
  else
    return TInt(floorN);
}

TFlt MNM_Ults::min(TFlt a, TFlt b)
{
  return a<b? a:b;
}


Chameleon::Chameleon(std::string const& value) 
{
    value_=value;
}

#include <iostream>

Chameleon::Chameleon(const char* c) 
{
    value_=c;
}

Chameleon::Chameleon(double d) 
{
    std::stringstream s;
    s<<d;
    value_=s.str();
}

Chameleon::Chameleon(Chameleon const& other) 
{
    value_=other.value_;
}

Chameleon& Chameleon::operator=(Chameleon const& other) 
{
    value_=other.value_;
    return *this;
}

Chameleon& Chameleon::operator=(double i) 
{
    std::stringstream s;
    s << i;
    value_ = s.str();
    return *this;
}

Chameleon& Chameleon::operator=(std::string const& s) 
{
    value_=s;
    return *this;
}

Chameleon::operator std::string() const 
{
    return value_;
}

Chameleon::operator double() const 
{
    return atof(value_.c_str());
}

std::string trim(std::string const& source, char const* delims = " \t\r\n") {
  std::string result(source);
  std::string::size_type index = result.find_last_not_of(delims);
  if(index != std::string::npos)
    result.erase(++index);

  index = result.find_first_not_of(delims);
  if(index != std::string::npos)
    result.erase(0, index);
  else
    result.erase();
  return result;
}

ConfigFile::ConfigFile(std::string const& configFile) {
  std::ifstream file(configFile.c_str());

  std::string line;
  std::string name;
  std::string value;
  std::string inSection;
  int posEqual;
  while (std::getline(file,line)) {

    if (! line.length()) continue;

    if (line[0] == '#') continue;
    if (line[0] == ';') continue;

    if (line[0] == '[') {
      inSection=trim(line.substr(1,line.find(']')-1));
      continue;
    }

    posEqual=line.find('=');
    name  = trim(line.substr(0,posEqual));
    value = trim(line.substr(posEqual+1));

    content_[inSection+'/'+name]=Chameleon(value);
  }
}

Chameleon const& ConfigFile::Value(std::string const& section, std::string const& entry) const {

  std::map<std::string,Chameleon>::const_iterator ci = content_.find(section + '/' + entry);

  if (ci == content_.end()) throw "does not exist";

  return ci->second;
}

Chameleon const& ConfigFile::Value(std::string const& section, std::string const& entry, double value) {
  try {
    return Value(section, entry);
  } catch(const char *) {
    return content_.insert(std::make_pair(section+'/'+entry, Chameleon(value))).first->second;
  }
}

Chameleon const& ConfigFile::Value(std::string const& section, std::string const& entry, std::string const& value) {
  try {
    return Value(section, entry);
  } catch(const char *) {
    return content_.insert(std::make_pair(section+'/'+entry, Chameleon(value))).first->second;
  }
}

MNM_ConfReader::MNM_ConfReader (std::string filename, std::string confKey) {
  m_configFile = new ConfigFile(filename);
  m_confKey = confKey;
}

MNM_ConfReader::~MNM_ConfReader () {
  if (!m_configFile) {
    delete m_configFile;
  }
}

TInt MNM_ConfReader::get_int (std::string key) {
  int val = m_configFile->Value(m_confKey, key);
  return TInt(val);
}

TFlt MNM_ConfReader::get_float(std::string key)
{
  double val = m_configFile->Value(m_confKey, key);
  return TFlt(val);
}

std::string MNM_ConfReader::get_string(std::string key)
{
  std::string val = m_configFile->Value(m_confKey, key);
  return val;
}