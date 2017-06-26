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
#ifndef SIMPLEJSON_UTILS_H
#define SIMPLEJSON_UTILS_H

#include <vector>
#include <string>

using std::vector;
using std::string;

namespace simplejson {
	class utils
	{
		public:
			static vector<string> array2vector(char ** array, unsigned int arrayLen);
			static vector<unsigned long long int> array2vector(unsigned long long int * array, unsigned int arrayLen);
			static vector<long long int> array2vector(long long int * array, unsigned int arrayLen);
			static vector<JSONObject *> array2vector(JSONObject ** array, unsigned int arrayLen);
	};

}

#endif // SIMPLEJSON_UTILS_H
