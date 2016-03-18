/*
 *  OpenVPN-GUI -- A Windows GUI for OpenVPN.
 *
 *  Copyright (C) 2004 Mathias Sundman <mathias@nilings.se>
 *                2010 Heiko Hund <heikoh@users.sf.net>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program (see the file COPYING included with this
 *  distribution); if not, write to the Free Software Foundation, Inc.,
 *  59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#ifndef OPTIONS_H
#define OPTIONS_H

typedef struct connection connection_t;

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <time.h>
#include <lmcons.h>

#include "manage.h"

#define MAX_NAME (UNLEN + 1)
#define MAX_HOSTNAME 256

/*
 * Maximum number of parameters associated with an option,
 * including the option name itself.
 */
#define MAX_PARMS           5   /* May number of parameters per option */

typedef enum {
    service_noaccess     = -1,
    service_disconnected =  0,
    service_connecting   =  1,
    service_connected    =  2
} service_state_t;

typedef enum {
    config,
    windows,
    manual
} proxy_source_t;

typedef enum {
    http,
    socks
} proxy_t;

/* connection states */
typedef enum {
    disconnected,
    connecting,
    reconnecting,
    connected,
    disconnecting,
    suspending,
    suspended,
    resuming,
    timedout,
    onhold
} conn_state_t;

/* Interactive Service IO parameters */
typedef struct {
    OVERLAPPED o; /* This has to be the first element */
    HANDLE pipe;
    HANDLE hEvent;
    WCHAR readbuf[512];
} service_io_t;
#define FLAG_PRESTARTED 1
#define FLAG_AUTO_CONNECT 2

/* Connections parameters */
struct connection {
    TCHAR config_file[MAX_PATH];    /* Name of the config file */
    TCHAR config_name[MAX_PATH];    /* Name of the connection */
    TCHAR config_dir[MAX_PATH];     /* Path to this configs dir */
    TCHAR log_path[MAX_PATH];       /* Path to Logfile */
    TCHAR ip[16];                   /* Assigned IP address for this connection */
    int flags;                      /* AutoConnect at startup, prestarted config etc..*/
    conn_state_t state;             /* State the connection currently is in */
    int failed_psw_attempts;        /* # of failed attempts entering password(s) */
    time_t connected_since;         /* Time when the connection was established */
    proxy_t proxy_type;             /* Set during querying proxy credentials */

    struct {
        SOCKET sk;
        struct sockaddr_storage addr;
        WCHAR host[MAX_HOSTNAME];
        WCHAR port[6];
        time_t timeout;
        char password[16];
        char *saved_data;
        size_t saved_size;
        mgmt_cmd_t *cmd_queue;
        BOOL connected;             /* True, if management interface has connected */
    } manage;

    HANDLE hProcess;                /* Handle of openvpn process if directly started */
    service_io_t iserv;

    HANDLE exit_event;
    DWORD threadId;
    HWND hwndStatus;
    HMENU hMenuConn;
    int index;
    connection_t *next;
};

connection_t *
NewConnection (void);

void
DeleteConnection(connection_t *c);

connection_t *
GetConnByFile (const TCHAR *file);

connection_t *
GetConnById (int index);

/* All options used within OpenVPN GUI */
typedef struct {
    /* Array of configs to autostart */
    const TCHAR *auto_connect[MAX_AUTO_CONNECT];

    /* Connection parameters */
    connection_t *conn;             /* List of connection structures */
    int num_configs;                  /* Number of configs */

    service_state_t service_state;    /* State of the OpenVPN Service */
    int psw_attempts;                 /* Number of psw attemps to allow */
    int connectscript_timeout;        /* Connect Script execution timeout (sec) */
    int disconnectscript_timeout;     /* Disconnect Script execution timeout (sec) */
    int preconnectscript_timeout;     /* Preconnect Script execution timeout (sec) */

    /* Proxy Settings */
    proxy_source_t proxy_source;      /* Where to get proxy information from */
    proxy_t proxy_type;               /* The type of proxy to use */
    TCHAR proxy_http_address[100];    /* HTTP Proxy Address */
    TCHAR proxy_http_port[6];         /* HTTP Proxy Port */
    TCHAR proxy_socks_address[100];   /* SOCKS Proxy Address */
    TCHAR proxy_socks_port[6];        /* SOCKS Proxy Address */

    /* Registry values */
    TCHAR exe_path[MAX_PATH];
    TCHAR config_dir[MAX_PATH];
    TCHAR global_config_dir[MAX_PATH];
    TCHAR ext_string[16];
    TCHAR log_dir[MAX_PATH];
    TCHAR priority_string[64];
    TCHAR append_string[2];
    TCHAR log_viewer[MAX_PATH];
    TCHAR editor[MAX_PATH];
    TCHAR allow_edit[2];
    TCHAR allow_service[2];
    TCHAR allow_password[2];
    TCHAR allow_proxy[2];
    TCHAR silent_connection[2];
    TCHAR service_only[2];
    TCHAR show_balloon[2];
    TCHAR show_script_window[2];
    TCHAR psw_attempts_string[2];
    TCHAR disconnect_on_suspend[2];
    TCHAR connectscript_timeout_string[4];
    TCHAR disconnectscript_timeout_string[4];
    TCHAR preconnectscript_timeout_string[4];
    TCHAR ovpn_admin_group[MAX_NAME];

#ifdef DEBUG
    FILE *debug_fp;
#endif

    HWND hWnd;
    HINSTANCE hInstance;
    BOOL session_locked;
    HANDLE netcmd_semaphore;
    int mgmt_port;            /* port number offset for management interface */
} options_t;

void InitOptions(options_t *);
void ProcessCommandLine(options_t *, TCHAR *);
int CountConnState(conn_state_t);
connection_t* GetConnByManagement(SOCKET);
#endif
