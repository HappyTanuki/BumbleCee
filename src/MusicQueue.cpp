#include <MusicQueue.hpp>
#include <ogg/ogg.h>
#include <oggz/oggz.h>
#include <opus/opusfile.h>
#include <thread>

MusicQueue::MusicQueue(FMusicQueueID id, std::shared_ptr<dpp::cluster> botCluster)
{
    this->id = id;
    repeat = false;
    this->botCluster = botCluster;

    botCluster->on_voice_track_marker([this, botCluster](const dpp::voice_track_marker_t &marker)
    {
        std::cout << marker.track_meta << " Marker reached.\n";

        if (empty())
        {
            std::cout << "Queue ended\n";
            playMutex.unlock();
            return;
        }

        auto music = pop(0);
        if (repeat)
        {
            (*this) += music;
        }
        
        markerCallback();
    });
}

void MusicQueue::operator+=(FQueueElement operand)
{
    mutex.lock();
    queue.push_back(operand);
    mutex.unlock();
}

FQueueElement MusicQueue::pop(int index)
{
    mutex.lock();
    auto iter = queue.begin();
    std::advance(iter, index);
    auto returnValue = *iter;
    queue.erase(iter);
    mutex.unlock();

    return returnValue;
}

FQueueElement MusicQueue::peek(int index)
{
    mutex.lock();
    auto iter = queue.begin();
    std::advance(iter, index);
    auto returnValue = *iter;
    mutex.unlock();

    return returnValue;
}

bool MusicQueue::empty()
{
    mutex.lock();
    bool empty = queue.empty();
    mutex.unlock();
    
    return empty;
}

void MusicQueue::clear()
{
    mutex.lock();
    queue.clear();
    mutex.unlock();
}

std::list<struct FQueueElement>::iterator MusicQueue::begin()
{
    mutex.lock();
    auto returnValue = queue.begin();
    mutex.unlock();

    return returnValue;
}

std::list<struct FQueueElement>::iterator MusicQueue::end()
{
    mutex.lock();
    auto returnValue = queue.end();
    mutex.unlock();

    return returnValue;
}

std::size_t MusicQueue::size()
{
    mutex.lock();
    auto returnValue = queue.size();
    mutex.unlock();

    return returnValue;
}

FMusicQueueID MusicQueue::getId()
{
    return id;
}

void MusicQueue::markerCallback()
{
    std::cout << "Music play started\n";

    dpp::discord_client* joinedShard = botCluster->get_shard(id.shard_id);
    if (!joinedShard)
    {
        std::cout << "No shard\n";
        return;
    }

    if (empty())
    {
        std::cout << "Queue ended\n";
        playMutex.unlock();
        return;
    }

    FQueueElement music = peek(0);

    dpp::voiceconn* v = joinedShard->get_voice(id.guild_id);
    if (!v || !v->voiceclient || !v->voiceclient->is_ready())
    {
        std::cout << "not in voicechat. quit musicplay";
        return;
    }

    //v->voiceclient->error(4014);

    /* load the audio file with oggz */
    OGGZ *track_og = oggz_open(("Music/" + music.fileName + ".ogg").c_str(), OGGZ_READ);

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
            dpp::voiceconn *voiceconn = (dpp::voiceconn *)user_data;

            /* send the audio */
            voiceconn->voiceclient->send_audio_opus(packet->op.packet, packet->op.bytes);

            /* make sure to always return 0 here, read more on oggz documentation */
            return 0;
        },
        /* this will be the value of void *user_data */
        (void *)v
    );

    // read loop
    while (v && v->voiceclient && !v->voiceclient->terminating) {
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

    v->voiceclient->insert_marker(music.fileName + " end");

    std::cout << "audio sending complete\n";
}

void MusicQueue::play()
{
    if (!playMutex.try_lock())
    {
        std::cout << "Already playing\n";
        return;
    }

    dpp::discord_client* joinedShard = botCluster->get_shard(id.shard_id);
    if (!joinedShard)
    {
        std::cout << "No shard\n";
        playMutex.unlock();
        return;
    }

    dpp::voiceconn* v = joinedShard->get_voice(id.guild_id);
    if (!v || !v->voiceclient || !v->voiceclient->is_ready())
    {
        std::cout << "not in voicechat. quit musicplay";
        playMutex.unlock();
        return;
    }
    
    markerCallback();
}
