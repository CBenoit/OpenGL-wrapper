#pragma once

#include <array>
#include <optional>
#include <iostream>

#include <glad/glad.h>

#include "checkable.hpp"
#include "opengl_codes.hpp"
#include "utils.hpp"

namespace ow {

	struct VBO_attribs {

		VBO_attribs& operator=(const VBO_attribs& other) {
			if (other.layout_size) {
				layout_size = other.layout_size;
			}
			if (other.stride) {
				stride = other.stride;
			}
			if (other.layout_offset) {
				layout_offset = other.layout_offset;
			}
			if (other.layout_index) {
				layout_index = other.layout_index;
			}
			if (other.normalized) {
				normalized = other.normalized;
			}
			return *this;
		}

		std::optional<GLuint> layout_size;
		std::optional<GLsizei> stride;
		std::optional<size_t> layout_offset;
		std::optional<GLuint> layout_index;
		std::optional<bool> normalized;
	};

	template<unsigned int N>
	class VBO : public checkable {
	public:

		template<typename = std::enable_if_t<(N > 0)>>
		VBO() : checkable{}, m_attributes{}, selected_idx{0}, vbo_array{} {
			GLuint array[N];
			glGenBuffers(N, array);
			if (glGetError() != GL_NO_ERROR) {
				throw std::runtime_error("Chuck Norris");
			}
			for (auto i = 0u; i < N; ++i) {
				id(i) = array[i];
				usable(i) = false;
			}

			m_attributes.layout_size = 3u;
			m_attributes.stride = 0;
			m_attributes.layout_offset = 0u;

			m_attributes.layout_index = 0u;
			m_attributes.normalized = false;
		}

		VBO(const VBO<N>& other) = delete;

		VBO(VBO<N>&& other) noexcept
				: m_attributes(other.m_attributes)
				, selected_idx{other.selected_idx}
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

		void select_vbo(unsigned int idx) {
			selected_idx = idx;
		}

		unsigned int current_idx() const {
			return selected_idx;
		}

		GLuint id(unsigned int idx = current_idx()) const {
			return std::get<0>(vbo_array[idx]);
		}


		template<typename T>
		void set_data(const T &data, GLenum usage = GL_STATIC_DRAW) {
			set_data_s(current_idx(), data, usage);
		}

		template<typename T>
		void set_data_s(unsigned int idx, const T &data, GLenum usage = GL_STATIC_DRAW) {
			bind(idx);
			glBufferData(GL_ARRAY_BUFFER, sizeof(typename T::value_type) * data.size(), data.data(), usage);
			p_state = p_state &&
			          check_errors("Error while setting data in buffer " + std::to_string(id(idx)) + ".\n");
			type(idx) = get_gl_type<typename T::value_type>();
			usable(idx) = true;
			size_of(idx) = sizeof(typename T::value_type);
		}

		void set_attributes(const VBO_attribs& attribs) noexcept {
			m_attributes = attribs;
		}

		VBO_attribs& attribs() noexcept {
			return m_attributes;
		}

		const VBO_attribs& attribs() const noexcept {
			return m_attributes;
		}

		void flush_layout_attribs() {
			flush_layout_attrib(current_idx());
		}

		void flush_layout_attrib(unsigned int vbo_idx) {
			bind(vbo_idx);
			glVertexAttribPointer(
					*m_attributes.layout_index
					, *m_attributes.layout_size
					, type(0)
					, static_cast<GLboolean>(*m_attributes.normalized ? GL_TRUE : GL_FALSE)
					, static_cast<GLsizei>(*m_attributes.stride * size_of(vbo_idx))
					, (void *) (*m_attributes.layout_offset)
			);
			glEnableVertexAttribArray(*m_attributes.layout_index);
		}

	private:

		void bound_chk(unsigned int idx) const {
			if (idx >= N) {
				throw std::out_of_range(
						"[VBO] Accessing index " + std::to_string(idx) + " (max is " + std::to_string(N - 1) + ")");
			}
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

		size_t& size_of(unsigned int idx) {
			bound_chk(idx);
			return std::get<3>(vbo_array[idx]);
		}

		void bind(unsigned int idx) {
			glBindBuffer(GL_ARRAY_BUFFER, id(idx));
			p_state = check_errors("Error while binding array buffer " + std::to_string(id(idx)) + ".\n");
		}

		VBO_attribs m_attributes;
		unsigned int selected_idx;
		std::array<std::tuple<GLuint, GLenum, bool, size_t>, N> vbo_array;
	};
}
