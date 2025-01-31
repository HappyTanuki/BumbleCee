#include <Commands/BumbleBeeCommand.hpp>

namespace bumbleBee::commands {
    void Delete::execute(const dpp::slashcommand_t &event) {
        event.edit_original_response(dpp::message("delete"));
    }

    void Delete::init() {
        aliases.push_back("delete");
    }
}