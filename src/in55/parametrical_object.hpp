#pragma once

#include <ow/mesh.hpp>

class parametrical_object : public ow::mesh {
public:
	explicit parametrical_object(unsigned int n) noexcept;
};
