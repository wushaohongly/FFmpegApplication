//
// Created by wushaohong on 2020/5/5.
//

#ifndef STUDYFFMPEG_ANDROIDLOG_H
#define STUDYFFMPEG_ANDROIDLOG_H

#include <android/log.h>

#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, "FFMPEG",__VA_ARGS__)

#endif //WUSHPLAYER_ANDROIDLOG_H
