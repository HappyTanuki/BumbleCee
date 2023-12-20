#include <BumbleCeepp.hpp>
#include <Commands.hpp>
#include <dpp/nlohmann/json.hpp>

using json = nlohmann::json;

int main() {
    json configdocument;
    std::ifstream configfile("config.json");
    configfile >> configdocument;

    std::shared_ptr<BumbleCeepp> BumbleBee(BumbleCeepp::GetInstance(configdocument["token"]));

    Play Command1(BumbleBee);
    Repeat Command2(BumbleBee);
    Queue Command3(BumbleBee);
    Skip Command4(BumbleBee);

    BumbleBee->AddCommand(Command1);
    BumbleBee->AddCommand(Command2);
    BumbleBee->AddCommand(Command3);
    BumbleBee->AddCommand(Command4);

    BumbleBee->Start();

    return 0;
}