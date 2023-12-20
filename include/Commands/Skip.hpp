#ifndef _SKIP_HPP_
#define _SKIP_HPP_
#include <CommandType.hpp>
#include <BumbleCeepp.hpp>
#include <memory>

class Skip : public ICommand {
public:
    Skip(std::shared_ptr<BumbleCeepp> Bot);

    void operator()(const dpp::slashcommand_t& Event) {}
    void operator()(std::list<FQueueElement>& MusicQueue, const dpp::slashcommand_t& Event);
private:
    std::shared_ptr<BumbleCeepp> Bot;
};

#endif