#include <Commands/Delete.hpp>
#include <iostream>

namespace Commands {
    Delete::Delete(std::shared_ptr<BumbleCeepp> Bot) {
        this->Bot = Bot;

        dpp::slashcommand Command = dpp::slashcommand("d", "큐의 해당하는 번호의 노래를 지웁니다", Bot->BotCluster->me.id);

        Command.add_option(
            dpp::command_option(dpp::co_string, "pos", "큐 번호", Bot->BotCluster->me.id)
        );

        CommandObjectVector.push_back(Command);
    }

    void Delete::operator()(std::list<FQueueElement>& MusicQueue, const dpp::slashcommand_t& Event) {
        std::string Pos = std::get<std::string>(Event.get_parameter("pos"));
        if (!atoi(Pos.c_str())) {
            dpp::message msg(Event.command.channel_id, "현재 재생중인 곡은 삭제할 수 없습니다!");
            Event.reply(msg);
            return;
        }

        auto PopedElement = Bot->QueueDelete(atoi(Pos.c_str()));

        dpp::embed embed = dpp::embed()
            .set_title(PopedElement.title)
            .set_description(PopedElement.description)
            .set_color(dpp::colors::sti_blue)
            .set_image(PopedElement.thumbnail)
            .set_timestamp(time(0));

        dpp::message msg(Event.command.channel_id, "다음 항목을 큐에서 삭제했습니다!:");
        msg.add_embed(embed);

        Event.reply(msg);
    }
}