#pragma once
#include <dpp/dpp.h>
#include <Queue/MusicQueueElement.hpp>
#include <cmath>

namespace bumbleBee {
class QueuedMusicListEmbedProvider {
public:
    static std::queue<dpp::embed> makeEmbed(std::shared_ptr<std::list<std::shared_ptr<MusicQueueElement>>> queue, std::list<std::shared_ptr<MusicQueueElement>>::iterator np, bool repeat) {
        std::queue<dpp::embed> returnValue;
        std::list<std::shared_ptr<MusicQueueElement>>::iterator it = queue->begin();
        if (queue->size() == 0) {
            dpp::embed embed = makeEmbedPart(queue, np, repeat, it);
            returnValue.push(embed);
            return returnValue;
        }
        for (int i = 0; i < std::ceil(queue->size() / 5.0) && it != queue->end(); i++) {
            dpp::embed embed = makeEmbedPart(queue, np, repeat, it);
            returnValue.push(embed);
        }
        return returnValue;
    }
    
private:
    static dpp::embed makeEmbedPart(
        std::shared_ptr<std::list<std::shared_ptr<MusicQueueElement>>> queue,
        std::list<std::shared_ptr<MusicQueueElement>>::iterator np,
        bool repeat,
        std::list<std::shared_ptr<MusicQueueElement>>::iterator &startIter) {
        dpp::embed embed = dpp::embed()
            .set_color(dpp::colors::sti_blue);

        if (queue->begin() == queue->end()) {
            embed
                .set_title("큐가 비었습니다!")
                .set_timestamp(time(0));

            if (repeat)
                embed.add_field(":repeat:","");

            return embed;
        }

        int startIndex = std::distance(queue->begin(), startIter) + 1;
        int index = startIndex;

        for (; (index < startIndex + 5) && startIter != queue->end() && index < queue->size()+1; startIter++, index++) { //iter로 순회하면 왠지 모르게 이상한 값을 읽을 때가 있음 이유는 나도 몰?루
            if (*startIter == *np)
                embed.add_field (
                    "np",
                    "",
                    true
                );
            else
                embed.add_field (
                    std::to_string(index),
                    "",
                    true
                );
            embed.add_field (
                (*startIter)->embed.title,
                (*startIter)->embed.description,
                true
            )
            .add_field("","");
        }

        if (startIter == queue->end() || index >= queue->size()+1) {
            embed.set_timestamp(time(0));
            if (repeat)
                embed.add_field(":repeat:","");
        }

        return embed;
    }
};
}