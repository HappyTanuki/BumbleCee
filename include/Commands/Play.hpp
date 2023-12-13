#ifndef _PLAY_HPP_
#define _PLAY_HPP_
#include <CommandType.hpp>
#include <BumbleCeepp.hpp>
#include <memory>

class Play : public ICommand {
public:
    Play(std::shared_ptr<BumbleCeepp> Bot);

    void operator()(const dpp::slashcommand_t& Event) {}
    void operator()(std::list<FQueueElement>& MusicQueue, const dpp::slashcommand_t& Event);
private:
    std::shared_ptr<BumbleCeepp> Bot;
};

#endif