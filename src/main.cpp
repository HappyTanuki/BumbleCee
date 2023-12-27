#include <BumbleCeepp.hpp>
#include <Commands.hpp>
#include <dpp/nlohmann/json.hpp>

using json = nlohmann::json;

int main() {
    json configdocument;
    std::ifstream configfile("config.json");
    configfile >> configdocument;

    std::shared_ptr<BumbleCeepp> BumbleBee(BumbleCeepp::GetInstance(configdocument["token"]));

    Commands::Play Command1(BumbleBee);
    Commands::Repeat Command2(BumbleBee);
    Commands::Queue Command3(BumbleBee);
    Commands::Skip Command4(BumbleBee);
    Commands::Leave Command5(BumbleBee);
    Commands::Delete Command6(BumbleBee);

    BumbleBee->AddCommand(Command1);
    BumbleBee->AddCommand(Command2);
    BumbleBee->AddCommand(Command3);
    BumbleBee->AddCommand(Command4);
    BumbleBee->AddCommand(Command5);
    BumbleBee->AddCommand(Command6);

    BumbleBee->Start();

    return 0;
}