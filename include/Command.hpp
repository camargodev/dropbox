#ifndef COMMAND_HPP
#define COMMAND_HPP

using Command = int;

const Command UPLOAD_FILE          = 1;
const Command IDENTIFICATION       = 2;
const Command DISCONNECT           = 3;
const Command SIMPLE_MESSAGE       = 4;
const Command DOWNLOAD_REQUISITION = 5;
const Command DOWNLOADED_FILE      = 6;
const Command ERROR_MESSAGE        = 7;
const Command DELETE_REQUISITION   = 8;
const Command DELETE_ORDER         = 9;
const Command FILE_LISTING         = 10;
const Command LIST_REQUISITION     = 11;

#endif
