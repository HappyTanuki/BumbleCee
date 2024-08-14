#include "BumbleCeepp.hpp"
#include <string>
#include <FQueueElement.hpp>
#include <Commands/Commands.hpp>
#include <oggz/oggz.h>

BumbleCeepp::BumbleCeepp(std::string token, std::string DBURL, std::string DBID, std::string DBPassword, int clusterCount)
    : IBot(token, DBURL, DBID, DBPassword, clusterCount)
{
    
    commandsArray.push_back(std::make_shared<commands::Play>(botCluster->me.id, this));
    commandsArray.push_back(std::make_shared<commands::Repeat>(botCluster->me.id, this));
    commandsArray.push_back(std::make_shared<commands::Queue>(botCluster->me.id, this));
    commandsArray.push_back(std::make_shared<commands::Skip>(botCluster->me.id, this));
    commandsArray.push_back(std::make_shared<commands::Leave>(botCluster->me.id, this));
    commandsArray.push_back(std::make_shared<commands::Delete>(botCluster->me.id, this));
    
    botCluster->on_voice_track_marker([&](const dpp::voice_track_marker_t &marker)
    {
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
        marker.voice_client->log(dpp::loglevel::ll_trace, "Marker count : " + remainingSongsCount);

        if (remainingSongsCount <= 1 && !marker.voice_client->is_playing())
        {
            auto joinedShard = marker.from;
            std::cout << "Queue ended\n";
            if (!joinedShard)
                return;
            marker.voice_client->stop_audio();
            joinedShard->disconnect_voice(marker.voice_client->server_id);
            return;
        }

        if (repeat)
            enqueueMusic({nowPlayingMusic, findEmbed(nowPlayingMusic)}, marker.voice_client);
    });

    // cluster->on_voice_ready([&](const dpp::voice_ready_t& Voice){ queue->play(); });
    
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

dpp::embed BumbleCeepp::findEmbed(std::string musicID)
{
    auto iter = musicEmbedMap.find(musicID);
    if (iter != musicEmbedMap.end())
        return iter->second;
    
    std::unique_ptr<sql::Connection> conn(DBDriver->connect(*this->DBURL, *DBProperties));
    std::unique_ptr<sql::PreparedStatement> stmnt(conn->prepareStatement("SELECT embed FROM songs_info WHERE ID = ?"));
    stmnt->setString(1, musicID);
    std::unique_ptr<sql::ResultSet> res(stmnt->executeQuery());
    
    dpp::embed returnValue = makeEmbed(
        res->getString("webpage_url").c_str(),
        res->getString("title").c_str(),
        res->getString("uploader").c_str(),
        musicID,
        res->getString("thumbnail").c_str(),
        res->getInt("duration"));

    musicEmbedMap[musicID] = returnValue;

    return returnValue;
}

dpp::embed BumbleCeepp::makeEmbed(std::string webpage_url, std::string title, std::string uploader, std::string id, std::string thumbnail, time_t duration)
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

    std::unique_ptr<sql::Connection> conn(DBDriver->connect(*this->DBURL, *DBProperties));
    std::unique_ptr<sql::PreparedStatement> stmnt(
        conn->prepareStatement("REPLACE INTO songs_info (ID, webpage_url, title, uploader, thumbnail, duration) VALUE (?, ?, ?, ?, ?, ?)"));
    stmnt->setString(1, id);
    stmnt->setString(2, webpage_url);
    stmnt->setString(3, title);
    stmnt->setString(4, uploader);
    stmnt->setString(5, thumbnail);
    stmnt->setInt(6, duration);
    stmnt->executeQuery();

    return returnValue;
}
