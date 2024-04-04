/*
 *  Trade secret of Alibaba Group R&D.
 *  Copyright (c) 2010 Alibaba Group R&D. (unpublished)
 *
 *  All rights reserved.  This notice is intended as a precaution against
 *  inadvertent publication and does not imply publication or any waiver
 *  of confidentiality.  The year included in the foregoing notice is the
 *  year of creation of the work.
 *
 */
#include "client.h"
#include "common.h"
#include <string>
#include <iostream>
#include <thread>
#include <chrono>
#include <memory>

using namespace aliyun_log_sdk_v6;
using namespace std;
int main(int argc, char **argv)
{
    string endpoint = "cn-hangzhou-corp.sls.aliyuncs.com";
    string accessKeyId = "";
    string accessKey = "";
    string project = "";
    string logstore = "";
    string topic = "";
    auto clientPtr = make_shared<LOGClient>(endpoint, accessKeyId, accessKey, LOG_REQUEST_TIMEOUT, "127.0.0.1", false);
    vector<LogItem> logGroup;
    for (int i = 0; i < 2; ++i)
    {
        logGroup.push_back(LogItem());
        LogItem &item = logGroup.back();
        item.timestamp = time(NULL);
        item.source = "127.0.0.1";
        item.topic = topic;
        item.data.push_back({"status", "200"});
        item.data.push_back({"latency", "126"});
    }
    try
    {
        // ptr-> CreateConsumerGroup(project, logstore, "hahhah", 10, true);
        clientPtr->DeleteConsumerGroup(project, logstore, "test-consumer-group");
        uint32_t shardId = 2;
        auto beginTime = time(NULL);
        clientPtr->PostLogStoreLogs(project, logstore, topic, logGroup);
        this_thread::sleep_for(chrono::seconds(1));
        clientPtr->PostLogStoreLogs(project, logstore, topic, logGroup);
        this_thread::sleep_for(chrono::seconds(1));
        auto endTime = time(NULL);
        clientPtr->PostLogStoreLogs(project, logstore, topic, logGroup);
        this_thread::sleep_for(chrono::seconds(1));
        clientPtr->PostLogStoreLogs(project, logstore, topic, logGroup);
        this_thread::sleep_for(chrono::seconds(1));
        GetCursorResponse beginCursorResp = clientPtr->GetCursor(project, logstore, shardId, beginTime);
        GetCursorResponse endCursorResp = clientPtr->GetCursor(project, logstore, shardId, endTime);
        GetBatchLogResponse getLogResp = clientPtr->GetBatchLog(project, logstore, shardId, 1000, beginCursorResp.result);
        cout << getLogResp.result.logGroups.size() << "\t" << getLogResp.result.logGroupCount << endl;
        getLogResp = clientPtr->GetBatchLog(project, logstore, 2, 1000, beginCursorResp.result, endCursorResp.result);
        cout << getLogResp.result.logGroups.size() << "\t" << getLogResp.result.logGroupCount << endl;

        ListConsumerGroupResponse lrs = clientPtr->ListConsumerGroup(project, logstore);
        for (size_t i = 0; i < lrs.consumerGroups.size(); ++i)
            cout << lrs.consumerGroups[i].GetConsumerGroupName() << ", " << lrs.consumerGroups[i].GetTimeoutInSec() << ", " << lrs.consumerGroups[i].GetInOrder() << endl;
        clientPtr->UpdateCheckpoint(project, logstore, "hahhah", 0, "V0hBVFRIRlVDSw==");
        ListCheckpointResponse lcps = clientPtr->ListCheckpoint(project, logstore, "hahhah");
        for (size_t i = 0; i < lcps.consumerGroupCheckpoints.size(); ++i)
        {
            cout << lcps.consumerGroupCheckpoints[i].GetShard() << ", " << lcps.consumerGroupCheckpoints[i].GetCheckpoint() << ", " << lcps.consumerGroupCheckpoints[i].GetUpdateTime() << endl;
        }
        while (true)
        {
            HeartbeatResponse resp = clientPtr->ConsumerGroupHeartbeat(project, logstore, "hahhah", "cc1", std::vector<uint32_t>());
            cout << "cc1, heartbeat: ";
            for (std::vector<uint32_t>::const_iterator it = resp.shards.begin(); it != resp.shards.end(); ++it)
                cout << *it << ", ";
            cout << endl;
            resp = clientPtr->ConsumerGroupHeartbeat(project, logstore, "hahhah", "cc2", std::vector<uint32_t>());
            cout << "cc2, heartbeat: ";
            for (std::vector<uint32_t>::const_iterator it = resp.shards.begin(); it != resp.shards.end(); ++it)
                cout << *it << ", ";
            cout << endl;
            this_thread::sleep_for(chrono::seconds(2));
        }
    }
    catch (LOGException &e)
    {
        cout << e.GetErrorCode() << ":" << e.GetMessage() << endl;
    }
}
