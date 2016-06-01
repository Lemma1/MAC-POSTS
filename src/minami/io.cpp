#include "io.h"

#include <fstream>
#include <iostream>


int MNM_IO::build_node_factory(std::string file_folder, MNM_ConfReader *conf_reader, MNM_Node_Factory *node_factory)
{
  /* find file */
  std::string __node_file_name = file_folder + "/MNM_input_node";
  std::ifstream __node_file;
  __node_file.open(__node_file_name, std::ios::in);

  /* read confid */
  TInt __num_of_node = conf_reader -> get_int("num_of_node");
  std::cout << __num_of_node << "\n";
  TFlt __flow_scalar = conf_reader -> get_float("flow_scalar");

  /* read file */
  std::string __line;
  std::vector<std::string> __words;
  TInt __node_ID;
  std::string __type;

  if (__node_file.is_open())
  {
    std::getline(__node_file,__line); //skip the first line
    for (int i=0; i < __num_of_node; ++i){
      std::getline(__node_file,__line);
      __words = split(__line, ' ');
      if (__words.size() == 2) {
        std::cout << __line << "\n";
        __node_ID = TInt(std::stoi(__words[0]));
        __type = __words[1];
        if (__type == "FWJ"){
          printf("Enter FWJ\n");
          node_factory -> make_node(__node_ID, MNM_TYPE_FWJ, __flow_scalar);
        }
        if (__type =="DMOND"){
          node_factory -> make_node(__node_ID, MNM_TYPE_ORIGIN, __flow_scalar);
        }
        if (__type =="DMDND"){
          node_factory -> make_node(__node_ID, MNM_TYPE_ORIGIN, __flow_scalar);
        }
      }
    }
    __node_file.close();
  }
  return 0;
}


std::vector<std::string> MNM_IO::split(const std::string &text, char sep) 
{
  std::vector<std::string> tokens;
  std::size_t start = 0, end = 0;
  while ((end = text.find(sep, start)) != std::string::npos) {
    tokens.push_back(text.substr(start, end - start));
    start = end + 1;
  }
  tokens.push_back(text.substr(start));
  return tokens;
}