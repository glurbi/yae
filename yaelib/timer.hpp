#ifndef _timer_hpp_
#define _timer_hpp_

#include <memory>

class timer {
public:
    timer();
    ~timer();
    void reset();
    double elapsed();
private:
    struct timer_private;
    std::unique_ptr<timer_private> p;
};

#endif