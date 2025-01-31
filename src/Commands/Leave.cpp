#include <Commands/BumbleBeeCommand.hpp>

namespace bumbleBee::commands {
    void Leave::execute(const dpp::slashcommand_t &event) {
        event.edit_original_response(dpp::message("leave"));
    }

    void Leave::init() {
    }
}