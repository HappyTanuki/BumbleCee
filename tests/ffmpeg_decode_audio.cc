#include <fstream>

#include "ffmpeg/libavcodec.h"
#include "precomp.h"

int main() {
  const char* input_filename = "golden.webm";
  const char* output_filename = "output.pcm";

  AVFormatContext* fmt_ctx = NULL;
  AVCodecContext* codec_ctx = NULL;
  const AVCodec* codec = NULL;
  AVPacket* packet = NULL;
  AVFrame* frame = NULL;
  SwrContext* swr_ctx = NULL;
  FILE* outfile = NULL;

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
    fprintf(stderr, "Could not find audio stream\n");
    return -1;
  }

  codec =
      avcodec_find_decoder(fmt_ctx->streams[stream_index]->codecpar->codec_id);
  if (!codec) {
    fprintf(stderr, "Could not find decoder\n");
    return -1;
  }

  codec_ctx = avcodec_alloc_context3(codec);
  if (!codec_ctx) {
    fprintf(stderr, "Could not allocate codec context\n");
    return -1;
  }

  if (avcodec_parameters_to_context(
          codec_ctx, fmt_ctx->streams[stream_index]->codecpar) < 0) {
    fprintf(stderr, "Failed to copy codec parameters\n");
    return -1;
  }

  if (avcodec_open2(codec_ctx, codec, NULL) < 0) {
    fprintf(stderr, "Could not open codec\n");
    return -1;
  }

  packet = av_packet_alloc();
  frame = av_frame_alloc();
  if (!packet || !frame) {
    fprintf(stderr, "Could not allocate packet or frame\n");
    return -1;
  }

  outfile = fopen(output_filename, "wb");
  if (!outfile) {
    fprintf(stderr, "Could not open output file\n");
    return -1;
  }

  // AVChannelLayout 초기화
  AVChannelLayout in_layout, out_layout;
  if (av_channel_layout_copy(&in_layout, &codec_ctx->ch_layout) < 0) {
    fprintf(stderr, "Failed to copy channel layout\n");
    return -1;
  }
  av_channel_layout_default(&out_layout, 2);  // 스테레오

  swr_ctx = NULL;  // 먼저 NULL로 선언
  if (swr_alloc_set_opts2(&swr_ctx, &out_layout, AV_SAMPLE_FMT_S16, 48000,
                          &in_layout, codec_ctx->sample_fmt,
                          codec_ctx->sample_rate, 0, NULL) < 0) {
    fprintf(stderr, "Failed to allocate and set SwrContext\n");
    return -1;
  }

  if (swr_init(swr_ctx) < 0) {
    fprintf(stderr, "Failed to initialize SwrContext\n");
    return -1;
  }

  while (av_read_frame(fmt_ctx, packet) >= 0) {
    if (packet->stream_index == stream_index) {
      if (avcodec_send_packet(codec_ctx, packet) == 0) {
        while (avcodec_receive_frame(codec_ctx, frame) == 0) {
          int out_samples =
              av_rescale_rnd(swr_get_delay(swr_ctx, codec_ctx->sample_rate) +
                                 frame->nb_samples,
                             48000, codec_ctx->sample_rate, AV_ROUND_UP);

          uint8_t** out_buf = NULL;
          int out_linesize = 0;
          av_samples_alloc_array_and_samples(&out_buf, &out_linesize, 2,
                                             out_samples, AV_SAMPLE_FMT_S16, 0);

          int converted_samples =
              swr_convert(swr_ctx, out_buf, out_samples,
                          (const uint8_t**)frame->data, frame->nb_samples);

          fwrite(out_buf[0], 1, converted_samples * 2 * 2, outfile);
          av_freep(&out_buf[0]);
          free(out_buf);
        }
      }
    }
    av_packet_unref(packet);
  }

  // 디코더 플러시
  avcodec_send_packet(codec_ctx, NULL);
  while (avcodec_receive_frame(codec_ctx, frame) == 0) {
    int out_samples = av_rescale_rnd(
        swr_get_delay(swr_ctx, codec_ctx->sample_rate) + frame->nb_samples,
        48000, codec_ctx->sample_rate, AV_ROUND_UP);

    uint8_t** out_buf = NULL;
    int out_linesize = 0;
    av_samples_alloc_array_and_samples(&out_buf, &out_linesize, 2, out_samples,
                                       AV_SAMPLE_FMT_S16, 0);

    int converted_samples =
        swr_convert(swr_ctx, out_buf, out_samples, (const uint8_t**)frame->data,
                    frame->nb_samples);

    fwrite(out_buf[0], 1, converted_samples * 2 * 2, outfile);
    av_freep(&out_buf[0]);
    free(out_buf);
  }

  fclose(outfile);
  swr_free(&swr_ctx);
  av_frame_free(&frame);
  av_packet_free(&packet);
  avcodec_free_context(&codec_ctx);
  avformat_close_input(&fmt_ctx);

  printf("Decoding finished, output saved to %s\n", output_filename);
  return 0;
}