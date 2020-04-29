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
#include "client_wrapper/sls_client.h"
#include <unistd.h>
#include <string>
#include <iostream>
using namespace aliyun_log_sdk_v6;
using namespace std;

void CallbackFunc(GetBatchLogResponse resp) {
  cout << "get batch log: " << resp.result.logGroups.size() << "\t" << resp.result.logGroupCount << endl;
}

int main(int argc,char ** argv)
{
  // input your own sls logstore config to run the client
  SlsConf conf;
  conf.endPoint = "cn-hangzhou-corp.sls.aliyuncs.com";
  conf.accessId = "";
  conf.accessKey = "";
  conf.project="";
  conf.logStore="";
  conf.consumerGroup = "test_consumer_group";

  SlsClient sls_client;
  sls_client.init(conf, &CallbackFunc);
  sls_client.start();
  sleep(60);
  sls_client.stop();
}
