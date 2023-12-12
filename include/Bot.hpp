#pragma once
#include "CommandType.hpp"
#include <dpp/dpp.h>
#include <memory>

class IBot {
private:
public:
    IBot(std::string Token);
    void AddCommand(ICommand &Command);
    void Start();

    std::shared_ptr<dpp::cluster> bot;
protected:
    virtual void OnReady(const dpp::ready_t& event);
    virtual void OnCommand(const dpp::slashcommand_t& event);

    std::vector<ICommand*> CommandsArray;
};