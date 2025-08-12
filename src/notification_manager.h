#pragma once

#include <notification.h>
#include <containers.h>

class INotificable
{
public:
     virtual ~INotificable() {}
     virtual void OnNotify(NotificationType type, Notification *notification) = 0;
};

struct NotificationNode
{
    INotificable* listener;
    NotificationNode* next;
};

struct NotificationRegistry
{
    NotificationNode* firstNode = nullptr;
};

class NotificationManager
{
private:
     static NotificationManager instance;
     static bool initialize;
     
     Array<NotificationRegistry> registries;
     ObjectAllocator<NotificationNode> allocator;

     NotificationNode* GetNode(INotificable* listener, NotificationType type) const;
     NotificationNode* GetPrev(INotificable* listener, NotificationType type) const;
public:

     static void Init(i32 stackNum);
     static void Terminate();
     static NotificationManager *Get();

     void AddListener(INotificable *listener, NotificationType type);
     void RemoveListener(INotificable* listener, NotificationType type);
     void SendNotification(NotificationType type, Notification *notification);
};
