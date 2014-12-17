#ifndef _context_hpp_
#define _context_hpp_

#include <vector>
#include <memory>

#include "matrix.hpp"
#include "texture.hpp"
#include "program.hpp"

class shader_program;

class rendering_context {
public:
    rendering_context();
    void projection(matrix44f mat);
    void push(matrix44f mat);
    void pop();
    matrix44f mvp();
    matrix44f mv();
    void reset();
    vector3f dir;
    double elapsed_time_seconds;
    double last_frame_times_seconds[100];
    long frame_count;
    std::shared_ptr<program> prog;
private:
    std::vector<matrix44f> mvp_stack;
    std::vector<matrix44f> mv_stack;
};

#endif
