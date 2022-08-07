#include <regex>
#include <list>
#include <iostream>
#include <string>

#include "Data.h"

// Lookup table 
struct param_lut {
    std::string patterns;
    int id;
};
const std::list<std::string> patterns_v {"([0-9]+)p([0-9]+)v?"};
const std::list<std::string> patterns_t {"(m)([0-9]+)c","([0-9]+)c"};

int xlut_size = 8;
param_lut xlut[] {
    {"CMax", 0},
    {"RCMax", 1},
    {"CMin", 2},
    {"RCMin", 3},
    {"cmax", 0},
    {"rcmax", 1},
    {"cmin", 2},
    {"rcmin", 3},
};

int ttlut_size = 2;
param_lut ttlut[] {
    {"_max_", 0},
    {"_min_", 1}
};

int cclut_size = 3;
param_lut cclut[] {
    {"ff", 0},
    {"tt", 1},
    {"ss", 2}
};

Data::Data()
{
    // constructor
}
Data::~Data()
{
    // destructor
}

// setters and getters
void Data::add(std::string line)
{
    _data = line;
    int match_flag = 0;
    // _p_v
    for ( std::list<std::string>::const_iterator p = patterns_v.begin() ; p != patterns_v.end() ; p++)
    {
        std::cout << "line: " << line << std::endl;
        std::regex regexp(*p);
        std::smatch m;
        if (std::regex_search(line, m, regexp)) {
            auto w = std::stoi(m[1].str());
            auto d = std::stoi(m[2].str());
            _p_v = (float) d/100;
            match_flag++;
            break;
        }
        
    }
    // _p_t
    for ( std::list<std::string>::const_iterator p = patterns_t.begin() ; p != patterns_t.end() ; p++)
    {
        std::regex regexp(*p);
        std::smatch m;
        if (std::regex_search(line, m, regexp)) {
            if (m[1].str() == "m") {
                auto d = std::stoi(m[2].str());
                std::cout << "DEBUG temp  d neg: " << d << std::endl;
                _p_t = (int) d * -1;
            } else {
                auto d = std::stoi(m[1].str());            
                std::cout << "DEBUG temp  d pos: " << d << std::endl;
                _p_t = (int) d; 
            }
            for (auto x : m)
                std::cout << "DEBUG temp m" << x << std::endl;
            std::cout << "DEBUG temp  _p_t" << _p_t << std::endl;
            std::cout << "DEBUG temp  line: " << line << std::endl;
            match_flag++;
            break;
        }
    }
    // _p_x
    for ( int pi = 0 ; pi <  xlut_size ; pi++)
    {
        param_lut p = xlut[pi];
        std::regex regexp(p.patterns);
        std::smatch m;
        if (std::regex_search(line, m, regexp)) {
            _p_x = p.patterns;
            _p_x_id = p.id;
            match_flag++;
            break;
        }
    }
    // _p_tt
    for ( int pi = 0 ; pi <  ttlut_size ; pi++)
    {
        param_lut p = ttlut[pi];
        std::regex regexp(p.patterns);
        std::smatch m;
        if (std::regex_search(line, m, regexp)) {
            _p_tt = p.patterns;
            _p_tt_id = p.id;
            match_flag++;
            break;
        }
    }
   // _p_c
    for ( int pi = 0 ; pi <  cclut_size ; pi++)
    {
        param_lut p = cclut[pi];
        std::regex regexp(p.patterns);
        std::smatch m;
        if (std::regex_search(line, m, regexp)) {
            _p_c = p.patterns;
            _p_c_id = p.id;
            match_flag++;
            break;
        }
    }
    if ( match_flag < 3 ) {
        std::cout << "WARN: Invalid Line (" << match_flag << "): " << line << std::endl;
    } else {
        std::cout << " volt: " << _p_v << std::endl;
        std::cout << " temp: " << _p_t << std::endl;
        std::cout << " extr: " << _p_x << std::endl;
        std::cout << " tt: " << _p_tt << std::endl;
        std::cout << " corner: " << _p_c << std::endl;
    }


}

float Data::get_v()
{
    return _p_v;
}

int Data::get_t()
{
    return _p_t;
}
std::string Data::get_x()
{
    return _p_x;
}
int Data::get_x_code()
{
    return _p_x_id;
}
std::string Data::get_tt()
{
    return _p_tt;
}
int Data::get_tt_code()
{
    return _p_tt_id;
}
std::string Data::get_c()
{
    return _p_c;
}
int Data::get_c_code()
{
    return _p_c_id;
}
