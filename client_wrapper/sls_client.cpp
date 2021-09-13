#include <stdio.h>
#include "sls_client.h"

namespace aliyun_log_sdk_v6 {

void SlsClient::init(const SlsConf& config, std::function<void(GetBatchLogResponse)> callbackFunc) {
  _slsConf = config;
  _callbackFunc = callbackFunc;

  char hostname[HOST_NAME_MAX];
  gethostname(hostname, HOST_NAME_MAX);

  char pidStr[16];
  int pid = getpid();
  sprintf(pidStr, "%d", pid);

  _hostSign = std::string(hostname) + "_" + pidStr;

  _slsClient = std::make_shared<aliyun_log_sdk_v6::LOGClient>(_slsConf.endPoint, _slsConf.accessId, _slsConf.accessKey,
                                                              aliyun_log_sdk_v6::LOG_REQUEST_TIMEOUT, _hostSign, false);
}

void SlsClient::start() {
  _running = true;

  // create consumer group if not existed, throw exception if existed
  try {
    _slsClient->CreateConsumerGroup(_slsConf.project, _slsConf.logStore, _slsConf.consumerGroup, 10, true);
  } catch (aliyun_log_sdk_v6::LOGException & e) {
    std::cout << "exception:" << e.GetErrorCode().c_str() << " err msg:" << e.GetMessage().c_str() << std::endl;
  }

  _loopThread = std::thread(&SlsClient::loopRun, this);
}

void SlsClient::stop() {
  _running = false;
  _loopThread.join();
}

void SlsClient::loopRun() {
  // logstore shards & checkpoints
  std::unordered_map<uint32_t, std::string> shardCp;

  while (_running) {
    try {
      // register or refresh heartbeat and get shards
      HeartbeatResponse hbr = _slsClient->ConsumerGroupHeartbeat(_slsConf.project, _slsConf.logStore,
                                                                 _slsConf.consumerGroup,
                                                                 _hostSign, std::vector<uint32_t>());
      if (hbr.shards.size() == 0) {
        sleep(5);
        continue;
      }

      // check shards rebalance event
      if (isShardRebalanced(hbr.shards, shardCp)) {
        shardCp.clear();

        // set default consume offet to certain time
        time_t beginTime = time(NULL);
        // beginTime -= 86400 * 7;

        for (const auto& elem : hbr.shards) {
          GetCursorResponse cr = _slsClient->GetCursor(_slsConf.project, _slsConf.logStore, elem, beginTime);
          shardCp[elem] = cr.result;
        }

        // if consumer group has checkpoints, substitude default value
        ListCheckpointResponse lcps = _slsClient->ListCheckpoint(_slsConf.project, _slsConf.logStore, _slsConf.consumerGroup);
        for (const auto& elem : lcps.consumerGroupCheckpoints) {
          if (!elem.GetCheckpoint().empty() && shardCp.count(elem.GetShard())) {
            shardCp[elem.GetShard()] = elem.GetCheckpoint();
          }
        }
      }

      // consume log shard sequently
      for (const auto& pair : shardCp) {
        GetBatchLogResponse getLogResp = _slsClient->GetBatchLog(_slsConf.project, _slsConf.logStore, pair.first, _slsConf.logBatchSize, pair.second);

        if (getLogResp.result.logGroupCount == 0) {
          sleep(1);
          continue;
        }

        // callback
        _callbackFunc(getLogResp);

        // update checkpoint
        shardCp[pair.first] = getLogResp.result.nextCursor;
        _slsClient->UpdateCheckpoint(_slsConf.project, _slsConf.logStore,
                                     _slsConf.consumerGroup, pair.first, getLogResp.result.nextCursor);
      }
    } catch (aliyun_log_sdk_v6::LOGException & e) {
      std::cout << "exception:" << e.GetErrorCode().c_str() << " err msg:" << e.GetMessage().c_str() << std::endl;
      //sleep(5);
    }
  }
}

bool SlsClient::isShardRebalanced(const std::vector<uint32_t>& newShards,
                                  const std::unordered_map<uint32_t, std::string>& oldShards) {
  if (newShards.size() != oldShards.size()) return true;

  for (const auto& elem : newShards) {
    if (!oldShards.count(elem)) return true;
  }

  return false;
}

}
