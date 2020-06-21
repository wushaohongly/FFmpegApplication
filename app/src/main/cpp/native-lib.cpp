#include <jni.h>
#include <string>
#include "androidLog.h"

extern "C" {
#include "libavcodec/avcodec.h"
}

extern "C" JNIEXPORT jstring JNICALL
Java_com_wushaohong_ffmpegapplication_MainActivity_stringFromJNI(
        JNIEnv *env,
        jobject /* this */) {
    std::string hello = "avcodec_configuration()";
    return env->NewStringUTF(hello.c_str());
}

extern "C"
JNIEXPORT void JNICALL
Java_com_wushaohong_ffmpegapplication_MainActivity_decodeH264(JNIEnv *env, jobject thiz) {

    const int buffer_size = 4096;
    uint8_t buffer[buffer_size] = {0};
    uint8_t *cur_ptr;
    int cur_size;

    AVCodec *avCodec = nullptr;
    AVCodecContext *avCodecContext = nullptr;
    AVCodecParserContext *avCodecParserContext = nullptr;

    AVCodecID avCodecId = AV_CODEC_ID_H264;

    FILE *file_in = nullptr;
    FILE *file_out = nullptr;

    char filepath_in[] = "/storage/emulated/0/bigbuckbunny_480x272.h264";
    char filepath_out[] = "/storage/emulated/0/bigbuckbunny_480x272.yuv";

    AVPacket *avPacket;
    AVFrame *avFrame;

    avcodec_register_all();

    avCodec = avcodec_find_decoder(avCodecId);

    if (!avCodec) {
        LOGE("查找解码器失败");
        return;
    }

    avCodecContext = avcodec_alloc_context3(avCodec);

    if (!avCodecContext) {
        LOGE("创建解码解码器上下文失败");
        return;
    }

    avCodecParserContext = av_parser_init(avCodecId);

    if (!avCodecParserContext) {
        LOGE("创建解码解析器上下文失败");
        return;
    }

    // 打开解码器
    int ret = avcodec_open2(avCodecContext, avCodec, nullptr);

    if (ret != 0) {
        LOGE("打开解码器失败");
        return;
    }

    file_in = fopen(filepath_in, "rb");
    file_out = fopen(filepath_out, "wb");

    avFrame = av_frame_alloc();
    avPacket = av_packet_alloc();

    av_init_packet(avPacket);

    while (1) {

        cur_size = fread(buffer, 1, buffer_size, file_in);
        if (cur_size == 0) {
            break;
        }

        cur_ptr = buffer;

        while (cur_size > 0) {

            int len = av_parser_parse2(avCodecParserContext, avCodecContext,
                                       &avPacket->data, &avPacket->size,
                                       cur_ptr, cur_size,
                                       AV_NOPTS_VALUE, AV_NOPTS_VALUE, AV_NOPTS_VALUE);

            cur_ptr += len;
            cur_size -= len;

            if (avPacket->size == 0) {
                continue;
            }

            ret = avcodec_send_packet(avCodecContext, avPacket);

            if (ret != 0) {
                break;
            }

            ret = avcodec_receive_frame(avCodecContext, avFrame);
            if (ret != 0) {
                continue;
            }

            for (int i = 0; i < avFrame->height; i++) {
                fwrite(avFrame->data[0] + avFrame->linesize[0] * i, 1, avFrame->width, file_out);
            }
            for (int i = 0; i < avFrame->height / 2; i++) {
                fwrite(avFrame->data[1] + avFrame->linesize[1] * i, 1, avFrame->width / 2,
                       file_out);
            }
            for (int i = 0; i < avFrame->height / 2; i++) {
                fwrite(avFrame->data[2] + avFrame->linesize[2] * i, 1, avFrame->width / 2,
                       file_out);
            }
        }

    }

    fclose(file_in);
    fclose(file_out);

    av_parser_close(avCodecParserContext);
    av_frame_clone(avFrame);
    avcodec_close(avCodecContext);

    av_free(avCodecContext);

}