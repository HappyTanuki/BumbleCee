#include <BumbleCeepp.hpp>
#include <dpp/nlohmann/json.hpp>
#include <memory>

using json = nlohmann::json;

int main()
{
    json configdocument;
    std::ifstream configfile("config.json");
    configfile >> configdocument;

    // setvbuf(stdout, NULL, _IONBF, 0);

    std::shared_ptr<BumbleCeepp> bumbleBee = std::make_shared<BumbleCeepp>(
        configdocument["token"], configdocument["dbURL"], configdocument["user"], configdocument["password"]);

    bumbleBee->start();
    
    return 0;
}