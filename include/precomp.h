#ifndef BUMBLEBEE_INCLUDE_PRECOMP_H_
#define BUMBLEBEE_INCLUDE_PRECOMP_H_
#ifdef WIN32
#include <winsock2.h>
#endif

#include <dpp/dpp.h>

#include <iostream>
#include <queue>

#include "boost/asio.hpp"
#include "boost/beast/http.hpp"
#include "boost/beast/ssl.hpp"
#include "boost/log/trivial.hpp"
#include "boost/process.hpp"

extern "C" {
#include "libavformat/avformat.h"
#include "libavutil/avutil.h"
#include "libswresample/swresample.h"
#include "libswscale/swscale.h"
}

#endif  // BUMBLEBEE_INCLUDE_PRECOMP_H_