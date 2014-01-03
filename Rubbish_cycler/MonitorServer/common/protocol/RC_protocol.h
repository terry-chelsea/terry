/*
 * =====================================================================================
 *
 *       Filename:  Rubbish_recycler.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  08/05/13 16:56:07
 *       Revision:  none
 *       Compiler:  gcc
 *
 * =====================================================================================
 */

#ifndef _H_RUBBISH_RECYCLER_H_
#define _H_RUBBISH_RECYCLER_H_

//for recycler protocol , use text string(use string not int to indicate interge)...
struct RCHeader
{
    uint32_t cmd;
    uint32_t length;
    uint32_t error;
};

//scale and error rate...
const uint32_t MSG_MPC_CS_START_RUBBISH_RECYCLER = 0x08000000;
//just ack to tells the rubbish recycler will start right now...
const uint32_t MSG_MPC_CS_START_RUBBISH_RECYCLER_ACK = 0x08000001;
//send it when all works finish , recycler blocks...
const uint32_t MSG_MPC_CS_START_RUBBISH_RECYCLER_FINISH_ACK = 0x08000002;

//no load data...
const uint32_t MSG_MPC_CS_CANCLE_RUBBISH_RECYCLER = 0x08000010;
const uint32_t MSG_MPC_CS_CANCLE_RUBBISH_RECYCLER_ACK = 0x08000011;


////CS to MU...
//bucket nr , bitmap size , hash function ids , NFS dir , if do compress for bitmap ...
const uint32_t MSG_CS_MU_START_BUCKET = 0x08000020;
//flag , can this bucket recycler in this MU node ...
const uint32_t MSG_CS_MU_START_BUCKET_ACK = 0x08000021;

//just heartbeat , no load data...
const uint32_t MSG_CS_MU_HEARTBEAT = 0x08000030;
//file block counter ...
const uint32_t MSG_CS_MU_HEARTBEAT_ACK = 0x08000031;

const uint32_t MSG_CS_MU_CANCLE_BUCKET = 0x08000040;
const uint32_t MSG_CS_MU_CANCLE_BUCKET_ACK = 0x08000041;

const uint32_t MSG_CS_MU_STOP_BUCKET = 0x08000050;
const uint32_t MSG_CS_MU_STOP_BUCKET_ACK = 0x08000051;

const uint32_t MSG_CS_MU_CONTINUE_BUCKET = 0x08000060;
const uint32_t MSG_CS_MU_CONTINUE_BUCKET_ACK = 0x08000061;

//ack to get the lock to upload bitmap ...
const uint32_t MSG_MU_CS_ASK_FOR_UPLOAD_BITMAP = 0x08000070;
const uint32_t MSG_MU_CS_ASK_FOR_UPLOAD_BITMAP_ACK = 0x08000071;

//send this to get next bucket ...
const uint32_t MSG_MU_CS_UPLOAD_FINISH - 0x08000080;
const uint32_t MSG_MU_CS_UPLOAD_FINISH_ACK = 0x08000081;


////CS to SU...
//bucket nr , bitmap block size , hash function ids , NFS dir ...
const uint32_t MSG_CS_SU_START_BUCKET = 0x08000090;
//flag
const uint32_t MSG_CS_SU_START_BUCKET_ACK = 0x08000091;

//just heartbear ...
const uint32_t MSG_CS_SU_HEARTBEAT = 0x080000A0;
//database items counter ...
const uint32_t MSG_CS_SU_HEARTBEAT_ACK = 0x080000A1;

//SU to CS ...
//apply for next bucket ...
const uint32_t MSG_SU_CS_BUCKET_FINISH = 0x080000B0;
const uint32_t MSG_SU_CS_BUCKET_FINISH_ACK = 0x080000B1;

#endif
