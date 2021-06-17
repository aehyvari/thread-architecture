#pragma once

#include <vector>
#include <mutex>
#include <condition_variable>

template<typename T>
class Channel {
    using tp_t = std::chrono::time_point<std::chrono::system_clock>;
    std::mutex m;
    std::condition_variable cv;
    bool requestStop;
    bool isStopping;
    std::vector<T> clauses;
public:
    Channel() : requestStop(false), isStopping(false) {}

    T & operator[] (int idx)    { return clauses[idx]; }
    std::mutex & getMutex()     { return m; }
    size_t size() const         { return clauses.size(); }
    auto cbegin() const         { return clauses.cbegin(); }
    auto cend() const           { return clauses.cend(); }
    auto begin() const          { return clauses.begin(); }
    auto end() const            { return clauses.end(); }
    void notify_one()           { cv.notify_one(); }
    void clear()                { clauses.clear(); }
    bool empty() const          { return clauses.empty(); }
    void push(T c)              { clauses.emplace_back(c); }
    bool shouldStop() const     { return requestStop; }
    void setShouldStop()        { requestStop = true; }
    void clearShouldStop()      { requestStop = false; }
    bool shallStop() const      { return isStopping; }
    void setShallStop()         { isStopping = true; }
    void clearShallStop()       { isStopping = false; }
    bool waitUntil(std::unique_lock<std::mutex> & lock, const tp_t& timepoint) { return cv.wait_until(lock, timepoint, [&] { return not empty() or shallStop(); }); }
};


