#include "BumbleCeepp.hpp"
#include <string>
#include <FQueueElement.hpp>
#include <Commands/Commands.hpp>
#include <oggz/oggz.h>

BumbleCeepp::BumbleCeepp(std::string token, std::string DBURL, std::string DBID, std::string DBPassword, int clusterCount)
    : IBot(token, clusterCount)
{
    sql::Properties pro({
        {"user", DBID},
        {"password", DBPassword}
    });

    conn = sql::mariadb::get_driver_instance()->connect(DBURL, pro);
    
    commandsArray.push_back(std::make_shared<commands::Play>(botCluster->me.id, this));
    commandsArray.push_back(std::make_shared<commands::Repeat>(botCluster->me.id, this));
    commandsArray.push_back(std::make_shared<commands::Queue>(botCluster->me.id, this));
    commandsArray.push_back(std::make_shared<commands::Skip>(botCluster->me.id, this));
    commandsArray.push_back(std::make_shared<commands::Leave>(botCluster->me.id, this));
    commandsArray.push_back(std::make_shared<commands::Delete>(botCluster->me.id, this));
    
    botCluster->on_voice_track_marker([&](const dpp::voice_track_marker_t &marker)
    {
        marker.voice_client->log(dpp::loglevel::ll_debug, "nowPlaying " + nowPlayingMusic);
        std::shared_ptr<dpp::embed> embed;
        if (nowPlayingMusic == "") {
            nowPlayingMusic = marker.track_meta;
            embed = findEmbed(nowPlayingMusic);
        }
        else {
            embed = findEmbed(nowPlayingMusic);
            nowPlayingMusic = marker.track_meta;
        }
        auto voice_members = dpp::find_guild(marker.voice_client->server_id)->voice_members;
        dpp::snowflake connectedChannel = marker.voice_client->channel_id;
        int memberCount = 0;
        for (auto member : voice_members)
            if ( member.second.channel_id == connectedChannel )
                memberCount++;

        if (!memberCount)
        {
            auto joinedShard = marker.from;
            std::cout << "voicechat is empty.";
            marker.voice_client->stop_audio();
            joinedShard->disconnect_voice(marker.voice_client->server_id);
            return;
        }

        marker.voice_client->log(dpp::loglevel::ll_debug, "Playing " + marker.track_meta + "on channel id " + marker.voice_client->channel_id.str() + ".");

        int remainingSongsCount = marker.voice_client->get_tracks_remaining();
        marker.voice_client->log(dpp::loglevel::ll_debug, "Marker count : " + std::to_string(remainingSongsCount));

        if (remainingSongsCount <= 1 && !marker.voice_client->is_playing())
        {
            auto joinedShard = marker.from;
            std::cout << "Queue ended\n";
            if (!joinedShard)
                return;

            if (repeat) {
                std::shared_ptr<dpp::embed> embed = findEmbed(nowPlayingMusic);
                enqueueMusic({nowPlayingMusic, *embed}, marker.voice_client);
            }
            else {
                marker.voice_client->stop_audio();
                joinedShard->disconnect_voice(marker.voice_client->server_id);
                return;
            }
        }

        if (repeat) {
            if (!embed) {
                botCluster->log(dpp::loglevel::ll_error, std::string("알 수 없는 오류 발생!"));
                return;
            }

            enqueueMusic({nowPlayingMusic, *embed}, marker.voice_client);
        }
    });
}

BumbleCeepp::~BumbleCeepp()
{
    conn->close();
}

void BumbleCeepp::enqueueMusic(FQueueElement item, dpp::discord_voice_client* vc)
{
    vc->insert_marker(item.ID);
    vc->log(dpp::loglevel::ll_debug, "Enqueueuing " + item.ID + "on channel id " + vc->channel_id.str() + ".");
    std::lock_guard<std::mutex> lock(enqueuingMutexMap[vc->server_id]);

    OGGZ *track_og = oggz_open(("Music/" + item.ID + ".ogg").c_str(), OGGZ_READ);

    /* If there was an issue reading the file, tell the user and stop */
    if (!track_og) {
        fprintf(stderr, "Error opening file\n");
        return;
    }

    /* set read callback, this callback will be called on packets with the serialno,
        * -1 means every packet will be handled with this callback.
        */
    oggz_set_read_callback(
        track_og, -1,
        [](OGGZ *oggz, oggz_packet *packet, long serialno, void *user_data) {
            dpp::discord_voice_client *vc = (dpp::discord_voice_client *)user_data;

            /* send the audio */
            vc->send_audio_opus(packet->op.packet, packet->op.bytes);

            /* make sure to always return 0 here, read more on oggz documentation */
            return 0;
        },
        /* this will be the value of void *user_data */
        (void *)vc
    );

    // read loop
    while (vc && !vc->terminating) {
        /* you can tweak this to whatever. Here I use BUFSIZ, defined in
            * stdio.h as 8192.
            */
        static const constexpr long CHUNK_READ = BUFSIZ * 2;

        const long read_bytes = oggz_read(track_og, CHUNK_READ);

        /* break on eof */
        if (!read_bytes) {
            break;
        }
    }

    /* Don't forget to free the memory */
    oggz_close(track_og);

    vc->log(dpp::loglevel::ll_debug, "Enqueued " + item.ID + "on channel id " + vc->channel_id.str() + ".");
}

std::shared_ptr<dpp::embed> BumbleCeepp::findEmbed(std::string musicID)
{
    sql::ResultSet* res;
    std::shared_ptr<dpp::embed> returnValue;
    try {
        std::unique_ptr<sql::PreparedStatement> stmnt(conn->prepareStatement("SELECT * FROM songs_info WHERE ID = ?"));
        stmnt->setString(1, musicID);
        res = stmnt->executeQuery();

        if (!res->next()) {
            return nullptr;
        }

        returnValue = makeEmbed(
            res->getString("webpage_url").c_str(),
            res->getString("title").c_str(),
            res->getString("uploader").c_str(),
            musicID,
            res->getString("thumbnail").c_str(),
            res->getInt("duration"));
    }
    catch(sql::SQLException& e){
        botCluster->log(dpp::loglevel::ll_error, std::string("SQLError: ") + e.what());
        return nullptr;
    }

    return returnValue;
}

bool BumbleCeepp::insertDB(
    std::string webpage_url, std::string title, std::string uploader, std::string id, std::string thumbnail, time_t duration)
{
    try {
        std::unique_ptr<sql::PreparedStatement> stmnt(
            conn->prepareStatement("REPLACE INTO songs_info (ID, webpage_url, title, uploader, thumbnail, duration) VALUE (?, ?, ?, ?, ?, ?)"));
        stmnt->setString(1, id);
        stmnt->setString(2, webpage_url);
        stmnt->setString(3, title);
        stmnt->setString(4, uploader);
        stmnt->setString(5, thumbnail);
        stmnt->setInt(6, duration);
        stmnt->executeQuery();

        return true;
    }
    catch(sql::SQLException& e){
        botCluster->log(dpp::loglevel::ll_debug, std::string("SQLError: ") + e.what());
        return false;
    }
}

std::shared_ptr<dpp::embed> BumbleCeepp::makeEmbed(
    std::string webpage_url, std::string title, std::string uploader, std::string id, std::string thumbnail, time_t duration)
{
    char SongLengthStr[10];
    tm t;
    t.tm_mday = duration / 86400;
    t.tm_hour = (duration % 86400)/3600;
    t.tm_min = (duration % 3600)/60;
    t.tm_sec = duration%60;
    strftime(SongLengthStr, sizeof(SongLengthStr), "%X", &t);

    dpp::embed returnValue = dpp::embed()
        .set_color(dpp::colors::sti_blue)
        .set_title(title)
        .set_description(uploader)
        .set_url(webpage_url)
        .set_image(thumbnail)
        .add_field(
            "길이",
            SongLengthStr,
            true
        );

    insertDB(webpage_url, title, uploader, id, thumbnail, duration);

    return std::make_shared<dpp::embed>(returnValue);
}