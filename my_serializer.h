#pragma once

#include "tinyxml2.h"
#include <cstddef>
#include <exception>
#include <fstream>
#include <list>
#include <map>
#include <set>
#include <sstream>
#include <string>
#include <type_traits>
#include <vector>

// Simple error class
class MyErr : public std::exception {
  const std::string info;

public:
  explicit MyErr(const std::string& s) : info(s) {}
  const char* what() const noexcept override { return info.c_str(); }
};

namespace BinarySerialize {

class BinarySerializer {
public:
  BinarySerializer(const std::string& file_name)
  {
    // Save mode: open and clear target file in binary mode, create target
    // file if not exist
    file.open(file_name, std::ios::binary | std::ios::out | std::ios::trunc);
  }
  ~BinarySerializer()
  {
    if (file.is_open())
      file.close();
  }

  // Key can be ignored in Binary Serialization

  // Basic types: arithmetic & string
  // Template only accepts arithmatic types
  template <class T, typename = std::enable_if_t<std::is_arithmetic<T>::value>>
  void process(const T& data)
  {
    // Write in data
    T data_copy = data;
    file.write(reinterpret_cast<char*>(&data_copy), sizeof(data));
  }
  // String
  void process(const std::string& data)
  {
    size_t len = data.length(); // Start with lenth
    process(len);               // Write len in file
    file.write(data.data(), len);
  }

  // STL containers
  // Pair
  template <class T1, class T2>
  void process(const std::pair<T1, T2>& data)
  {
    process(data.first);
    process(data.second);
  }
  // Vector
  template <class T>
  void process(const std::vector<T>& data)
  {
    process(data.size()); // Write in the lenth of data
    for (const T& value :
         data) { // Traverse through the vector and save everything
      process(value);
    }
  }
  // List
  template <class T>
  void process(const std::list<T>& data)
  {
    process(data.size()); // Write in the lenth of data
    for (const T& value :
         data) { // Traverse through the list and save everything
      process(value);
    }
  }
  // Set
  template <class T>
  void process(const std::set<T>& data)
  {
    process(data.size()); // Write in the lenth of data
    for (const T& value :
         data) { // Traverse through the set and save everything
      process(value);
    }
  }
  // Map
  template <class T1, class T2>
  void process(const std::map<T1, T2>& data)
  {
    process(data.size()); // Write in the lenth of data
    for (const auto& [key, value] :
         data) { // Traverse through the map and save everything
      process(key);
      process(value);
    }
  }

private:
  std::fstream file; // Target file
};

class BinaryDeserializer {
public:
  BinaryDeserializer(const std::string& file_name)
  {
    // Load mode: open target file and throw execption if failed.
    file.open(file_name, std::ios::binary | std::ios::in);
    if (!file.is_open()) {
      throw MyErr("BinarySerializer: Failed to open target file");
    }
  }
  ~BinaryDeserializer()
  {
    if (file.is_open())
      file.close();
  }

  // Basic types: arithmetic & string
  // Template only accepts arithmatic types
  template <class T, typename = std::enable_if_t<std::is_arithmetic<T>::value>>
  void process(T& data)
  {
    // Read data from file
    file.read(reinterpret_cast<char*>(&data), sizeof(data));
  }
  // String
  void process(std::string& data)
  {
    size_t len;
    process(len); // Read in len
    data.resize(len);
    file.read(data.data(), len);
  }

  // STL containers
  // Pair
  template <class T1, class T2>
  void process(std::pair<T1, T2>& data)
  {
    process(data.first);
    process(data.second);
  }
  // Vector
  template <class T>
  void process(std::vector<T>& data)
  {
    size_t len; // Read in the lenth
    process(len);
    data.clear();
    data.resize(len);       // Clear and resize the vector
    for (T& value : data) { // Traverse through the vector and load everything
      process(value);
    }
  }
  // List
  template <class T>
  void process(std::list<T>& data)
  {
    size_t len; // Read in the lenth
    process(len);
    data.clear();
    data.resize(len);       // Clear and resize the list
    for (T& value : data) { // Traverse through the list and load everything
      process(value);
    }
  }
  // Set
  template <class T>
  void process(std::set<T>& data)
  {
    size_t len; // Read in the lenth
    process(len);
    data.clear();                      // Clear the set
    for (size_t i = 0; i < len; i++) { // Load data one by one
      T value;
      process(value);
      data.insert(value);
    }
  }
  // Map
  template <class T1, class T2>
  void process(std::map<T1, T2>& data)
  {
    size_t len; // Read in the lenth
    process(len);
    data.clear();                      // Clear the map
    for (size_t i = 0; i < len; i++) { // Load data one by one
      T1 key;
      T2 value;
      process(key);
      process(value);
      data[key] = value; // Insert [key, value] into map
    }
  }

private:
  std::fstream file; // Target file
};

// Top functions for serialization & deserialization
template <class T>
void serialize(const T& data, const std::string& file_name)
{
  BinarySerializer processor(file_name);
  processor.process(data);
}

template <class T>
void deserialize(T& data, const std::string& file_name)
{
  BinaryDeserializer processor(file_name);
  processor.process(data);
}

} // namespace BinarySerialize

namespace XMLSerialize {

// Enable all stuff in tinyxml2
using namespace tinyxml2;

// Converter of XML text mode and binary mode
class XMLConverter {
public:
  // Text to Base64
  std::string operator()(const XMLDocument& file) const
  {
    // Get document
    XMLPrinter printer;
    file.Print(&printer);
    // Convert to cstr
    const char* xml_data = printer.CStr();
    size_t xml_size = printer.CStrSize();
    std::vector<unsigned char> raw_data(xml_data, xml_data + xml_size);
    // Base64 encode
    std::string encoded = base64_encode(raw_data);
    return encoded;
  }

  // Base64 to text
  std::string operator()(const std::string& encoded) const
  {
    std::vector<unsigned char> decoded = base64_decode(encoded);
    return std::string(decoded.data(), decoded.data() + decoded.size());
  }

private:
  // Helper function to encode text by base64
  std::string base64_encode(const std::vector<unsigned char>& data) const
  {
    const std::string base64_chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                                     "abcdefghijklmnopqrstuvwxyz"
                                     "0123456789+/";

    std::string encoded;
    int i = 0;
    int j = 0;
    unsigned char char_array_3[3];
    unsigned char char_array_4[4];
    size_t in_len = data.size();

    for (size_t n = 0; n < in_len; n++) {
      char_array_3[i++] = data[n];
      if (i == 3) {
        char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
        char_array_4[1] =
            ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
        char_array_4[2] =
            ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
        char_array_4[3] = char_array_3[2] & 0x3f;

        for (i = 0; i < 4; i++) {
          encoded += base64_chars[char_array_4[i]];
        }
        i = 0;
      }
    }

    if (i > 0) {
      for (j = i; j < 3; j++) {
        char_array_3[j] = '\0';
      }

      char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
      char_array_4[1] =
          ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
      char_array_4[2] =
          ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
      char_array_4[3] = char_array_3[2] & 0x3f;

      for (j = 0; j < i + 1; j++) {
        encoded += base64_chars[char_array_4[j]];
      }

      while (i++ < 3) {
        encoded += '=';
      }
    }

    return encoded;
  }

  // Helper function to decode base64 file
  std::vector<unsigned char>
  base64_decode(const std::string& encoded_string) const
  {
    const std::string base64_chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                                     "abcdefghijklmnopqrstuvwxyz"
                                     "0123456789+/";

    // Check if empty
    if (encoded_string.empty()) {
      return std::vector<unsigned char>();
    }

    // Calculate input lenth (ignore non-base64 characters)
    size_t i = 0;
    size_t j = 0;
    int in_ = 0;
    unsigned char char_array_4[4], char_array_3[3];
    std::vector<unsigned char> ret;

    std::string clean_encoded;
    for (char c : encoded_string) {
      if (isalnum(c) || c == '+' || c == '/' || c == '=') {
        clean_encoded += c;
      }
    }

    // Check if length is a multiple of 4
    if (clean_encoded.length() % 4 != 0) {
      throw MyErr("Invalid Base64 string length.");
    }

    size_t clean_len = clean_encoded.length();

    while (clean_len-- && (clean_encoded[in_] != '=') &&
           (isalnum(clean_encoded[in_]) || (clean_encoded[in_] == '+') ||
            (clean_encoded[in_] == '/'))) {
      char_array_4[i++] = clean_encoded[in_];
      in_++;
      if (i == 4) {
        for (i = 0; i < 4; i++) {
          size_t pos = base64_chars.find(char_array_4[i]);
          if (pos == std::string::npos) {
            throw MyErr("Invalid character in Base64 string.");
          }
          char_array_4[i] = static_cast<unsigned char>(pos);
        }

        char_array_3[0] =
            (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
        char_array_3[1] =
            ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
        char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

        for (i = 0; i < 3; i++) {
          ret.push_back(char_array_3[i]);
        }
        i = 0;
      }
    }

    // Hadle '=' at the end
    if (i > 0) {
      for (j = i; j < 4; j++) {
        char_array_4[j] = 0;
      }

      for (j = 0; j < 4; j++) {
        size_t pos = base64_chars.find(char_array_4[j]);
        if (pos != std::string::npos) {
          char_array_4[j] = static_cast<unsigned char>(pos);
        } else {
          char_array_4[j] = 0;
        }
      }

      char_array_3[0] =
          (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
      char_array_3[1] =
          ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
      char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

      for (j = 0; j < (i - 1); j++) {
        ret.push_back(char_array_3[j]);
      }
    }

    return ret;
  }
};

// Mode for xml file (text/binary)
enum class XMLMode { text, binary };

class XMLSerializer {
public:
  XMLSerializer(const std::string& file_name, XMLMode mode = XMLMode::text)
      : file_name(file_name), mode(mode)
  {
    // Create root node "<serialization></serialization>"
    root_ele = file.NewElement("serialization");
    file.InsertFirstChild(root_ele);
  }
  ~XMLSerializer()
  {
    // Save to file when destructing
    if (mode == XMLMode::text) {
      file.SaveFile(file_name.c_str()); // Use build-in method
    } else {                            // Binary version
      XMLConverter convert;
      std::string encoded = convert(file);
      // Open terget file in binary mode
      std::ofstream fout(file_name, std::ios::binary | std::ios::trunc);
      fout.write(encoded.data(), encoded.size());
      fout.close();
    }
  }

  // Common asset for external call
  template <class T>
  void process(const T& data)
  {
    // Create new <field> node
    cur_ele = file.NewElement("field");
    root_ele->InsertEndChild(cur_ele);
    process(data, cur_ele);
  }

protected:
  // Basic types
  // In the form of single element such as <posName val="3"/>

  // pos: current level of document (parent)
  template <class T, typename = std::enable_if_t<std::is_arithmetic<T>::value>>
  void process(const T& data, XMLElement* pos)
  {
    pos->SetAttribute("val", data); // Set attribute
    // There is only one data under pos because it is a basic type
    // So no collisions will happen
  }
  // String
  void process(const std::string& data, XMLElement* pos)
  {
    pos->SetAttribute("val", data.c_str()); // Same as before
  }

  // STL containers
  // Pair
  template <class T1, class T2>
  void process(const std::pair<T1, T2>& data, XMLElement* pos)
  {
    // Create <pair> node
    XMLElement* pair_ele = file.NewElement("pair");
    pos->InsertEndChild(pair_ele);

    // Create <first> & <second> node under <pair>
    XMLElement* first_ele = file.NewElement("first");
    pair_ele->InsertEndChild(first_ele);
    process(data.first, first_ele); // Process inner data recursively

    XMLElement* second_ele = file.NewElement("second");
    pair_ele->InsertEndChild(second_ele);
    process(data.second, second_ele);
  }
  // Vector
  template <class T>
  void process(const std::vector<T>& data, XMLElement* pos)
  {
    // Create <vector> node
    XMLElement* vec_ele = file.NewElement("vector");
    pos->InsertEndChild(vec_ele);

    // Write the length of vector
    XMLElement* len_ele = file.NewElement("length");
    vec_ele->InsertEndChild(len_ele);
    process(data.size(), len_ele);

    for (const T& value : data) { // Process everything in data one by one
      XMLElement* item_ele = file.NewElement("item");
      vec_ele->InsertEndChild(item_ele);
      process(value, item_ele);
    }
  }
  // List
  template <class T>
  void process(const std::list<T>& data, XMLElement* pos)
  {
    // Create <list> node
    XMLElement* list_ele = file.NewElement("list");
    pos->InsertEndChild(list_ele);

    // Write the length of list
    XMLElement* len_ele = file.NewElement("length");
    list_ele->InsertEndChild(len_ele);
    process(data.size(), len_ele);

    for (const T& value : data) { // Process everything in data one by one
      XMLElement* item_ele = file.NewElement("item");
      list_ele->InsertEndChild(item_ele);
      process(value, item_ele);
    }
  }
  // Set
  template <class T>
  void process(const std::set<T>& data, XMLElement* pos)
  {
    // Create <set> node
    XMLElement* set_ele = file.NewElement("set");
    pos->InsertEndChild(set_ele);

    // Write the length of set
    XMLElement* len_ele = file.NewElement("length");
    set_ele->InsertEndChild(len_ele);
    process(data.size(), len_ele);

    for (const T& value : data) { // Process everything in data one by one
      XMLElement* item_ele = file.NewElement("item");
      set_ele->InsertEndChild(item_ele);
      process(value, item_ele);
    }
  }
  // Map
  template <class T1, class T2>
  void process(const std::map<T1, T2>& data, XMLElement* pos)
  {
    // Create <map> node
    XMLElement* map_ele = file.NewElement("map");
    pos->InsertEndChild(map_ele);

    // Write the length of set
    XMLElement* len_ele = file.NewElement("length");
    map_ele->InsertEndChild(len_ele);
    process(data.size(), len_ele);

    for (const auto& [key, value] : data) {
      XMLElement* item_ele = file.NewElement("item");
      map_ele->InsertEndChild(item_ele);
      // <key> and <value> node for map item
      XMLElement* key_ele = file.NewElement("key");
      item_ele->InsertEndChild(key_ele);
      process(key, key_ele);
      XMLElement* val_ele = file.NewElement("value");
      item_ele->InsertEndChild(val_ele);
      process(value, val_ele);
    }
  }

  XMLDocument file;     // Target file
  XMLElement* root_ele; // Constantly point to root node <serialization>
  XMLElement* cur_ele;  // Current starting position of serialization, used for
                        // user-defined type
  const std::string file_name; // Used when saved to file
  XMLMode mode;
};

class XMLDeserializer {
public:
  XMLDeserializer(const std::string& file_name, XMLMode mode = XMLMode::text)
      : mode(mode)
  {
    // Try load from file
    if (mode == XMLMode::text) {
      // Open target file
      XMLError status = file.LoadFile(file_name.c_str());
      if (status != XML_SUCCESS) { // Failed to load
        throw MyErr("Failed to open target xml file.");
      }
    } else { // Binary version
      // Get raw data
      std::ifstream fin(file_name, std::ios::binary);
      if (!fin.is_open()) {
        throw MyErr("Failed to open target xml file (binary mode).");
      }

      std::stringstream buffer;
      buffer << fin.rdbuf();
      std::string encoded = buffer.str();
      // Convert to text
      XMLConverter convert;
      std::string decoded = convert(encoded);
      // Parse to xml
      file.Parse(decoded.c_str());
    }

    // Process root element
    root_ele = file.FirstChildElement("serialization");
    if (!root_ele) { // Failed to found
      throw MyErr("Failed to found root element <serialization>.");
    }
    // Get first field node
    cur_ele = root_ele->FirstChildElement("field");
    if (!cur_ele) {
      throw MyErr("Element <field> not found in <serialization>.");
    }
  }
  virtual ~XMLDeserializer() {}

  // Common asset for external call
  template <class T>
  void process(T& data)
  {
    if (!cur_ele) {
      throw MyErr("Field not found.");
    }

    process(data, cur_ele);
    cur_ele = cur_ele->NextSiblingElement("field");
  }

protected:
  // Basic types
  // pos: target location (parent node)
  template <class T, typename = std::enable_if_t<std::is_arithmetic<T>::value>>
  void process(T& data, XMLElement* pos)
  {
    const char* val = pos->Attribute("val");
    std::istringstream iss(val);
    double tmp; // Avoid char type being truncated
    iss >> tmp;
    data = static_cast<T>(tmp);
  }
  // String
  void process(std::string& data, XMLElement* pos)
  {
    data.assign(pos->Attribute("val"));
  }

  // STL containers
  // Pair
  template <class T1, class T2>
  void process(std::pair<T1, T2>& data, XMLElement* pos)
  {
    XMLElement* pair_ele = pos->FirstChildElement("pair");
    // Respectively process first and second
    XMLElement* first_ele = pair_ele->FirstChildElement("first");
    process(data.first, first_ele);
    XMLElement* second_ele = pair_ele->FirstChildElement("second");
    process(data.second, second_ele);
  }
  // Vector
  template <class T>
  void process(std::vector<T>& data, XMLElement* pos)
  {
    XMLElement* vec_ele = pos->FirstChildElement("vector");
    size_t len;
    XMLElement* len_ele = vec_ele->FirstChildElement("length");
    process(len, len_ele); // Read in length
    // Re-initialize data
    data.clear();
    // Traversal through all items and load them
    XMLElement* item_ele = vec_ele->FirstChildElement("item");
    while (item_ele != nullptr) {
      T value;
      process(value, item_ele);
      data.push_back(value);
      item_ele = item_ele->NextSiblingElement("item");
    }
  }
  // List
  template <class T>
  void process(std::list<T>& data, XMLElement* pos)
  {
    XMLElement* list_ele = pos->FirstChildElement("list");
    size_t len;
    XMLElement* len_ele = list_ele->FirstChildElement("length");
    process(len, len_ele); // Read in length
    // Re-initialize data
    data.clear();
    // Traversal through all items and load them
    XMLElement* item_ele = list_ele->FirstChildElement("item");
    while (item_ele != nullptr) {
      T value;
      process(value, item_ele);
      data.push_back(value);
      item_ele = item_ele->NextSiblingElement("item");
    }
  }
  // Set
  template <class T>
  void process(std::set<T>& data, XMLElement* pos)
  {
    XMLElement* set_ele = pos->FirstChildElement("set");
    size_t len;
    XMLElement* len_ele = set_ele->FirstChildElement("length");
    process(len, len_ele); // Read in length
    // Re-initialize data
    data.clear();
    // Traversal through all items and load them
    XMLElement* item_ele = set_ele->FirstChildElement("item");
    while (item_ele != nullptr) {
      T value;
      process(value, item_ele);
      data.insert(value); // Insert into set
      item_ele = item_ele->NextSiblingElement("item");
    }
  }
  // Map
  template <class T1, class T2>
  void process(std::map<T1, T2>& data, XMLElement* pos)
  {
    XMLElement* map_ele = pos->FirstChildElement("map");
    size_t len;
    XMLElement* len_ele = map_ele->FirstChildElement("length");
    process(len, len_ele); // Read in length
    // Re-initialize data
    data.clear();
    // Traversal through all items and load them
    XMLElement* item_ele = map_ele->FirstChildElement("item");
    while (item_ele != nullptr) {
      T1 key;
      T2 value;
      process(key, item_ele->FirstChildElement("key"));
      process(value, item_ele->FirstChildElement("value"));
      data[key] = value; // Insert [key, value] into map
      item_ele = item_ele->NextSiblingElement("item");
    }
  }

  XMLDocument file; // Target file
  XMLElement* root_ele;
  XMLElement* cur_ele; // Currently loading <field> node
  XMLMode mode;
};

// Wrapper class for binary version of xml serialization
// Used for user-defined type macro expansion
class XMLSerializerBase64 : public XMLSerializer {
public:
  // Call binary mode of base Ctor
  XMLSerializerBase64(const std::string& file_name)
      : XMLSerializer(file_name, XMLMode::binary)
  {
  }
  // Use the same Dtor of base class by default
};

class XMLDeserializerBase64 : public XMLDeserializer {
public:
  XMLDeserializerBase64(const std::string& file_name)
      : XMLDeserializer(file_name, XMLMode::binary)
  {
  }
};

// Top functions for serialization & deserialization
template <class T>
void serialize_xml(const T& data, const std::string& file_name)
{
  XMLSerializer processor(file_name);
  processor.process(data);
}

template <class T>
void deserialize_xml(T& data, const std::string& file_name)
{
  XMLDeserializer processor(file_name);
  processor.process(data);
}

template <class T>
void serialize_xml_base64(const T& data, const std::string& file_name)
{
  XMLSerializerBase64 processor(file_name);
  processor.process(data);
}

template <class T>
void deserialize_xml_base64(T& data, const std::string& file_name)
{
  XMLDeserializerBase64 processor(file_name);
  processor.process(data);
}

} // namespace XMLSerialize

// Macro for user-defined types
// Implemented by generating specialized functions for user-defined types
#define MY_SERIALIZE(Type, argcnt, ...)                                        \
  namespace BinarySerialize {                                                  \
  void serialize(const Type& data, const std::string& file_name)               \
  {                                                                            \
    BinarySerializer processor(file_name);                                     \
    SERIALIZE_##argcnt(__VA_ARGS__)                                            \
  }                                                                            \
  void deserialize(Type& data, const std::string& file_name)                   \
  {                                                                            \
    BinaryDeserializer processor(file_name);                                   \
    SERIALIZE_##argcnt(__VA_ARGS__)                                            \
  }                                                                            \
  }                                                                            \
  namespace XMLSerialize {                                                     \
  void serialize_xml(const Type& data, const std::string& file_name)           \
  {                                                                            \
    XMLSerializer processor(file_name);                                        \
    SERIALIZE_##argcnt(__VA_ARGS__)                                            \
  }                                                                            \
  void deserialize_xml(Type& data, const std::string& file_name)               \
  {                                                                            \
    XMLDeserializer processor(file_name);                                      \
    SERIALIZE_##argcnt(__VA_ARGS__)                                            \
  }                                                                            \
  void serialize_xml_base64(const Type& data, const std::string& file_name)    \
  {                                                                            \
    XMLSerializerBase64 processor(file_name);                                  \
    SERIALIZE_##argcnt(__VA_ARGS__)                                            \
  }                                                                            \
  void deserialize_xml_base64(Type& data, const std::string& file_name)        \
  {                                                                            \
    XMLDeserializerBase64 processor(file_name);                                \
    SERIALIZE_##argcnt(__VA_ARGS__)                                            \
  }                                                                            \
  }

// Expansion list for types with more than one field
// SERIALIZE_N() processes the first data and calls SERIALIZE_N-1() recursively
// and ends at SERIALIZE_1()
#define SERIALIZE_1(var) processor.process(data.var);
#define SERIALIZE_2(var, ...)                                                  \
  processor.process(data.var);                                                 \
  SERIALIZE_1(__VA_ARGS__)
#define SERIALIZE_3(var, ...)                                                  \
  processor.process(data.var);                                                 \
  SERIALIZE_2(__VA_ARGS__)
#define SERIALIZE_4(var, ...)                                                  \
  processor.process(data.var);                                                 \
  SERIALIZE_3(__VA_ARGS__)
#define SERIALIZE_5(var, ...)                                                  \
  processor.process(data.var);                                                 \
  SERIALIZE_4(__VA_ARGS__)
#define SERIALIZE_6(var, ...)                                                  \
  processor.process(data.var);                                                 \
  SERIALIZE_5(__VA_ARGS__)
#define SERIALIZE_7(var, ...)                                                  \
  processor.process(data.var);                                                 \
  SERIALIZE_6(__VA_ARGS__)
#define SERIALIZE_8(var, ...)                                                  \
  processor.process(data.var);                                                 \
  SERIALIZE_7(__VA_ARGS__)
#define SERIALIZE_9(var, ...)                                                  \
  processor.process(data.var);                                                 \
  SERIALIZE_8(__VA_ARGS__)
#define SERIALIZE_10(var, ...)                                                 \
  processor.process(data.var);                                                 \
  SERIALIZE_9(__VA_ARGS__)
#define SERIALIZE_11(var, ...)                                                 \
  processor.process(data.var);                                                 \
  SERIALIZE_10(__VA_ARGS__)
#define SERIALIZE_12(var, ...)                                                 \
  processor.process(data.var);                                                 \
  SERIALIZE_11(__VA_ARGS__)
#define SERIALIZE_13(var, ...)                                                 \
  processor.process(data.var);                                                 \
  SERIALIZE_12(__VA_ARGS__)
#define SERIALIZE_14(var, ...)                                                 \
  processor.process(data.var);                                                 \
  SERIALIZE_13(__VA_ARGS__)
#define SERIALIZE_15(var, ...)                                                 \
  processor.process(data.var);                                                 \
  SERIALIZE_14(__VA_ARGS__)
#define SERIALIZE_16(var, ...)                                                 \
  processor.process(data.var);                                                 \
  SERIALIZE_15(__VA_ARGS__)

// Can add more if necessary