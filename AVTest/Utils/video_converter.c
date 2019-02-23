//
//  video_converter.c
//  FFmpegTest
//
//  Created by apple on 2019/2/17.
//  Copyright © 2019年 com.hwb. All rights reserved.
//

#include "video_converter.h"
#include "libavformat/avformat.h"

void save_frame(AVFrame *frame, FILE *file);
void save_to_file(uint8_t *data, int linesize, int width, int height, FILE *file);

int convert2yuv(const char *src_path, const char * dst_path) {
    if (src_path == NULL || dst_path == NULL) {
        printf("invalide path\n");
        return -1;
    }
    
    FILE *file = NULL;
    int video_stream_index = AVERROR_STREAM_NOT_FOUND;
    int ret = 0;
    AVCodec *codec = NULL;
    AVCodecContext *codec_ctx = NULL;
    AVFrame *frame = NULL;
    AVPacket packet;
    
    //open dst file
    file = fopen(dst_path, "wb");
    
    //init ffmpeg
    av_register_all();
    avformat_network_init();
    
    //create avformatcontext
    AVFormatContext *ctx = avformat_alloc_context();
    avformat_open_input(&ctx, src_path, NULL, NULL);
    avformat_find_stream_info(ctx, NULL);
    
    //find video stream index
    video_stream_index = av_find_best_stream(ctx, AVMEDIA_TYPE_VIDEO, -1, -1, NULL, 0);
    if (video_stream_index == AVERROR_STREAM_NOT_FOUND) {
        printf("video stream not found!\n");
        goto CLEANUP;
    }
    
    //create codec contex
    codec_ctx = avcodec_alloc_context3(NULL);
    avcodec_parameters_to_context(codec_ctx, ctx->streams[video_stream_index]->codecpar);
    if (codec_ctx->pix_fmt != AV_PIX_FMT_YUV420P && codec_ctx->pix_fmt != AV_PIX_FMT_YUVJ420P) {
        printf("not support pix format\n");
        goto CLEANUP;
    }
    printf("pix format: %d\n", codec_ctx->pix_fmt);
    
    //find decoder and open it
    codec = avcodec_find_decoder(ctx->streams[video_stream_index]->codecpar->codec_id);
    if ((ret = avcodec_open2(codec_ctx, codec, NULL)) != 0) {
        printf("avcodec_open error: %d\n", ret);
        goto CLEANUP;
    }
    
    frame = av_frame_alloc();
    for (;;) {
        if (av_read_frame(ctx, &packet)) {
            //flush packet
            avcodec_send_packet(codec_ctx, NULL);
            while ((ret = avcodec_receive_frame(codec_ctx, frame)) == 0) {
                save_frame(frame, file);
            }
            
            printf("av_read_frame end\n");
            break;
        }
        
        if (packet.stream_index == video_stream_index) {
            if ((ret = avcodec_send_packet(codec_ctx, &packet)) != 0) {
                printf("send packet error: %d\n", ret);
                goto CLEANUP;
            }
            
            while ((ret = avcodec_receive_frame(codec_ctx, frame)) == 0) {
                save_frame(frame, file);
            }
            if (ret == AVERROR(EINVAL)) {
                printf("receive frame error!\n");
                goto CLEANUP;
            }
        }
    }
    
    ret = 0;
    
CLEANUP:
    av_packet_unref(&packet);
    av_frame_free(&frame);
    avcodec_free_context(&codec_ctx);
    avformat_close_input(&ctx);
    fclose(file);
    return ret;
}

void save_frame(AVFrame *frame, FILE *file) {
    save_to_file(frame->data[0], frame->linesize[0], frame->width, frame->height, file);
    save_to_file(frame->data[1], frame->linesize[1], frame->width * 0.5, frame->height * 0.5, file);
    save_to_file(frame->data[2], frame->linesize[2], frame->width * 0.5, frame->height * 0.5, file);
}

void save_to_file(uint8_t *data, int linesize, int width, int height, FILE *file) {
    width = linesize > width ? width : linesize;
    uint8_t *src = data;
    for (int i = 0; i < height; i++) {
        fwrite(src, sizeof(uint8_t), width, file);
        src += linesize;
    }
}
