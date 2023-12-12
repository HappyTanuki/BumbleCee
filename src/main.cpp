#include <BumbleCeepp.hpp>
#include <CommandType.hpp>
#include <iostream>
#include <Commands.hpp>
#include <dpp/nlohmann/json.hpp>

using json = nlohmann::json;

int main() {
    json configdocument;
    std::ifstream configfile("../config.json");
    configfile >> configdocument;

    std::unique_ptr<BumbleCeepp> BumbleBee(BumbleCeepp::GetInstance(configdocument["token"]));

    Play Command1(BumbleBee->bot->me.id);
    Queue Command2(BumbleBee->bot->me.id);
    Join Command3(BumbleBee->bot->me.id);

    BumbleBee->AddCommand(Command1);
    BumbleBee->AddCommand(Command2);
    BumbleBee->AddCommand(Command3);

    BumbleBee->Start();

    return 0;
}