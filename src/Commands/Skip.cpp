#include <Commands/BumbleBeeCommand.hpp>

namespace bumbleBee::commands {
    void Skip::execute(const dpp::slashcommand_t &event) {
        event.edit_original_response(dpp::message("skip"));
    }

    void Skip::init() {
    }
}