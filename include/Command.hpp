#ifndef COMMAND_HPP
#define COMMAND_HPP

using Command = int;

const Command UPLOAD_FILE             = 1;
const Command IDENTIFICATION          = 2;
const Command DISCONNECT              = 3;
const Command SIMPLE_MESSAGE          = 4;
const Command DOWNLOAD_REQUISITION    = 5;
const Command DOWNLOADED_FILE         = 6;
const Command ERROR_MESSAGE           = 7;
const Command DELETE_REQUISITION      = 8;
const Command DELETE_ORDER            = 9;
const Command FILE_LISTING            = 10;
const Command LIST_REQUISITION        = 11;
const Command GET_SYNC_DIR            = 1000;
const Command SYNC_FILE               = 2000;
const Command ASK_FOR_SYNC_FILE       = 3000;
const Command FILE_SYNCED             = 4000;
const Command ELECTION                = 10000;
const Command ANSWER                  = 20000;
const Command COORDINATOR             = 30000;
const Command IS_ALIVE                = 40000;
const Command IM_ALIVE                = 50000;
const Command TIMEOUT_TEST            = 60000;
const Command MIRROR                  = 12345;
const Command MIRROR_REPLICATION      = 54321;
const Command MIRROR_UPDATE           = 21354;
const Command NEW_SERVER              = 14051997;
#endif

