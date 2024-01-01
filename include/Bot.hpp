#ifndef _BOT_HPP_
#define _BOT_HPP_
#include <CommandType.hpp>
#include <dpp/dpp.h>
#include <memory>

class IBot {
private:
public:
    IBot(std::string Token, int TotalShard);
    void AddCommand(ICommand &Command);
    void Start();

    std::shared_ptr<dpp::cluster> BotCluster;
protected:
    virtual void OnReady(const dpp::ready_t& event);
    virtual void OnCommand(const dpp::slashcommand_t& event);

    std::vector<ICommand*> CommandsArray;
};

#endif