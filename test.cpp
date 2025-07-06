#include "my_serializer.h"
#include <cmath>
#include <iostream>
#include <list>
#include <map>
#include <set>
#include <string>
#include <vector>

void check(bool flag, const std::string& info = "")
{
  static int cnt = 0;
  std::cout << "Test#" << ++cnt << " " << (flag ? "Passed" : "Failed");
  if (info.size() > 0) {
    std::cout << ": " + info;
  }
  std::cout << std::endl;
}

// User-defined types
struct UserDefinedType {
  int idx;
  std::string name;
  std::vector<double> data;

  // Helper function for testing
  // Not necessary for serialization
  bool operator==(const UserDefinedType& other) const
  {
    return idx == other.idx && name == other.name && data == other.data;
  }
};

// User need to declare thier type as follows for macro expansion
// MY_SERIALIZE(Typename, Number of fields, field1, field2 ...)
// Support up to 16 fields
MY_SERIALIZE(UserDefinedType, 3, idx, name, data)

int main()
{
  try {
    /* BINARY SERIALIZATION */
    {
      using namespace BinarySerialize;
      std::cout << "Testing: Binary mode..." << std::endl;

      // Simple types
      int i1 = 5;
      serialize(i1, "test.data");
      int i2;
      deserialize(i2, "test.data");
      check(i1 == i2, "int");

      std::string s1 = "avada kedavra";
      serialize(s1, "test.data");
      std::string s2;
      deserialize(s2, "test.data");
      check(s1 == s2, "string");

      // Serialize a rvalue
      serialize(static_cast<float>(1.414), "test.data");
      float f;
      deserialize(f, "test.data");
      check(std::fabs(f - 1.414) < 1e-5, "rval");

      // Simple STL containers
      std::vector<double> vec1 = {3.14, 3.15, 3.16};
      serialize(vec1, "test.data");
      std::vector<double> vec2;
      deserialize(vec2, "test.data");
      check(vec1 == vec2, "vector");

      std::map<std::string, char> m1 = {
          {"ZJU", 'z'}, {"apple", 'a'}, {"banana", 'b'}};
      serialize(m1, "test.data");
      std::map<std::string, char> m2;
      deserialize(m2, "test.data");
      check(m1 == m2, "map");

      // Nested STL containers
      std::vector<std::list<int>> vli1 = {{1, 3, 5}, {2, 4}};
      serialize(vli1, "test.data");
      std::vector<std::list<int>> vli2;
      deserialize(vli2, "test.data");
      check(vli1 == vli2, "vector<list>");

      std::set<std::pair<std::string, double>> sp1 = {
          {"ZJU", 1.1}, {"NJU", 2.2}, {"SJTU", 3.3}};
      serialize(sp1, "test.data");
      std::set<std::pair<std::string, double>> sp2;
      deserialize(sp2, "test.data");
      check(sp1 == sp2, "set<pair>");

      // User-defined type
      UserDefinedType u1 = {233, "YANAMI", {1.2, 2.3, 3.4}};
      serialize(u1, "test.data");
      UserDefinedType u2;
      deserialize(u2, "test.data");
      check(u1 == u2, "User-defined type");
    }

    /* XML SERIALIZATION */
    {
      using namespace XMLSerialize;
      std::cout << "Testing: XML mode..." << std::endl;

      // Simple types
      int i1 = 5;
      serialize_xml(i1, "test.xml");
      int i2;
      deserialize_xml(i2, "test.xml");
      check(i1 == i2, "int");

      std::string s1 = "avada kedavra";
      serialize_xml(s1, "test.xml");
      std::string s2;
      deserialize_xml(s2, "test.xml");
      check(s1 == s2, "string");

      // Serialize a rvalue
      serialize_xml(static_cast<float>(1.414), "test.xml");
      float f;
      deserialize_xml(f, "test.xml");
      check(std::fabs(f - 1.414) < 1e-5, "rval");

      // Simple STL containers
      std::vector<double> vec1 = {3.14, 3.15, 3.16};
      serialize_xml(vec1, "test.xml");
      std::vector<double> vec2;
      deserialize_xml(vec2, "test.xml");
      check(vec1 == vec2, "vector");

      std::map<std::string, char> m1 = {
          {"ZJU", 'z'}, {"apple", 'a'}, {"banana", 'b'}};
      serialize_xml(m1, "test.xml");
      std::map<std::string, char> m2;
      deserialize_xml(m2, "test.xml");
      check(m1 == m2, "map");

      // Nested STL containers
      std::vector<std::list<int>> vli1 = {{1, 3, 5}, {2, 4}};
      serialize_xml(vli1, "test.xml");
      std::vector<std::list<int>> vli2;
      deserialize_xml(vli2, "test.xml");
      check(vli1 == vli2, "vector<list>");

      std::set<std::pair<std::string, double>> sp1 = {
          {"ZJU", 1.1}, {"NJU", 2.2}, {"SJTU", 3.3}};
      serialize_xml(sp1, "test.xml");
      std::set<std::pair<std::string, double>> sp2;
      deserialize_xml(sp2, "test.xml");
      check(sp1 == sp2, "set<pair>");

      // User-defined type
      UserDefinedType u1 = {233, "YANAMI", {1.2, 2.3, 3.4}};
      serialize_xml(u1, "test.xml");
      UserDefinedType u2;
      deserialize_xml(u2, "test.xml");
      check(u1 == u2, "User-defined type");
    }

    /* XML SERIALIZATION BINARY VERSION */
    {
      using namespace XMLSerialize;
      std::cout << "Testing: XML mode ( binary version )..." << std::endl;

      // Simple types
      int i1 = 5;
      serialize_xml_base64(i1, "test.bxml");
      int i2;
      deserialize_xml_base64(i2, "test.bxml");
      check(i1 == i2, "int");

      std::string s1 = "avada kedavra";
      serialize_xml_base64(s1, "test.bxml");
      std::string s2;
      deserialize_xml_base64(s2, "test.bxml");
      check(s1 == s2, "string");

      // Serialize a rvalue
      serialize_xml_base64(static_cast<float>(1.414), "test.bxml");
      float f;
      deserialize_xml_base64(f, "test.bxml");
      check(std::fabs(f - 1.414) < 1e-5, "rval");

      // Simple STL containers
      std::vector<double> vec1 = {3.14, 3.15, 3.16};
      serialize_xml_base64(vec1, "test.bxml");
      std::vector<double> vec2;
      deserialize_xml_base64(vec2, "test.bxml");
      check(vec1 == vec2, "vector");

      std::map<std::string, char> m1 = {
          {"ZJU", 'z'}, {"apple", 'a'}, {"banana", 'b'}};
      serialize_xml_base64(m1, "test.bxml");
      std::map<std::string, char> m2;
      deserialize_xml_base64(m2, "test.bxml");
      check(m1 == m2, "map");

      // Nested STL containers
      std::vector<std::list<int>> vli1 = {{1, 3, 5}, {2, 4}};
      serialize_xml_base64(vli1, "test.bxml");
      std::vector<std::list<int>> vli2;
      deserialize_xml_base64(vli2, "test.bxml");
      check(vli1 == vli2, "vector<list>");

      std::set<std::pair<std::string, double>> sp1 = {
          {"ZJU", 1.1}, {"NJU", 2.2}, {"SJTU", 3.3}};
      serialize_xml_base64(sp1, "test.bxml");
      std::set<std::pair<std::string, double>> sp2;
      deserialize_xml_base64(sp2, "test.bxml");
      check(sp1 == sp2, "set<pair>");

      // User-defined type
      UserDefinedType u1 = {233, "YANAMI", {1.2, 2.3, 3.4}};
      serialize_xml_base64(u1, "test.bxml");
      UserDefinedType u2;
      deserialize_xml_base64(u2, "test.bxml");
      check(u1 == u2, "User-defined type");
    }
  } catch (MyErr& err) {
    std::cout << "Error: " << err.what() << std::endl;
  } catch (...) {
    std::cout << "Unknown error." << std::endl;
  }
}