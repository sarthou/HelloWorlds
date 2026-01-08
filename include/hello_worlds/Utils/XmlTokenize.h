#ifndef HWS_XMLTOKENIZE_H
#define HWS_XMLTOKENIZE_H

#include <cassert>
#include <cmath>
#include <cstdlib>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <string>
#include <vector>

namespace hws {

  struct TokenFloatArray_t
  {
    std::vector<float>& values;
    TokenFloatArray_t(std::vector<float>& float_array) : values(float_array)
    {}

    void add(const char* token)
    {
      float v = (float)atof(token);
      values.push_back(v);
    }
  };
  struct TokenIntArray_t
  {
    std::vector<int>& values;
    TokenIntArray_t(std::vector<int>& array) : values(array)
    {}

    void add(const char* token)
    {
      int v = atoi(token);
      values.push_back(v);
    }
  };

  template<typename AddToken>
  void tokenize(const std::string& str, AddToken& token_adder, const std::string& delimiters = " \n")
  {
    std::string::size_type pos = 0;
    std::string::size_type last_pos = 0;
    while(true)
    {
      pos = str.find_first_of(delimiters, last_pos);
      if(pos == std::string::npos)
      {
        pos = str.length();
        if(pos != last_pos)
          token_adder.add(str.data() + last_pos);

        break;
      }
      else if(pos != last_pos)
        token_adder.add(str.data() + last_pos);

      last_pos = pos + 1;
    }
  }

  glm::vec3 getVector3FromXmlText(const char* text);
  glm::vec4 getVector4FromXmlText(const char* text);

} // namespace hws

#endif // HWS_XMLTOKENIZE_H