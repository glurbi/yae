#ifndef _texture_hpp_
#define _texture_hpp_

#include <GL/glew.h>

class texture {
public:
    texture(GLubyte* data, GLsizei w, GLsizei h);
    ~texture();
    GLuint get_id() const;
private:
    GLuint id;
};

#endif