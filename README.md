# Description

This is a header-only library to easily build a JSON. And this means, charset is limited.

It isn't meant to parse JSON (yet).

Came to life after dealing with RapidJSON weirdness and memory allocation issues reported by valgrind.

# What does header-only mean?

It means there is no library to link against and the library doesn't have any code to compile. Just drop it in your project and you're done. All the code in the header and it will be compiled when including the header file in your project. Another advantage is that there is no need to cross-compile.

# Example

```
#include <simplejson-cpp/simplejson.h>

// Creating document
JSONObject * main = new JSONObject();

// Adding regular objects (or variables)
string val = "my value";
main->Add("mykey", val);
main->Add("otherkey", "something");
main->Add("number", 1232);
main->Add("othernumber", 51.84);

// Adding another object to it
JSONObject * sub = new JSONObject();
sub->Add("item", 1);
main->Add(“subobject”, sub);
// ‘sub’ pointer now belongs to main, don’t delete it

// Adding array to 'main'
vector<string> vec;
vec.push_back("w");
vec.push_back("a");

main->Add(“wa”, vec);

// Add an array of JSONObject to the main document
vector<JSONObject *> sub2;

JSONObject * obj1 = new JSONObject();
obj1->Add("something", "else");

JSONObject * obj2 = new JSONObject(); // Leave this object empty

sub2.push_back(obj1);
sub2.push_back(obj2);

main->Add("objects", sub2);

// Export to string
string json = object->toString();
```

# Future

- Build tests
- Improve performance and memory usage
