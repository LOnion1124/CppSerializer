# CppSerializer

Final project of OOP course. For reference only. An auto serializer/deserializer supporting saving/loading data to/from file in 3 modes (binary, XML, binary XML). STL containers are supported.

## Background description (copied from PTA)

**Serialization**

In computer science, serialization is the process of translating an object state into a format that can be stored/transmitted and reconstructed later.

Binary serialization uses binary encoding to produce a compact result for uses such as storage or socket-based network streams. This mechanism is especially useful if you have to handle big data where the loading/saving time is crucial.

For cases where you want to read/edit the serialized data, e.g., for software configuration, a serialization to text files (XML, JSON, etc.) is a natural choice.

Please refer to Serialization for more details.

**Requirements**

Implement a module to support binary serialization/deserialization:

```cpp
int n0 = 256, n1;
// serialize object n0 to a binary file n.data
serialize(n0, "n.data");
// reconstruct object n1 from the content of n.data
deserialize(n1, "n.data");
// now n0 == n1 should be true.
```

Implement a wrapper module of tinyxml2 to support XML serialization:

```cpp
std::pair<int, double> pair0 = {2, 3.1}, pair1;
// serialize object pair0 to an XML file pair.xml with the name std_pair
serialize_xml(pair0, "std_pair", "pair.xml");
// reconstruct object pair1 from the content of pair.xml
deserialize_xml(pair1, "std_pair", "pair.xml");
// now pair0 == pair1 should be true.
```

The pair.xml would be something like this:

```xml
<serialization>
  <std_pair>
      <first val="2"/>
      <second val="3.1000000000000001"/>
  </std_pair>
</serialization>
```

Every module has its namespace.

Both modules should at least support the serialization of arithmetic types (see std::is_arithmetic), C++ string type (std::string), and STL containers (std::pair, std::vector, std::list, std::set, and std::map).

Both modules should provide a convenient mechanism (by macro, template, etc.) to support the serialization of user-defined types, e.g.,

```cpp
struct UserDefinedType {
  int idx;
  std::string name;
  std::vector<double> data;
};
```

During testing, you should cover all the required types and options.

[Bonus] Use binary-to-text encoding/decoding (base64) to implement a binary mode of XML serialization.

[Bonus] Support the serialization of smart pointers, e.g., std::unique_ptr.

## Dependences

The tinyxml2 part depends on `tinyxml2.h` and `tinyxml2.cpp` from [tinyxml2](https://github.com/leethomason/tinyxml2).
