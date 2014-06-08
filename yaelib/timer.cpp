#include <chrono>

#include "timer.hpp"

struct timer::timer_private {
    static const double r;
    std::chrono::high_resolution_clock::time_point start;
};

const double timer::timer_private::r = (double) std::chrono::high_resolution_clock::period::num / std::chrono::high_resolution_clock::period::den;

timer::timer() {
    p = std::unique_ptr<timer::timer_private>(new timer::timer_private());
    reset();
}

timer::~timer() {}
    
void timer::reset() {
    p->start = std::chrono::high_resolution_clock::now();
}

double timer::elapsed() {
    std::chrono::high_resolution_clock::time_point now = std::chrono::high_resolution_clock::now();
    std::chrono::high_resolution_clock::duration duration = now - p->start;
    return duration.count() * p->r;
}
