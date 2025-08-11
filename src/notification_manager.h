#pragma once

#include <containers.h>

enum NotificationType
{
     NOTIFICATION_PLAYER_MOVE = 0,
     NOTIFICATION_SHOOT,
     NOTIFICATION_ON_RESIZE,
     
     NOTIFICATION_TYPE_COUNT
};

class INotificable
{
public:
     virtual ~INotificable() {}
     virtual void OnNotify(NotificationType type, void *data, size_t size, void *sender) = 0;
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
     void SendNotification(NotificationType type, void *data, size_t size, void *sender);
};
