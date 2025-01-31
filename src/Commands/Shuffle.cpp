#include <Commands/BumbleBeeCommand.hpp>

namespace bumbleBee::commands {
    void Shuffle::execute(const dpp::slashcommand_t &event) {
        event.edit_original_response(dpp::message("shuffle"));
    }

    void Shuffle::init() {
    }
}