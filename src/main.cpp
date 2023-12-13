#include <BumbleCeepp.hpp>
#include <Commands.hpp>
#include <dpp/nlohmann/json.hpp>

using json = nlohmann::json;

int main() {
    json configdocument;
    std::ifstream configfile("../config.json");
    configfile >> configdocument;

    std::shared_ptr<BumbleCeepp> BumbleBee(BumbleCeepp::GetInstance(configdocument["token"]));

    Play Command1(BumbleBee);

    BumbleBee->AddCommand(Command1);

    BumbleBee->Start();

    return 0;
}