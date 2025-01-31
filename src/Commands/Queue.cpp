#include <Commands/BumbleBeeCommand.hpp>

namespace bumbleBee::commands {
    void Queue::execute(const dpp::slashcommand_t &event) {
        event.edit_original_response(dpp::message("queue"));
    }

    void Queue::init() {
    }
}