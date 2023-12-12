#include <Commands/Queue.hpp>
#include <iostream>

Queue::Queue(dpp::snowflake Id) {
    dpp::slashcommand Command = dpp::slashcommand("queue", "노래 예약 큐 확인", Id);
    dpp::slashcommand Alias = dpp::slashcommand("q", "노래 예약 큐 확인", Id);

    CommandObjectVector.push_back(Command);
    CommandObjectVector.push_back(Alias);
}

void Queue::operator()(std::list<std::string>& MusicQueue, const dpp::slashcommand_t& Event) {

}