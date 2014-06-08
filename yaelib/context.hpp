#ifndef _context_hpp_
#define _context_hpp_

#include <vector>
#include <memory>

#include "matrix.hpp"
#include "texture.hpp"
#include "program.hpp"

class program;

class rendering_context {
public:
    rendering_context();
    void projection(matrix44 mat);
    void push(matrix44 mat);
    void pop();
    matrix44 mvp();
    matrix44 mv();
    void reset();
    vector3 dir;
    double elapsed_time_seconds;
    double last_frame_times_seconds[100];
    long frame_count;
    std::shared_ptr<program> prog;
private:
    std::vector<matrix44> mvp_stack;
    std::vector<matrix44> mv_stack;
};


#endif
