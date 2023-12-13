#include <Commands/Play.hpp>
#include <dpp/dpp.h>
#include <dpp/nlohmann/json.hpp>
#include <stdlib.h>

using json = nlohmann::json;

Play::Play(std::shared_ptr<BumbleCeepp> Bot) {
    this->Bot = Bot;

    dpp::slashcommand Command = dpp::slashcommand("play", "노래 재생", Bot->BotCluster->me.id);

    Command.add_option(
        dpp::command_option(dpp::co_string, "query", "링크 또는 검색어", Bot->BotCluster->me.id)
    );

    dpp::slashcommand Alias = dpp::slashcommand("p", "노래 재생", Bot->BotCluster->me.id);

    Alias.add_option(
        dpp::command_option(dpp::co_string, "query", "링크 또는 검색어", Bot->BotCluster->me.id)
    );

    CommandObjectVector.push_back(Command);
    CommandObjectVector.push_back(Alias);
}

void Play::operator()(std::list<FQueueElement>& MusicQueue, const dpp::slashcommand_t& Event) {
    if (std::holds_alternative<std::monostate>(Event.get_parameter("query"))) {
        Event.reply("노래를 재생하려면 좀 노래를 넣고 재생시켜라 게이야");
        return;
    }

    /* Attempt to connect to a voice channel, returns false if we fail to connect. */
    if (!dpp::find_guild(Event.command.guild_id)->connect_member_voice(Event.command.get_issuing_user().id)) {
        return Event.reply("우리 게이는 도대체 노래 들을 생각도 없으면서 왜 신청하는거노?");
    }

    std::string Query = std::get<std::string>(Event.get_parameter("query"));

    system(("./yt-dlp -o temp --write-info-json -f 251 " + Query).c_str());

    json document;
    std::ifstream infofile("temp.info.json");
    infofile >> document;
    system("rm -f temp.info.json");

    system(("ffmpeg -i temp -c copy " + std::string(to_string(document["id"])) + ".ogg").c_str());
    system("rm -f temp");

    FQueueElement Data = {std::string(document["title"]),
                std::string(document["uploader"]),
                std::string(document["id"]),
                std::string(document["thumbnail"]),
                to_string(document["duration"]),
                Event.command.guild_id};

    Bot->enqueue(Data);

    dpp::message msg(Event.command.channel_id, "큐에 다음 곡을 추가했습니다:");

    msg.add_embed(dpp::embed()
    .set_color(dpp::colors::sti_blue)
    .set_title(document["title"])
    .set_description(document["uploader"])
    .set_url(Query)
    .set_image(document["thumbnail"])
    .add_field(
        "길이",
        to_string(document["duration"]),
        true
    ));

    Event.reply(msg);

    dpp::voiceconn* v = Event.from->get_voice(Event.command.guild_id);

    Bot->VoiceJoinedShardId = Event.from->shard_id;

    /* If the voice channel was invalid, or there is an issue with it, then tell the user. */
    if (v && v->voiceclient && v->voiceclient->is_ready()) {
        return Bot->QueuePlay();
    }

    Bot->BotCluster->on_voice_ready([this](const dpp::voice_ready_t& Voice){ Bot->QueuePlay(); });
    return;
}