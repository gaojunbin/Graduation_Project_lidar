/*********************************************************
* @file    main.c
* @author  Junbin Gao
* @data    2019.12
* @version v1.0.2
* @notes   use code to get cloud datas and save as csv files by batch
*********************************************************/

#include <stdio.h>
#include <stdlib.h>
#ifdef WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif
#include <string.h>
#include <apr_general.h>
#include <apr_getopt.h>
#include "livox_sdk.h"

#define NeedPointNumber 50000  // the number of points you need in each image
#define NeedFigureNumber 100   // the number of images you need

#define CSV_FILE true          // is to need save as csv files or only read in terminal

typedef enum
{
  kDeviceStateDisconnect = 0,
  kDeviceStateConnect = 1,
  kDeviceStateSampling = 2,
} DeviceState;

typedef struct
{
  uint8_t handle;
  DeviceState device_state;
  DeviceInfo info;
} DeviceItem;

DeviceItem devices[kMaxLidarCount];
uint32_t data_recveive_count[kMaxLidarCount];

/** Connect all the broadcast device. */
int lidar_count = 0;
char broadcast_code_list[kMaxLidarCount][kBroadcastCodeSize];

int fig_num = 0;

/** Connect the broadcast device in list, please input the broadcast code and modify the BROADCAST_CODE_LIST_SIZE. */
/*#define BROADCAST_CODE_LIST_SIZE  3
int lidar_count = BROADCAST_CODE_LIST_SIZE;
char broadcast_code_list[kMaxLidarCount][kBroadcastCodeSize] = {
  "000000000000002",
  "000000000000003",
  "000000000000004"
};*/

/** creat new file function Start */
uint8_t New_File(char *name, FILE *fp)
{
  fp = fopen(name, "w");
  fprintf(fp, "pointnumber,timestamp,handle,x,y,z,reflectivity\n");
  //  printf("creat new file successfully！");
  fclose(fp);
  return 0;
}
/** creat new file function End */

uint32_t ExistPointNumber  = 0;  //the number of points in current image you have already obtained 
uint32_t ExistFigureNumber = 0;  //the number of images you have already obtained

bool CompleteReceive = false;    //the flag of is completing receive or not

char file_name[30];   //to save the name of the files we will create
FILE *filepoint;      
/** Receiving point cloud data from Livox LiDAR. */
/* @func_name:  GetLidarData
 * @note:       core function to get the cloud data
 * @param:
 * handle:      the flag of lidar ID (0，1，2)
 * data：       the datas of we need
 * data_num：   unknow
 * client_data：unknow
*/
void GetLidarData(uint8_t handle, LivoxEthPacket *data, uint32_t data_num, void *client_data)
{
	
  static uint64_t cur_timestamp = 0; //save the timestamp to make sure the consistency in one image
  static uint64_t RunFlag = 0;       //is GetLidarData() function running correctly

  RunFlag++;

  if(RunFlag==10000)
  {
    printf("I am run!\n");
    RunFlag=0;
  }

  if (data) //exist data
  {
    if (ExistFigureNumber < NeedFigureNumber) //images we have got is not enough
    {
      LivoxRawPoint *p_point_data = (LivoxRawPoint *)data->data;
      int x = (*p_point_data).x;
      int y = (*p_point_data).y;
      int z = (*p_point_data).z;
      int r = (*p_point_data).reflectivity; //temporary saving datas

      //if ((y < 1887) && (y > -1743) && (z > -993) && (z < 2054)) //we can choose areas to get 
      if (1) //we can also get all datas that the lidar giving
      {
        if (ExistPointNumber == 0) //if current data is the first point in the image, then creat new file
        {
          cur_timestamp = *((uint64_t *)(data->timestamp));  //refresh current timestamp
          if (CSV_FILE == true)
          {
            sprintf(file_name, "CloudData_%d.csv", ExistFigureNumber + 1);
            if (New_File(file_name, filepoint) == 0)
            {
              printf("CloudData_%d.csv is created\n", ExistFigureNumber + 1);
              filepoint = fopen(file_name, "a");
            }
          }
        }

        if (ExistPointNumber < NeedPointNumber) //points in current image we have got is not enough
        {
          ExistPointNumber = ExistPointNumber + 1;
          if (CSV_FILE == true)
          {
            if (filepoint == NULL)         
            {
              printf("the file can not found\n");
              exit(0);
            }
            fprintf(filepoint, "%d,%ld,%d,%d,%d,%d,%d\n", ExistPointNumber, cur_timestamp, handle, x, y, z, r); //!!!!!尤其注意这里要用逗号隔开，因为excel表里面就默认识别逗号隔开的才能分类fprintf（文件指针，格式字符串，列表）
          }
          else
          {
            printf("the data is：%d,%ld,%d,%d,%d,%d,%d\n", ExistPointNumber, cur_timestamp, handle, x, y, z, r);
          }
          
          if (ExistPointNumber % 10000 == 0)
          {
            printf("collecting……(%d)\n",ExistPointNumber);
          }
        }
        else 
        {
          if (CSV_FILE == true)
          {
	      fclose(filepoint);
          }
          ExistFigureNumber = ExistFigureNumber + 1;
          printf("the %dst images is done，gets:%d points\n", ExistFigureNumber, ExistPointNumber);
          ExistPointNumber = 0;
        }
      }
    }
    else
    {
      CompleteReceive = true;
      printf("successful,thanks for using!\n");
    }
  }
}

/** Callback function of starting sampling. */
void OnSampleCallback(uint8_t status, uint8_t handle, uint8_t response, void *data)
{
  printf("OnSampleCallback statue %d handle %d response %d \n", status, handle, response);
  if (status == kStatusSuccess)
  {
    if (response != 0)
    {
      devices[handle].device_state = kDeviceStateConnect;
    }
  }
  else if (status == kStatusTimeout)
  {
    devices[handle].device_state = kDeviceStateConnect;
  }
}

/** Callback function of stopping sampling. */
void OnStopSampleCallback(uint8_t status, uint8_t handle, uint8_t response, void *data)
{
}

/** Query the firmware version of Livox LiDAR. */
void OnDeviceInformation(uint8_t status, uint8_t handle, DeviceInformationResponse *ack, void *data)
{
  if (status != kStatusSuccess)
  {
    printf("Device Query Informations Failed %d\n", status);
  }
  if (ack)
  {
    printf("firm ver: %d.%d.%d.%d\n",
           ack->firmware_version[0],
           ack->firmware_version[1],
           ack->firmware_version[2],
           ack->firmware_version[3]);
  }
}

/** Callback function of changing of device state. */
void OnDeviceChange(const DeviceInfo *info, DeviceEvent type)
{
  if (info == NULL)
  {
    return;
  }
  printf("OnDeviceChange broadcast code %s update type %d\n", info->broadcast_code, type);
  uint8_t handle = info->handle;
  if (handle >= kMaxLidarCount)
  {
    return;
  }
  if (type == kEventConnect)
  {
    QueryDeviceInformation(handle, OnDeviceInformation, NULL);
    if (devices[handle].device_state == kDeviceStateDisconnect)
    {
      devices[handle].device_state = kDeviceStateConnect;
      devices[handle].info = *info;
    }
  }
  else if (type == kEventDisconnect)
  {
    devices[handle].device_state = kDeviceStateDisconnect;
  }
  else if (type == kEventStateChange)
  {
    devices[handle].info = *info;
  }

  if (devices[handle].device_state == kDeviceStateConnect)
  {
    printf("Device State error_code %d\n", devices[handle].info.status.status_code);
    printf("Device State working state %d\n", devices[handle].info.state);
    printf("Device feature %d\n", devices[handle].info.feature);
    if (devices[handle].info.state == kLidarStateNormal)
    {
      if (devices[handle].info.type == kDeviceTypeHub)
      {
        HubStartSampling(OnSampleCallback, NULL);
      }
      else
      {
        LidarStartSampling(handle, OnSampleCallback, NULL);
      }
      devices[handle].device_state = kDeviceStateSampling;
    }
  }
}

/** Callback function when broadcast message received.
 * You need to add listening device broadcast code and set the point cloud data callback in this function.
 */
void OnDeviceBroadcast(const BroadcastDeviceInfo *info)
{
  if (info == NULL)
  {
    return;
  }

  printf("Receive Broadcast Code %s\n", info->broadcast_code);

  if (lidar_count > 0)
  {
    bool found = false;
    int i = 0;
    for (i = 0; i < lidar_count; ++i)
    {
      if (strncmp(info->broadcast_code, broadcast_code_list[i], kBroadcastCodeSize) == 0)
      {
        found = true;
        break;
      }
    }
    if (!found)
    {
      return;
    }
  }

  bool result = false;
  uint8_t handle = 0;
  result = AddLidarToConnect(info->broadcast_code, &handle);
  if (result == kStatusSuccess)
  {
    /** Set the point cloud data for a specific Livox LiDAR. */
    SetDataCallback(handle, GetLidarData, NULL);
    devices[handle].handle = handle;
    devices[handle].device_state = kDeviceStateDisconnect;
  }
}

/** Set the program options.
* You can input the registered device broadcast code and decide whether to save the log file.
*/
int SetProgramOption(int argc, const char *argv[])
{
  apr_status_t rv;
  apr_pool_t *mp = NULL;
  static const apr_getopt_option_t opt_option[] = {
      /** Long-option, short-option, has-arg flag, description */
      {"code", 'c', 1, "Register device broadcast code"},
      {"log", 'l', 0, "Save the log file"},
      {"help", 'h', 0, "Show help"},
      {NULL, 0, 0, NULL},
  };
  apr_getopt_t *opt = NULL;
  int optch = 0;
  const char *optarg = NULL;

  if (apr_initialize() != APR_SUCCESS)
  {
    return -1;
  }

  if (apr_pool_create(&mp, NULL) != APR_SUCCESS)
  {
    return -1;
  }

  rv = apr_getopt_init(&opt, mp, argc, argv);
  if (rv != APR_SUCCESS)
  {
    printf("Program options initialization failed.\n");
    return -1;
  }

  /** Parse the all options based on opt_option[] */
  bool is_help = false;
  while ((rv = apr_getopt_long(opt, opt_option, &optch, &optarg)) == APR_SUCCESS)
  {
    switch (optch)
    {
    case 'c':
      printf("Register broadcast code: %s\n", optarg);
      char *sn_list = (char *)malloc(sizeof(char) * (strlen(optarg) + 1));
      strncpy(sn_list, optarg, sizeof(char) * (strlen(optarg) + 1));
      char *sn_list_head = sn_list;
      sn_list = strtok(sn_list, "&");
      int i = 0;
      while (sn_list)
      {
        strncpy(broadcast_code_list[i], sn_list, kBroadcastCodeSize);
        sn_list = strtok(NULL, "&");
        i++;
      }
      lidar_count = i;
      free(sn_list_head);
      sn_list_head = NULL;
      break;
    case 'l':
      printf("Save the log file.\n");
      SaveLoggerFile();
      break;
    case 'h':
      printf(
          " [-c] Register device broadcast code\n"
          " [-l] Save the log file\n"
          " [-h] Show help\n");
      is_help = true;
      break;
    }
  }
  if (rv != APR_EOF)
  {
    printf("Invalid options.\n");
  }

  apr_pool_destroy(mp);
  mp = NULL;
  apr_terminate();
  if (is_help)
    return 1;
  return 0;
}

int main(int argc, const char *argv[])
{
  /** Set the program options. */
  if (SetProgramOption(argc, argv))
    return 0;

  printf("Livox SDK initializing.\n");
  /** Initialize Livox-SDK. */
  if (!Init())
  {
    return -1;
  }
  printf("Livox SDK has been initialized.\n");

  LivoxSdkVersion _sdkversion;
  GetLivoxSdkVersion(&_sdkversion);
  printf("Livox SDK version %d.%d.%d .\n", _sdkversion.major, _sdkversion.minor, _sdkversion.patch);

  memset(devices, 0, sizeof(devices));
  memset(data_recveive_count, 0, sizeof(data_recveive_count));

  /** Set the callback function receiving broadcast message from Livox LiDAR. */
  SetBroadcastCallback(OnDeviceBroadcast);

  /** Set the callback function called when device state change,
 * which means connection/disconnection and changing of LiDAR state.
 */
  SetDeviceStateUpdateCallback(OnDeviceChange);

  /** Start the device discovering routine. */
  if (!Start())
  {
    Uninit();
    return -1;
  }
  printf("Start discovering device.\n");

  while (CompleteReceive==false);


  int i = 0;
  for (i = 0; i < kMaxLidarCount; ++i)
  {
    if (devices[i].device_state == kDeviceStateSampling)
    {
      /** Stop the sampling of Livox LiDAR. */
      LidarStopSampling(devices[i].handle, OnStopSampleCallback, NULL);
    }
  }

  /** Uninitialize Livox-SDK. */
  Uninit();
}
