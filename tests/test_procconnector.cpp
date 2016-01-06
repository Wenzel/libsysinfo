#include <stdio.h>

#include "procconnector.h"

void handler(struct proc_event event)
{
    switch (event.what) {
    case event.PROC_EVENT_NONE:
        break;
    case event.PROC_EVENT_FORK:
        printf("fork: parent tid=%d pid=%d -> child tid=%d pid=%d\n",
               event.event_data.fork.parent_pid,
               event.event_data.fork.parent_tgid,
               event.event_data.fork.child_pid,
               event.event_data.fork.child_tgid);
        break;
    case event.PROC_EVENT_EXEC:
        printf("exec: tid=%d pid=%d\n",
               event.event_data.exec.process_pid,
               event.event_data.exec.process_tgid);
        break;
    case event.PROC_EVENT_UID:
        printf("uid change: tid=%d pid=%d from %d to %d\n",
               event.event_data.id.process_pid,
               event.event_data.id.process_tgid,
               event.event_data.id.r.ruid,
               event.event_data.id.e.euid);
        break;
    case event.PROC_EVENT_GID:
        printf("gid change: tid=%d pid=%d from %d to %d\n",
               event.event_data.id.process_pid,
               event.event_data.id.process_tgid,
               event.event_data.id.r.rgid,
               event.event_data.id.e.egid);
        break;
    case event.PROC_EVENT_EXIT:
        printf("exit: tid=%d pid=%d exit_code=%d\n",
               event.event_data.exit.process_pid,
               event.event_data.exit.process_tgid,
               event.event_data.exit.exit_code);
        break;
    default:
        printf("unhandled proc event\n");
        break;
    }

}

int main(int argc, char* argv[])
{
    ProcConnector connector = ProcConnector();
    connector.addCallback(handler);
    connector.listen();

}
