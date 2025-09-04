#include "precomp.h"

#define OPUS_FRAME_SIZE 960  // 20ms @ 48kHz

int main() {
  const char* input_filename = "golden.webm";
  const char* output_filename = "output.opus";

  AVFormatContext* fmt_ctx = NULL;
  AVCodecContext* dec_ctx = NULL;
  AVCodecContext* enc_ctx = NULL;
  const AVCodec* decoder = NULL;
  const AVCodec* encoder = NULL;
  AVPacket* packet = NULL;
  AVFrame* frame = NULL;
  AVFrame* enc_frame = NULL;
  SwrContext* swr_ctx = NULL;
  FILE* outfile = NULL;

  av_log_set_level(AV_LOG_ERROR);

  if (avformat_open_input(&fmt_ctx, input_filename, NULL, NULL) < 0) {
    fprintf(stderr, "Could not open input file\n");
    return -1;
  }
  if (avformat_find_stream_info(fmt_ctx, NULL) < 0) {
    fprintf(stderr, "Could not find stream info\n");
    return -1;
  }

  int stream_index = -1;
  for (unsigned i = 0; i < fmt_ctx->nb_streams; i++) {
    if (fmt_ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
      stream_index = i;
      break;
    }
  }
  if (stream_index == -1) {
    fprintf(stderr, "No audio stream found\n");
    return -1;
  }

  decoder =
      avcodec_find_decoder(fmt_ctx->streams[stream_index]->codecpar->codec_id);
  if (!decoder) {
    fprintf(stderr, "Decoder not found\n");
    return -1;
  }
  dec_ctx = avcodec_alloc_context3(decoder);
  avcodec_parameters_to_context(dec_ctx,
                                fmt_ctx->streams[stream_index]->codecpar);
  avcodec_open2(dec_ctx, decoder, NULL);

  encoder = avcodec_find_encoder(AV_CODEC_ID_OPUS);
  if (!encoder) {
    fprintf(stderr, "Opus encoder not found\n");
    return -1;
  }
  enc_ctx = avcodec_alloc_context3(encoder);

  AVChannelLayout enc_layout;
  av_channel_layout_default(&enc_layout, 2);  // 스테레오
  av_channel_layout_copy(&enc_ctx->ch_layout, &enc_layout);

  enc_ctx->sample_rate = 48000;
  enc_ctx->sample_fmt = AV_SAMPLE_FMT_FLT;
  enc_ctx->bit_rate = 128000;

  avcodec_open2(enc_ctx, encoder, NULL);

  swr_ctx = NULL;
  if (swr_alloc_set_opts2(&swr_ctx, &enc_ctx->ch_layout, enc_ctx->sample_fmt,
                          enc_ctx->sample_rate, &dec_ctx->ch_layout,
                          dec_ctx->sample_fmt, dec_ctx->sample_rate, 0,
                          NULL) < 0) {
    fprintf(stderr, "Failed to allocate SwrContext\n");
    return -1;
  }
  swr_init(swr_ctx);

  packet = av_packet_alloc();
  frame = av_frame_alloc();
  enc_frame = av_frame_alloc();

  outfile = fopen(output_filename, "wb");
  if (!outfile) {
    fprintf(stderr, "Could not open output file\n");
    return -1;
  }

  // 임시 PCM 버퍼 (float, 스테레오)
  float* pcm_buffer = (float*)malloc(sizeof(float) * 2 * OPUS_FRAME_SIZE *
                                     4);  // 충분히 큰 버퍼
  int buffered_samples = 0;

  while (av_read_frame(fmt_ctx, packet) >= 0) {
    if (packet->stream_index != stream_index) {
      av_packet_unref(packet);
      continue;
    }

    avcodec_send_packet(dec_ctx, packet);
    while (avcodec_receive_frame(dec_ctx, frame) == 0) {
      int max_out = av_rescale_rnd(
          swr_get_delay(swr_ctx, dec_ctx->sample_rate) + frame->nb_samples,
          enc_ctx->sample_rate, dec_ctx->sample_rate, AV_ROUND_UP);

      uint8_t** out_data = NULL;
      int out_linesize = 0;
      av_samples_alloc_array_and_samples(&out_data, &out_linesize, 2, max_out,
                                         enc_ctx->sample_fmt, 0);

      int converted =
          swr_convert(swr_ctx, out_data, max_out, (const uint8_t**)frame->data,
                      frame->nb_samples);

      // float PCM으로 임시 버퍼에 추가
      memcpy(pcm_buffer + buffered_samples * 2, out_data[0],
             converted * 2 * sizeof(float));
      buffered_samples += converted;

      av_freep(&out_data[0]);
      free(out_data);

      // OPUS_FRAME_SIZE 단위로 인코딩
      while (buffered_samples >= OPUS_FRAME_SIZE) {
        enc_frame->nb_samples = OPUS_FRAME_SIZE;
        enc_frame->format = enc_ctx->sample_fmt;
        enc_frame->sample_rate = enc_ctx->sample_rate;
        av_channel_layout_copy(&enc_frame->ch_layout, &enc_ctx->ch_layout);
        enc_frame->data[0] = (uint8_t*)pcm_buffer;

        AVPacket* out_pkt = av_packet_alloc();
        avcodec_send_frame(enc_ctx, enc_frame);
        while (avcodec_receive_packet(enc_ctx, out_pkt) == 0) {
          fwrite(out_pkt->data, 1, out_pkt->size, outfile);
          av_packet_unref(out_pkt);
        }
        av_packet_free(&out_pkt);

        // 버퍼 이동
        memmove(pcm_buffer, pcm_buffer + OPUS_FRAME_SIZE * 2,
                (buffered_samples - OPUS_FRAME_SIZE) * 2 * sizeof(float));
        buffered_samples -= OPUS_FRAME_SIZE;
      }
    }
    av_packet_unref(packet);
  }

  // 디코더 플러시
  avcodec_send_packet(dec_ctx, NULL);
  while (avcodec_receive_frame(dec_ctx, frame) == 0) {
    int max_out = av_rescale_rnd(
        swr_get_delay(swr_ctx, dec_ctx->sample_rate) + frame->nb_samples,
        enc_ctx->sample_rate, dec_ctx->sample_rate, AV_ROUND_UP);

    uint8_t** out_data = NULL;
    int out_linesize = 0;
    av_samples_alloc_array_and_samples(&out_data, &out_linesize, 2, max_out,
                                       enc_ctx->sample_fmt, 0);

    int converted =
        swr_convert(swr_ctx, out_data, max_out, (const uint8_t**)frame->data,
                    frame->nb_samples);

    memcpy(pcm_buffer + buffered_samples * 2, out_data[0],
           converted * 2 * sizeof(float));
    buffered_samples += converted;

    av_freep(&out_data[0]);
    free(out_data);

    while (buffered_samples >= OPUS_FRAME_SIZE) {
      enc_frame->nb_samples = OPUS_FRAME_SIZE;
      enc_frame->format = enc_ctx->sample_fmt;
      enc_frame->sample_rate = enc_ctx->sample_rate;
      av_channel_layout_copy(&enc_frame->ch_layout, &enc_ctx->ch_layout);
      enc_frame->data[0] = (uint8_t*)pcm_buffer;

      AVPacket* out_pkt = av_packet_alloc();
      avcodec_send_frame(enc_ctx, enc_frame);
      while (avcodec_receive_packet(enc_ctx, out_pkt) == 0) {
        fwrite(out_pkt->data, 1, out_pkt->size, outfile);
        av_packet_unref(out_pkt);
      }
      av_packet_free(&out_pkt);

      memmove(pcm_buffer, pcm_buffer + OPUS_FRAME_SIZE * 2,
              (buffered_samples - OPUS_FRAME_SIZE) * 2 * sizeof(float));
      buffered_samples -= OPUS_FRAME_SIZE;
    }
  }

  // 마지막 남은 샘플 인코딩
  if (buffered_samples > 0) {
    enc_frame->nb_samples = buffered_samples;
    enc_frame->format = enc_ctx->sample_fmt;
    enc_frame->sample_rate = enc_ctx->sample_rate;
    av_channel_layout_copy(&enc_frame->ch_layout, &enc_ctx->ch_layout);
    enc_frame->data[0] = (uint8_t*)pcm_buffer;

    AVPacket* out_pkt = av_packet_alloc();
    avcodec_send_frame(enc_ctx, enc_frame);
    while (avcodec_receive_packet(enc_ctx, out_pkt) == 0) {
      fwrite(out_pkt->data, 1, out_pkt->size, outfile);
      av_packet_unref(out_pkt);
    }
    av_packet_free(&out_pkt);
  }

  // 인코더 플러시
  avcodec_send_frame(enc_ctx, NULL);
  AVPacket* out_pkt = av_packet_alloc();
  while (avcodec_receive_packet(enc_ctx, out_pkt) == 0) {
    fwrite(out_pkt->data, 1, out_pkt->size, outfile);
    av_packet_unref(out_pkt);
  }
  av_packet_free(&out_pkt);

  fclose(outfile);
  free(pcm_buffer);
  swr_free(&swr_ctx);
  av_frame_free(&frame);
  av_frame_free(&enc_frame);
  av_packet_free(&packet);
  avcodec_free_context(&dec_ctx);
  avcodec_free_context(&enc_ctx);
  avformat_close_input(&fmt_ctx);

  printf("Encoding finished: %s\n", output_filename);
  return 0;
}