#ifndef _LEAVE_HPP_
#define _LEAVE_HPP_
#include <CommandType.hpp>
#include <BumbleCeepp.hpp>
#include <memory>

namespace Commands {
    class Leave : public ICommand {
    public:
        Leave(std::shared_ptr<BumbleCeepp> Bot);

        void operator()(const dpp::slashcommand_t& Event) {}
        void operator()(std::list<FQueueElement>& MusicQueue, const dpp::slashcommand_t& Event);
    private:
        std::shared_ptr<BumbleCeepp> Bot;
    };
}

#endif