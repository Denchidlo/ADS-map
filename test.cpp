#include <iostream>
#include <chrono>
#include <algorithm>
#include "src/base_map.hpp"
#include <map>

using namespace std;
using namespace std::chrono;

int main()
{
    base_map::map<uint64_t, void*> dict;
    map<uint64_t, void*> std_dict;
    auto start = high_resolution_clock::now();
    for (int i = 1; i <= 1000000; ++i)
    {
        dict.insert(i, (void*)i);
    }
    auto end = high_resolution_clock::now();

    auto duration = duration_cast<microseconds>(end - start);
  
    cout << "Time taken by function: "
         << duration.count() << " microseconds" << endl;

    start = high_resolution_clock::now();

    for (int i = 1; i <= 1000000; ++i)
    {
        dict.erase(i);
        dict.insert(i, (void*)i);
    }
    end = high_resolution_clock::now();


    duration = duration_cast<microseconds>(end - start);

    cout << "Time taken by erase function: "
         << duration.count() << " microseconds" << endl;
    
    // void* a = dict[255555];
    // cout << (a == (void*)255555) << endl;

    // a = dict[2555556];
    // cout << (a == (void*)2555556) << endl;

    // start = high_resolution_clock::now();

    // // dict.count();

    // for (int i = 1; i <= 1000000; ++i)
    // {
    //     std_dict.insert({i, (void*)i});
    // }
    // end = high_resolution_clock::now();

    // duration = duration_cast<microseconds>(end - start);
  
    // cout << "Time taken by std function: "
    //      << duration.count() << " microseconds" << endl;

    // start = high_resolution_clock::now();

    // for (int i = 1; i <= 1000000; ++i)
    // {
    //     std_dict.erase(i);
    //     std_dict.insert({i, (void*)i});
    // }
    // end = high_resolution_clock::now();

    // duration = duration_cast<microseconds>(end - start);

    // cout << "Time taken by erase function: "
    //      << duration.count() << " microseconds" << endl;

    auto iter = dict.begin();

    cout << (int64_t)(*(iter)) << endl;

    for (int i = 0; i < 100000; ++i)
        cout << (int64_t)(*(++iter)) << endl;
}
