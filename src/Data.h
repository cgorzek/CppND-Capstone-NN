#ifndef DATA_H
#define DATA_H

#include <string>

class Data {
    public:
        // Probably implement the rule of 5.

        Data();
        ~Data();

        // setters and getters
        void add(std::string line);

        float get_v();
        int get_t();
        std::string get_x();
        int         get_x_code();        // encoded version
        std::string get_tt();
        int         get_tt_code();       // encoded version
        std::string get_c();
        int         get_c_code();        // encoded version

    private:
        std::string _data;
        float       _p_v; // voltage
        int         _p_t; // temp
        std::string _p_x; // extraction corner
        int         _p_x_id;
        std::string _p_tt; // timing test, setup/hold
        int         _p_tt_id;
        std::string _p_c; // corner, fast/slow
        int         _p_c_id;
};

#endif