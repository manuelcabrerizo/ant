#pragma once

#define MAX_LISTENERS_PER_NOTIFICATION 10

enum NotificationType
{
     NOTIFICATION_SET_POSITION = 0,
     NOTIFICATION_SET_DIRECTION,
     NOTIFICATION_SHOOT,
     
     NOTIFICATION_TYPE_COUNT
};

struct Notification
{
     u8 data[256];
};

class INotificable
{
public:
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
