// ======================================================================== //
// Copyright 2015 Ingo Wald
//                                                                          //
// Licensed under the Apache License, Version 2.0 (the "License");          //
// you may not use this file except in compliance with the License.         //
// You may obtain a copy of the License at                                  //
//                                                                          //
//     http://www.apache.org/licenses/LICENSE-2.0                           //
//                                                                          //
// Unless required by applicable law or agreed to in writing, software      //
// distributed under the License is distributed on an "AS IS" BASIS,        //
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. //
// See the License for the specific language governing permissions and      //
// limitations under the License.                                           //
// ======================================================================== //

// pbrt
#include "pbrt/Parser.h"
// stl
#include <iostream>
#include <vector>

int main(int ac, char **av)
{
  std::vector<std::string> fileName;
  bool dbg = false;
  for (int i=1;i<ac;i++) {
    const std::string arg = av[i];
    if (arg[0] == '-') {
      if (arg == "-dbg" || arg == "--dbg")
        dbg = true;
      else
        THROW_RUNTIME_ERROR("invalid argument '"+arg+"'");
    } else {
      fileName.push_back(arg);
    }          
  }

  std::cout << "-------------------------------------------------------" << std::endl;
  std::cout << "parsing:";
  for (int i=0;i<fileName.size();i++)
    std::cout << " " << fileName[i];
  std::cout << std::endl;

  plib::pbrt::Parser *parser = new plib::pbrt::Parser(dbg);
  try {
    for (int i=0;i<fileName.size();i++)
      parser->parse(fileName[i]);
    
    std::cout << "==> parsing successful (grammar only for now)" << std::endl;
  } catch (std::runtime_error e) {
    std::cout << "**** ERROR IN PARSING ****" << std::endl << e.what() << std::endl;
    exit(1);
  }
}
