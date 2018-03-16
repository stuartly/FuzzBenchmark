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

/*
 * This example demonstrates how to obtain a list of all registered clients from
 * the server using a LIST_CLIENTS operation. In addition, the list of registered
 * objects and object instances is extracted from the response.
 */

#include <stdlib.h>
#include <stdio.h>

#include <awa/common.h>
#include <awa/server.h>

#define IPC_PORT (54321)
#define IPC_ADDRESS "127.0.0.2"
#define OPERATION_PERFORM_TIMEOUT 1000

int main(void)
{
    /* Create and initialise server session */
    AwaServerSession * session = AwaServerSession_New();

    /* Use default IPC configuration */
    AwaServerSession_Connect(session);

    /* Create LIST_CLIENTS operation */
    AwaServerListClientsOperation * operation = AwaServerListClientsOperation_New(session);

    /* Perform the operation, resulting in a response for each registered client */
    AwaServerListClientsOperation_Perform(operation, OPERATION_PERFORM_TIMEOUT);

    /*
     * Iterate through each client in the response, extracting the client's
     * endpoint name and registered objects
     */
    AwaClientIterator * clientIterator = AwaServerListClientsOperation_NewClientIterator(operation);
    while (AwaClientIterator_Next(clientIterator))
    {
        const char * clientID = AwaClientIterator_GetClientID(clientIterator);
        printf("%s :", clientID);

        const AwaServerListClientsResponse * response = AwaServerListClientsOperation_GetResponse(operation, clientID);
        AwaRegisteredEntityIterator * entityIterator = AwaServerListClientsResponse_NewRegisteredEntityIterator(response);
        while (AwaRegisteredEntityIterator_Next(entityIterator))
        {
            printf(" %s", AwaRegisteredEntityIterator_GetPath(entityIterator));
        }
        printf("\n");

        /* Iterators must be freed after use */
        AwaRegisteredEntityIterator_Free(&entityIterator);
    }

    /* Iterators must be freed after use */
    AwaClientIterator_Free(&clientIterator);

    /* Operations must be freed after use */
    AwaServerListClientsOperation_Free(&operation);

    AwaServerSession_Disconnect(session);
    AwaServerSession_Free(&session);
    return 0;
}

