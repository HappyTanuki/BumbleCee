#include <Commands/BumbleBeeCommand.hpp>

namespace bumbleBee::commands {
    void Repeat::execute(const dpp::slashcommand_t &event) {
        event.edit_original_response(dpp::message("repeat"));
    }

    void Repeat::init() {
    }
}