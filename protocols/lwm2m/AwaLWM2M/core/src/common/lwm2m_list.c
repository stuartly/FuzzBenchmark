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


#include "lwm2m_list.h"

void ListAdd(struct ListHead * newEntry, struct ListHead * head)
{
    struct ListHead * pos;
    struct ListHead * next = head;

    ListForEach(pos, head)
    {
        next = pos;
    }

    //next->Prev     = newEntry;
    newEntry->Next = head;
    newEntry->Prev = next;
    next->Next     = newEntry;
}


void ListInsertAfter(struct ListHead * newEntry, struct ListHead * afterEntry)
{

    if (afterEntry->Prev == afterEntry->Next)
    {
        ListAdd(newEntry, afterEntry->Next);
    }
    else
    {
        newEntry->Next = afterEntry->Next;
        newEntry->Prev = afterEntry;

        afterEntry->Next = newEntry;
        if (newEntry->Next->Prev == afterEntry)
        {
            newEntry->Next->Prev = newEntry;
        }
    }
}

void ListRemove(struct ListHead * entry)
{
    struct ListHead * next = entry->Next;
    struct ListHead * prev = entry->Prev;

    next->Prev = prev;
    prev->Next = next;

    ListInit(entry); // otherwise if we call ListRemove again we'll get a bad memory access
}

void ListInit(struct ListHead * list)
{
    list->Next = list;
    list->Prev = list;
}

int ListCount(const struct ListHead * list)
{
    int count = 0;
    struct ListHead * pos;
    ListForEach(pos, list)
    {
        count ++;
    }
    return count;
}
