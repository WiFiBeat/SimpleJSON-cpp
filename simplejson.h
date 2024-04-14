/*
 *   Copyright 2017 Thomas d'Otreppe de Bouvette <tdotreppe@aircrack-ng.org>
 *
 *   Licensed under the Apache License, Version 2.0 (the "License");
 *   you may not use this file except in compliance with the License.
 *   You may obtain a copy of the License at

 *       http://www.apache.org/licenses/LICENSE-2.0

 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 */
#ifndef SIMPLEJSON_H
#define SIMPLEJSON_H

#include <vector>
#include <string>
#include <sstream>
#include <stdlib.h>
#include <string.h>
#include <sstream>
#include <algorithm>
#include <regex>

using std::vector;
using std::string;
using std::stringstream;

// TODO: Add Parse()
// TODO: Add AddRawJSON(const string & json)
// TODO: Also add something for binary data (either sent as a vector <unsigned char> or unsigned char * + len
//       => converts to base64 (see C++11 version and the next one:
//            https://stackoverflow.com/questions/180947/base64-decode-snippet-in-c )
// Idea for later: Allow to set a path for a value so user doesn't have to create all the different objects.
//                 Relative to the object created.

#define SIMPLEJSON_NUMBER_SET(variable_type) \
		inline void Set(const string & key, variable_type number) \
		{ \
			this->Set(key, std::to_string(number).c_str(), false); \
		}

#define SIMPLEJSON_GENERIC_ADD(variable_type) \
		inline JSONObject * Add(const string & key, variable_type item) \
		{ \
			return this->Add(new JSONObject(key, item)); \
		}

#define SIMPLEJSON_CONSTRUCTOR_AND_ADD(variable_type) \
		JSONObject(const string & key, variable_type item) \
			: _isDocument(false), _string(NULL), _nullValue(false) \
		{ \
			if (key.empty()) { \
				throw string("Empty key"); \
			} \
			this->Set(key, item); \
		} \
		SIMPLEJSON_GENERIC_ADD(variable_type)

// OBJECT_TYPE: Object type (string, JSONObject *, unsigned long long int
#define SIMPLEJSON_SET_ARRAY(OBJECT_TYPE, ADDDOUBLEQUOTES, STREAMSTRING, DO_BEFORE_STREAM) \
			void Set(const string & key, vector<OBJECT_TYPE> array) \
			{ \
				if (array.empty()) { \
				this->Set(key, "[]", false, 2); \
					return; \
				} \
				stringstream ss; \
				ss << '['; \
				bool first = true; \
				for (OBJECT_TYPE item: array) { \
					if (first) { \
						first = false; \
					} else { \
						ss << ','; \
					} \
					DO_BEFORE_STREAM; \
					if (ADDDOUBLEQUOTES) { \
						ss << '"' << STREAMSTRING << '"'; \
					} else { \
						ss << STREAMSTRING; \
					} \
				} \
				ss << ']'; \
				string temp = ss.str(); \
				this->Set(key, temp.c_str(), false, temp.size()); \
			}

namespace simplejson {
	class JSONObject
	{
	private:
		bool _isDocument;
		char * _string;
		bool _nullValue;

		vector<JSONObject *> _items;

		void Set(const string & key, const char * text, bool addDoubleQuotes = true, int textLen = -1)
		{
			// XXX: Would it be faster to do a sprintf?
			if (text == NULL) {
				this->Set(key, "null", false, 4);
				return;
			}
			if (textLen < 0) {
				textLen = strlen(text);
			}
			this->_string = static_cast<char *>(calloc(1, key.size() + textLen + ((addDoubleQuotes) ? 4 : 2) + 1 + 1));
			this->_string[0] = '"';
			// flawfinder: ignore
			memcpy(this->_string + 1, key.c_str(), key.size());
			unsigned int pos = 1 + key.size();
			this->_string[pos++] = '"';
			this->_string[pos++] = ':';
			if (addDoubleQuotes) {
				this->_string[pos++] = '"';
				// flawfinder: ignore
				memcpy(this->_string + pos, text, textLen);
				this->_string[pos + textLen] = '"';
			} else {
				// flawfinder: ignore
				memcpy(this->_string + pos, text, textLen);
				
			}
		}

		inline void Set(const string & key, bool bit)
		{
			this->Set(key, (bit) ? "true" : "false", false, (bit) ? 4 : 5);
		}

		inline void Set(const string & key)
		{
			this->Set(key, NULL, false, -1);
		}

		JSONObject * Add(JSONObject * object)
		{
			if (this->_isDocument == false) {
				throw string("Cannot add JSON node to a non-document");
			}
			if (object == NULL) {
				throw string("NULL JSONObject");
			}

			// Reset string (in case it was computed)
			if (this->_string) {
				free(this->_string);
				this->_string = NULL;
			}

			// And add it to the list
			this->_items.push_back(object);

			return this;
		}

		SIMPLEJSON_NUMBER_SET(unsigned long long int)
		SIMPLEJSON_NUMBER_SET(long long int)
		SIMPLEJSON_NUMBER_SET(unsigned int)
		SIMPLEJSON_NUMBER_SET(int)
		SIMPLEJSON_NUMBER_SET(double)
		SIMPLEJSON_NUMBER_SET(float)

		SIMPLEJSON_SET_ARRAY(JSONObject *, false, item->toString(),)
		SIMPLEJSON_SET_ARRAY(string, true, temp, string temp(item);std::regex_replace(temp, std::regex("\\\""), "\\\""););
		SIMPLEJSON_SET_ARRAY(unsigned long long int, false, item,)
		SIMPLEJSON_SET_ARRAY(long long int, false, item,)
		SIMPLEJSON_SET_ARRAY(unsigned int, false, item,)
		SIMPLEJSON_SET_ARRAY(int, false, item,)
		SIMPLEJSON_SET_ARRAY(double, false, item,)
		SIMPLEJSON_SET_ARRAY(float, false, item,)

	public:
		~JSONObject()
		{
			free(this->_string);
			for (JSONObject * item: this->_items) {
				delete item;
			}
		}

		JSONObject() : _isDocument(true), _string(NULL), _nullValue(false) { }

		JSONObject(const string & key, JSONObject * object) : _isDocument(false), _string(NULL), _nullValue(false)
		{
			if (key.empty()) {
				throw string("Empty key");
			}
			if (object) {
				string temp("");
				if (object->_isDocument) {
					temp = object->toString();
					delete object;
					object = NULL; // Not necessary but just to be sure
				} else {
					JSONObject * doc = new JSONObject();
					doc->Add(object);
					temp = doc->toString();
					delete doc;
				}
				this->Set(key, temp.c_str(), false, temp.size());
			} else {
				this->Set(key);
			}
		}

		JSONObject(const string & key) : _isDocument(false), _string(NULL), _nullValue(true)
		{
			if (key.empty()) {
				throw string("Empty key");
			}

			this->Set(key);
		}

		JSONObject(const string & key, const string & text) : _isDocument(false), _string(NULL), _nullValue(false)
		{
			if (key.empty()) {
				throw string("Empty key");
			}

			this->Set(key, text.c_str(), true, text.size());
		}

		SIMPLEJSON_CONSTRUCTOR_AND_ADD(const char *)
		SIMPLEJSON_CONSTRUCTOR_AND_ADD(double)
		SIMPLEJSON_CONSTRUCTOR_AND_ADD(float)
		SIMPLEJSON_CONSTRUCTOR_AND_ADD(unsigned long long int)
		SIMPLEJSON_CONSTRUCTOR_AND_ADD(long long int)
		SIMPLEJSON_CONSTRUCTOR_AND_ADD(unsigned int)
		SIMPLEJSON_CONSTRUCTOR_AND_ADD(int)
		SIMPLEJSON_CONSTRUCTOR_AND_ADD(bool)
		SIMPLEJSON_CONSTRUCTOR_AND_ADD(vector<string>)
		SIMPLEJSON_CONSTRUCTOR_AND_ADD(vector<JSONObject *>)
		SIMPLEJSON_CONSTRUCTOR_AND_ADD(vector<unsigned long long int>)
		SIMPLEJSON_CONSTRUCTOR_AND_ADD(vector<long long int>)
		SIMPLEJSON_CONSTRUCTOR_AND_ADD(vector<unsigned int>)
		SIMPLEJSON_CONSTRUCTOR_AND_ADD(vector<int>)
		SIMPLEJSON_CONSTRUCTOR_AND_ADD(vector<double>)
		SIMPLEJSON_CONSTRUCTOR_AND_ADD(vector<float>)


		inline bool empty()
		{
			return (this->_isDocument && this->_items.empty())
				|| (!this->_isDocument && this->_nullValue);
		}
		
		inline bool isDocument()
		{
			return this->_isDocument;
		}

		string toString()
		{
			if (this->_string == NULL) {
				stringstream ss;

				if (this->_isDocument) {
					ss << '{';
					bool first = true;
					for (JSONObject * obj: this->_items) {
						if (first) {
							first = false;
						} else {
							ss << ',';
						}
						ss << obj->toString();
					}
					ss << '}';
				}

				// Copy string
				string temp = ss.str();
				this->_string = static_cast<char *>(calloc(1, temp.size() + 1));
				if (this->_string == NULL) {
					throw string("Failed allocating memory to generate JSON document");
				}
				// flawfinder: ignore
				memcpy(this->_string, temp.c_str(), temp.size());
			}

			return this->_string;
		}
		
		inline JSONObject * Add(const string & key)
		{
			return this->Add(new JSONObject(key));
		}
		
		SIMPLEJSON_GENERIC_ADD(const string &)
		SIMPLEJSON_GENERIC_ADD(JSONObject *)
	};
}
#endif // SIMPLEJSON_H
