#include <Commands/Queue.hpp>
#include <iostream>

Queue::Queue(std::shared_ptr<BumbleCeepp> Bot) {
    this->Bot = Bot;

    dpp::slashcommand Command = dpp::slashcommand("queue", "노래 예약 큐 확인", Bot->BotCluster->me.id);
    dpp::slashcommand Alias = dpp::slashcommand("q", "노래 예약 큐 확인", Bot->BotCluster->me.id);

    CommandObjectVector.push_back(Command);
    CommandObjectVector.push_back(Alias);
}

void Queue::operator()(std::list<FQueueElement>& MusicQueue, const dpp::slashcommand_t& Event) {
    dpp::embed embed = dpp::embed()
        .set_color(dpp::colors::sti_blue)
        .set_title("영상 제목&링크")
        .set_url("https://dpp.dev/")
        .set_description("영상 설명란")
        .set_image("https://dpp.dev/DPP-Logo.png")
        .set_footer(
            dpp::embed_footer()
            .set_text("업로더 이름")
            .set_icon("https://dpp.dev/DPP-Logo.png")
        )
        .set_timestamp(time(0));

    dpp::message msg(Event.command.channel_id, "some Text");
    msg.add_embed(embed);

    Event.reply(msg);
}