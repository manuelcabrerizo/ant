#pragma once

#define MAX_LISTENERS_PER_NOTIFICATION 10

enum NotificationType
{
     NOTIFICATION_PLAYER_MOVE = 0,
     NOTIFICATION_SHOOT,
     
     NOTIFICATION_TYPE_COUNT
};

struct Notification
{
     u8 data[256];
};

// TODO: find a way to note use and interface for this.
// try to use function pointers intead
class INotificable
{
public:
     virtual ~INotificable() {}
     virtual void OnNotify(NotificationType type, Notification notification, void *sender) = 0;
};


struct NotificationRegistry
{
     // TODO: see if its no to hard to use slotmaps here
     // so we can remover listeners if we want
     Array<INotificable *> listeners;
};

class NotificationManager
{
private:
     static NotificationManager instance;
     static bool initialize;
     
     Array<NotificationRegistry> registries;
public:

     static void Init(i32 stackNum);
     static void Terminate();
     static NotificationManager *Get();

     void RegisterListener(INotificable *listener, NotificationType type);
     
     void SendNotification(Notification notification, NotificationType type, void *sender);
};
