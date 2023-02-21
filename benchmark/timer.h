#pragma once

#include <chrono>
#include <string>
#include <vector>

using timelist_t = std::vector<long>;

class Timer {
    
    public:
        Timer(std::string);
        ~Timer();
        void Stop();

        static std::vector<std::string> s_labels; // function names
        static std::vector<timelist_t> s_times;
        static void s_print_times();
        static std::vector<unsigned> s_labels_occurrencies; // number of times a func is called
        static long s_compute_mean(timelist_t);
        static long s_compute_conf_int(timelist_t);
    
    private:
        std::string label; 
        unsigned label_index;
        std::chrono::time_point< std::chrono::high_resolution_clock> m_StartTimepoint;

        int search_label(std::string);
};