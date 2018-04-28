#pragma once

#include <ow/exceptions.hpp>

namespace ow {

class checkable {
public:
	explicit checkable(bool initial_state = true) : p_state{initial_state} {}

	explicit operator bool() const { return p_state; }

protected:
	void chk_state(std::string_view msg = "Invalid state") const {
		if (!*this) {
			throw invalid_state(msg.data());
		}
	}

	bool p_state;
};

}
