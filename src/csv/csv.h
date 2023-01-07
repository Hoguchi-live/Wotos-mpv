#ifndef _CSV_H_
#define _CSV_H_

#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <cstring>
#include <string>
#include <sstream>
#include <iomanip>
#include <ctime>
#include <locale.h>
#include <time.h>

void read_record();
long int string2timestamp(const std::string &);
#endif
