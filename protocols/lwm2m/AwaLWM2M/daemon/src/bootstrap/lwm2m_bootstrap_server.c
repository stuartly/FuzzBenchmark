/************************************************************************************************************************
 Copyright (c) 2016, Imagination Technologies Limited and/or its affiliated group companies.
 All rights reserved.

 Redistribution and use in source and binary forms, with or without modification, are permitted provided that the
 following conditions are met:
     1. Redistributions of source code must retain the above copyright notice, this list of conditions and the
        following disclaimer.
     2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the
        following disclaimer in the documentation and/or other materials provided with the distribution.
     3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote
        products derived from this software without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
 USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
************************************************************************************************************************/


#include <poll.h>
#include <stdio.h>
#include <getopt.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/select.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/stat.h>
#include <dirent.h>
#include <errno.h>
#include <signal.h>
#include <time.h>

#include "awa_bootstrapd_cmdline.h"
#include "lwm2m_object_store.h"
#include "coap_abstraction.h"
#include "dtls_abstraction.h"
#include "lwm2m_core.h"
#include "lwm2m_object_defs.h"
#include "bootstrap/lwm2m_bootstrap.h"
#include "bootstrap/lwm2m_bootstrap_cert.h"
#include "bootstrap/lwm2m_bootstrap_psk.h"

#define DEFAULT_IP_ADDRESS          "0.0.0.0"
#define MAX_BOOTSTRAP_CONFIG_FILES  (4)

typedef struct
{
    char * IPAddress;
    char * InterfaceName;
    int AddressFamily;
    int CoapPort;
    const char * ConfigFiles[MAX_BOOTSTRAP_CONFIG_FILES];
    int NumConfigFiles;
    bool Secure;
    bool Daemonise;
    bool Verbose;
    char * LogFile;
    bool Version;
} Options;

static FILE * logFile;
static const char * version = VERSION; /* from Makefile */
static volatile int quit = 0;

static void PrintOptions(const Options * options);

static void CtrlCSignalHandler(int dummy)
{
    quit = 1;
}

// Fork off a daemon process, the parent will exit at this point
static void Daemonise(bool verbose)
{
    pid_t pid;

    // forkoff the parent process
    pid = fork();

    if (pid < 0)
    {
        printf("Failed to start daemon\n");
        exit(EXIT_FAILURE);
    }

    if (pid > 0)
    {
        if (verbose)
            printf("Daemon running as %d\n", pid);
        exit(EXIT_SUCCESS);
    }

    umask(0);

    // create sid for child
    if (setsid() < 0)
    {
        printf("Failed to set sid\n");
        exit(EXIT_FAILURE);
    }

    // close off standard file descriptors
    close (STDIN_FILENO);
    close (STDOUT_FILENO);
    close (STDERR_FILENO);
}

static int Bootstrap_Start(Options * options)
{
    int result = 0;

    if (options->Daemonise)
    {
        Daemonise(options->Verbose);
    }
    else
    {
        signal(SIGINT, CtrlCSignalHandler);
    }

    signal(SIGTERM, CtrlCSignalHandler);

    // open log files here
    if (options->LogFile)
    {
        errno = 0;
        logFile = fopen(options->LogFile, "at");
        if (logFile != NULL)
        {
            Lwm2m_SetOutput(logFile);

            // redirect stdout
            dup2(fileno(logFile), STDOUT_FILENO);
        }
        else
        {
            Lwm2m_Error("Failed to open log file %s: %s\n", options->LogFile, strerror(errno));
        }
    }

    if (options->Version)
    {
        Lwm2m_Printf(0, "%s\n", version);
        goto error_close_log;
    }

    Lwm2m_SetLogLevel((options->Verbose) ? DebugLevel_Debug : DebugLevel_Info);
    Lwm2m_PrintBanner();
    if (options->Verbose)
    {
        PrintOptions(options);
    }
    Lwm2m_Info("Awa LWM2M Bootstrap Server, version %s\n", version);
    Lwm2m_Info("  Process ID     : %d\n", getpid());
    Lwm2m_Info("  DTLS library   : %s\n", DTLS_LibraryName);
    Lwm2m_Info("  CoAP library   : %s\n", coap_LibraryName);
    Lwm2m_Info("  CoAP port      : %d\n", options->CoapPort);
    Lwm2m_Info("  CoAP Security  : %s\n", options->Secure ? "DTLS": "None");

    if (options->InterfaceName != NULL)
    {
        Lwm2m_Info("  Interface      : %s [IPv%d]\n", options->InterfaceName, options->AddressFamily == AF_INET ? 4 : 6);
    }
    else if (strcmp(DEFAULT_IP_ADDRESS, options->IPAddress) != 0)
    {
        Lwm2m_Info("  IP Address     : %s\n", options->IPAddress);
    }

    char ipAddress[NI_MAXHOST];
    if (options->InterfaceName != NULL)
    {
        if (Lwm2mCore_GetIPAddressFromInterface(options->InterfaceName, options->AddressFamily, ipAddress, sizeof(ipAddress)) != 0)
        {
            result = 1;
            goto error_close_log;
        }
        Lwm2m_Info("  Interface Addr : %s\n", ipAddress);
    }
    else
    {
        strncpy(ipAddress, options->IPAddress, NI_MAXHOST);
        ipAddress[NI_MAXHOST - 1] = '\0';  // Defensive
    }

    srandom((int)time(NULL)*getpid());

    CoapInfo * coap = coap_Init(ipAddress, options->CoapPort, options->Secure, (options->Verbose) ? DebugLevel_Debug : DebugLevel_Info);
    if (coap == NULL)
    {
        printf("Unable to map address to network interface\n");
        result = 1;
        goto error_close_log;
    }

    if (options->Secure)
    {
        coap_SetCertificate(bootsrapCert, sizeof(bootsrapCert), AwaCertificateFormat_PEM);
        coap_SetPSK(pskIdentity, pskKey, sizeof(pskKey));
    }

    Lwm2mContextType * context = Lwm2mCore_Init(coap);

    // must happen after coap_Init()
    Lwm2m_RegisterObjectTypes(context);

    if (!Lwm2mBootstrap_BootStrapInit(context, options->ConfigFiles, options->NumConfigFiles))
    {
        printf("Failed to initialise bootstrap\n");
        result = 1;
        goto error_destroy;
    }

    // wait for messages on both the IPC and CoAP interfaces
    while (!quit)
    {
        int loop_result;
        struct pollfd fds[1];
        int nfds = 1;
        int timeout;

        fds[0].fd = coap->fd;
        fds[0].events = POLLIN;

        timeout = Lwm2mCore_Process(context);

        loop_result = poll(fds, nfds, timeout);

        if (loop_result < 0)
        {
            if (errno == EINTR)
            {
                continue;
            }

            perror("poll:");
            break;
        }
        else if (loop_result > 0)
        {
            if (fds[0].revents == POLLIN)
            {
                coap_HandleMessage();
            }
        }
        coap_Process();
    }
    Lwm2m_Debug("Exit triggered\n");

error_destroy:
    Lwm2mBootstrap_Destroy();
    Lwm2mCore_Destroy(context);
    coap_Destroy();

error_close_log:
    Lwm2m_Info("Bootstrap Server exiting\n");
    if (logFile != NULL)
    {
        fclose(logFile);
    }

    return result;
}

static void PrintOptions(const Options * options)
{
    printf("Options provided:\n");
    printf("  IPAddress      (--ip)             : %s\n", options->IPAddress);
    printf("  InterfaceName  (--interface)      : %s\n", options->InterfaceName ? options->InterfaceName : "");
    printf("  AddressFamily  (--addressFamily)  : %d\n", options->AddressFamily == AF_INET? 4 : 6);
    printf("  Port           (--port)           : %d\n", options->CoapPort);
    int i;
    for (i = 0; i < options->NumConfigFiles; ++i)
    {
        printf("  Config         (--config)         : %s\n", options->ConfigFiles[i]);
    }
    printf("  Secure         (--secure)         : %d\n", options->Secure);
    printf("  Daemonize      (--daemonize)      : %d\n", options->Daemonise);
    printf("  Verbose        (--verbose)        : %d\n", options->Verbose);
    printf("  LogFile        (--logFile)        : %s\n", options->LogFile ? options->LogFile : "");
    printf("  Version        (--version)        : %d\n", options->Version);
}

static int ParseOptions(int argc, char ** argv, struct gengetopt_args_info * ai, Options * options)
{
    int result = EXIT_SUCCESS;
    if (cmdline_parser(argc, argv, ai) == 0)
    {
        options->IPAddress = ai->ip_arg;
        options->InterfaceName = ai->interface_arg;
        options->AddressFamily = ai->addressFamily_arg == 4 ? AF_INET : AF_INET6;
        options->CoapPort = ai->port_arg;
        int i;
        for (i = 0; i < ai->config_given; ++i)
        {
            options->ConfigFiles[i] = ai->config_arg[i];
        }
        options->NumConfigFiles = ai->config_given;
        options->Secure = ai->secure_flag;
        options->Daemonise = ai->daemonize_flag;
        options->Verbose = ai->verbose_flag;
        options->LogFile = ai->logFile_arg;
        options->Version = ai->version_flag;

        if (options->Secure && strcmp(DTLS_LibraryName, "None") == 0)
        {
            printf("Error: not built with DTLS support\n\n");
            result = EXIT_FAILURE;
        }
    }
    else
    {
        result = EXIT_FAILURE;
    }
    return result;
}

int main(int argc, char ** argv)
{
    int result = EXIT_FAILURE;
    struct gengetopt_args_info ai;
    Options options =
    {
        .IPAddress = NULL,
        .InterfaceName = NULL,
        .AddressFamily = AF_UNSPEC,
        .CoapPort = 0,
        .ConfigFiles = {0},
        .NumConfigFiles = 0,
        .Secure = false,
        .Daemonise = false,
        .Verbose = false,
        .LogFile = NULL,
        .Version = false,
    };

    if (ParseOptions(argc, argv, &ai, &options) == EXIT_SUCCESS)
    {
        result = Bootstrap_Start(&options);
    }
    cmdline_parser_free(&ai);
    exit(result);
}

