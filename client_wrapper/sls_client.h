#pragma once

#include <thread>
#include <mutex>
#include <memory>
#include <deque>
#include <functional>
#include <unistd.h>
#include <vector>
#include <unordered_map>
#include "../client.h"
#include "../common.h"

namespace aliyun_log_sdk_v6 {

struct SlsConf {
  SlsConf(): logBatchSize(100) {}

  std::string endPoint;
  std::string accessId;
  std::string accessKey;
  std::string project;
  std::string logStore;
  std::string consumerGroup;
  int logBatchSize;
};

class SlsClient {
public:
  SlsClient(): _running(false) {}

  // use std::bind to generate callback function
  void init(const SlsConf& config, std::function<void(GetBatchLogResponse)> callbackFunc);

  void start();
  void stop();

private:
  void loopRun();
  bool isShardRebalanced(const std::vector<uint32_t>& newShards, const std::unordered_map<uint32_t, std::string>& oldShards);

private:
  std::string _hostSign;
  SlsConf _slsConf;
  std::shared_ptr<aliyun_log_sdk_v6::LOGClient> _slsClient;
  std::function<void(GetBatchLogResponse)> _callbackFunc;

  bool _running;
  std::thread _loopThread;
};

}
