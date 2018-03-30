#pragma once

#include <array>
#include <iostream>

#include <glad/glad.h>

#include "checkable.hpp"
#include "opengl_codes.hpp"
#include "utils.hpp"

namespace ow {

	template<unsigned int N>
	class VBO : public checkable {
	public:

		template<typename = std::enable_if_t<(N > 0)>>
		VBO() : checkable{}, selected_idx{0}, vbo_array{} {
			GLuint array[N];
			glGenBuffers(N, array);
			if (glGetError() != GL_NO_ERROR) {
				throw std::runtime_error("Chuck Norris");
			}
			for (auto i = 0u; i < N; ++i) {
				id(i) = array[i];
				usable(i) = false;
			}
		}

		VBO(const VBO& other) = delete;

		VBO(VBO&& other) noexcept
				: selected_idx{other.selected_idx}
				, vbo_array{} {
			for (auto i = 0u; i < N; ++i) {
				id(i) = other.id(i);
				other.id(i) = 0;
			}
		}

		~VBO() {
			GLuint array[N];
			for (auto i = 0u; i < N; ++i) {
				array[i] = id(i);
			}
			glDeleteBuffers(N, array);
			check_errors("Chuck Norris exception.\n");
		}

		void select_idx(unsigned int idx) {
			selected_idx = idx;
		}

		unsigned int current_idx() const {
			return selected_idx;
		}

		GLuint id(unsigned int idx = current_idx()) const {
			return std::get<0>(vbo_array[idx]);
		}


		template<typename T>
		void set_data(const T &data, GLenum usage) {
			set_data_s(current_idx(), data, usage);
		}

		template<typename T>
		void set_data_s(unsigned int idx, const T &data, GLenum usage) {
			bind(idx);
			glBufferData(GL_ARRAY_BUFFER, sizeof(typename T::value_type) * data.size(), data.data(), usage);
			p_state = p_state &&
					check_errors("Error while setting data in buffer " + std::to_string(id(idx)) + ".\n");
			type(idx) = get_gl_type<typename T::value_type>();
			usable(idx) = true;
			size_of(idx) = sizeof(typename T::value_type);
		}

		void setall_attributes(GLuint layout_size, GLsizei stride = 0, unsigned int layout_offset = 0,
		                       GLuint vertex_attribute_index = 0, bool normalized = true) {
			for (auto i = 0u; i < N; ++i) {
				set_attributes_s(i, layout_size, stride, layout_offset, vertex_attribute_index, normalized);
			}
		}

		void enable_layout(GLuint layout = 0) {
			enable_layout_s(current_idx(), layout);
		}

		void enable_layout_s(unsigned int idx, GLuint layout = 0) {
			bind(idx);
			glEnableVertexAttribArray(layout);
		}

		void set_attributes(GLuint layout_size, GLsizei stride = 0, GLuint vertex_attribute_index = 0,
		                    unsigned int layout_offset = 0, bool normalized = true) {
			set_attributes_s(current_idx(), layout_size, stride, vertex_attribute_index, layout_offset, normalized);
		}

		void set_attributes_s(unsigned int idx, GLuint layout_size, GLsizei stride = 0,
		                      GLuint vertex_attribute_index = 0, unsigned int layout_offset = 0,
		                      bool normalized = true) {
			if (!usable(idx)) {
				ow::logger << "[VBO]: You must set data before setting attributes !.\n";
				return;
			}

			bind(idx);
			glVertexAttribPointer(
					vertex_attribute_index, layout_size, type(idx),
					static_cast<GLboolean>(normalized ? GL_TRUE : GL_FALSE),
					stride * size_of(idx), (void *) (layout_offset * size_of(idx)));
			p_state = p_state &&
					check_errors("Error while setting data in buffer " + std::to_string(id(idx)) + ".\n");
		}

		void use() const {
			bind(selected_idx);
		}

		void use_s(unsigned int idx) const {
			bind(idx);
		}

	private:

		void bound_chk(unsigned int idx) const {
			if (idx >= N) {
				throw std::out_of_range(
						"[VBO] Accessing index " + std::to_string(idx) + " (max is " + std::to_string(N - 1) + ")");
			}
		}

		GLuint id(unsigned int idx) const {
			bound_chk(idx);
			return std::get<0>(vbo_array[idx]);
		}

		GLuint& id(unsigned int idx) {
			bound_chk(idx);
			return std::get<0>(vbo_array[idx]);
		}

		GLenum& type(unsigned int idx) {
			bound_chk(idx);
			return std::get<1>(vbo_array[idx]);
		}

		bool& usable(unsigned int idx) {
			bound_chk(idx);
			return std::get<2>(vbo_array[idx]);
		}

		unsigned char& size_of(unsigned int idx) {
			bound_chk(idx);
			return std::get<3>(vbo_array[idx]);
		}

		void bind(unsigned int idx) const {
			glBindBuffer(GL_ARRAY_BUFFER, id(idx));
			p_state = check_errors("Error while binding array buffer " + std::to_string(id(idx)) + ".\n");
		}

		unsigned int selected_idx;
		std::array<std::tuple<GLuint, GLenum, bool, unsigned char>, N> vbo_array;
	};
}
