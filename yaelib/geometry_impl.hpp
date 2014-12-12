template<class T, GLenum T_primitive>
geometry<T,T_primitive>::geometry(GLsizei count, GLint dimensions)
    : count(count), positions_id(0), tex_coords_id(0), normals_id(0), dimensions(dimensions)
{
}

template<class T, GLenum T_primitive>
geometry<T, T_primitive>::~geometry()
{
    glDeleteBuffers(1, &positions_id);
    glDeleteBuffers(1, &tex_coords_id);
    glDeleteBuffers(1, &normals_id);
}

template<class T, GLenum T_primitive>
void geometry<T, T_primitive>::set_vertex_positions(GLuint positionsId_)
{
    positions_id = positionsId_;
}

template<class T, GLenum T_primitive>
void geometry<T, T_primitive>::set_vertex_tex_coords(GLuint texCoordsId_)
{
    tex_coords_id = texCoordsId_;
}

template<class T, GLenum T_primitive>
void geometry<T, T_primitive>::set_vertex_normals(GLuint normalsId_)
{
    normals_id = normalsId_;
}

template<class T, GLenum T_primitive>
void geometry<T, T_primitive>::set_vertex_positions(void* data, long size)
{
    glGenBuffers(1, &positions_id);
    glBindBuffer(GL_ARRAY_BUFFER, positions_id);
    glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
}

template<class T, GLenum T_primitive>
void geometry<T, T_primitive>::set_vertex_tex_coords(void* data, long size)
{
    glGenBuffers(1, &tex_coords_id);
    glBindBuffer(GL_ARRAY_BUFFER, tex_coords_id);
    glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
}

template<class T, GLenum T_primitive>
void geometry<T, T_primitive>::set_vertex_normals(void* data, long size)
{
    glGenBuffers(1, &normals_id);
    glBindBuffer(GL_ARRAY_BUFFER, normals_id);
    glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
}

template<class T, GLenum T_primitive>
GLuint geometry<T, T_primitive>::get_positions_id() const
{
    return positions_id;
}

template<class T, GLenum T_primitive>
GLuint geometry<T, T_primitive>::get_tex_coords_id() const {
    return tex_coords_id;
}

template<class T, GLenum T_primitive>
GLuint geometry<T, T_primitive>::get_normals_id() const
{
    return normals_id;
}

template<class T, GLenum T_primitive>
GLsizei geometry<T, T_primitive>::get_count() const {
    return count;
}

template<class T, GLenum T_primitive>
GLint geometry<T, T_primitive>::get_dimensions() const
{
    return dimensions;
}

template <class T>
buffer_object_builder<T>::buffer_object_builder(std::vector<T> data_)
{
    data = data_;
}

template <class T>
void* buffer_object_builder<T>::get_data()
{
    return &data[0];
}

template <class T>
GLsizeiptr buffer_object_builder<T>::get_size()
{
    return data.size() * sizeof(T);
}

template <class T>
GLsizeiptr buffer_object_builder<T>::get_count()
{
    return data.size();
}

template <class T>
GLuint buffer_object_builder<T>::build()
{
    GLuint id;
    glGenBuffers(1, &id);
    glBindBuffer(GL_ARRAY_BUFFER, id);
    glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(T), &data[0], GL_STATIC_DRAW);
    return id;
}

template <class T>
geometry_builder<T>::geometry_builder(GLint dimensions)
    : dimensions(dimensions)
{
    data.push(std::vector<T>());
}

template <class T>
std::unique_ptr<geometry<T>> geometry_builder<T>::build()
{
    auto b = buffer_object_builder<T> { data.top() };
    auto g = std::make_unique<geometry<T>>(data.top().size() / dimensions, dimensions);
    g->set_vertex_positions(b.build());
    return g;
}

template <class T>
geometry_builder<T>& geometry_builder<T>::operator<<(const std::vector<T>& v)
{
    data.top().insert(data.top().end(), v.begin(), v.end());
    return *this;
}

template <class T>
geometry_builder<T>& geometry_builder<T>::make_grid(int nx, int ny)
{
    for (int y = 0; y < ny; y++) {
        for (int x = 0; x < nx; x++) {
            auto v = std::vector<T> { (T)x, (T)y, (T)0, (T)(x + 1), (T)y, (T)0, (T)(x + 1), (T)(y + 1), (T)0, (T)x, (T)(y + 1), (T)0 };
            data.top().insert(data.top().end(), v.begin(), v.end());
        }
    }
    return *this;
}

template <class T>
geometry_builder<T>& geometry_builder<T>::transform(const matrix44f& tr)
{
    for (typename std::vector<T>::size_type i = 0; i < data.top().size(); i += 3) {
        vector3f v3 = tr * vector3f(&data.top()[i]);
        v3.copy(&data.top()[i]);
    }
    return *this;
}

template <class T>
geometry_builder<T>& geometry_builder<T>::begin()
{
    data.push(std::vector<T>());
    return *this;
}

template <class T>
geometry_builder<T>& geometry_builder<T>::end()
{
    std::vector<T> oldtop = std::move(data.top());
    data.pop();
    data.top().insert(data.top().end(), oldtop.begin(), oldtop.end());
    return *this;
}
