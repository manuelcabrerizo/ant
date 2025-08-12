#include "notification_manager.h"
#include <memory.h>

NotificationManager NotificationManager::instance;
bool NotificationManager::initialize = false;
     
void NotificationManager::Init(i32 stackNum)
{
     if(initialize)
     {
          ASSERT(!"Error: notification manager already initialize");
     }
     memset(&instance, 0, sizeof(NotificationManager));

     instance.registries.Init((unsigned int)NotificationType::Count, stackNum);
     instance.registries.size = (unsigned int)NotificationType::Count;
     instance.allocator.Init(stackNum);

     initialize = true;
}

void NotificationManager::Terminate()
{
     if(!initialize)
     {
          ASSERT(!"Error: notification manager has not been initialize");
     }     
     initialize = false;
}

NotificationManager *NotificationManager::Get()
{
     if(!initialize)
     {
          ASSERT(!"Error: notification manager has not been initialize");
     }
     return &instance;
}

void NotificationManager::AddListener(INotificable *listener, NotificationType type)
{
    ASSERT(GetNode(listener, type) == nullptr);

    NotificationNode* node = allocator.Alloc();
    node->listener = listener;

    auto& reg = registries[(unsigned int)type];
    if (reg.firstNode == nullptr)
    {
        node->next = nullptr;
        reg.firstNode = node;
    }
    else
    {
        node->next = reg.firstNode;
        reg.firstNode = node;
    }
}

void NotificationManager::RemoveListener(INotificable *listener, NotificationType type)
{
    NotificationNode* prev = GetPrev(listener, type);
    NotificationNode* node;

    auto& reg = registries[(unsigned int)type];
    if (prev)
    {
        node = prev->next;
    }
    else
    {
        node = reg.firstNode;
    }

    ASSERT(node != nullptr);

    if (prev)
    {
        prev->next = node->next;
    }
    else
    {
        reg.firstNode = node->next;
    }
    allocator.Free(node);
}

void NotificationManager::SendNotification(NotificationType type, Notification *notification)
{
    auto& reg = registries[(unsigned int)type];
    NotificationNode* node = reg.firstNode;
    while (node != nullptr)
    {
        node->listener->OnNotify(type, notification);
        node = node->next;
    }
}

NotificationNode* NotificationManager::GetNode(INotificable* listener, NotificationType type) const
{
    auto& reg = registries[(unsigned int)type];
    NotificationNode* node = reg.firstNode;
    while (node != nullptr)
    {
        if (node->listener == listener)
        {
            return node;
        }
        node = node->next;
    }
    return nullptr;
}

NotificationNode* NotificationManager::GetPrev(INotificable* listener, NotificationType type) const
{
    auto& reg = registries[(unsigned int)type];
    NotificationNode* node = reg.firstNode;
    while (node != nullptr)
    {
        if (node->next && node->next->listener == listener)
        {
            return node;
        }
        node = node->next;
    }
    return nullptr;
}
