#ifndef _QUEUE_HPP_
#define _QUEUE_HPP_
#include <CommandType.hpp>
#include <BumbleCeepp.hpp>
#include <memory>

class Queue : public ICommand {
public:
    Queue(std::shared_ptr<BumbleCeepp> Bot);

    void operator()(const dpp::slashcommand_t& Event) {}
    void operator()(std::list<FQueueElement>& MusicQueue, const dpp::slashcommand_t& Event);
private:
    std::shared_ptr<BumbleCeepp> Bot;
};

#endif