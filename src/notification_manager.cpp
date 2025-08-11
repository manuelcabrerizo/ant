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

     instance.registries.Init(NOTIFICATION_TYPE_COUNT, stackNum);
     instance.registries.size = NOTIFICATION_TYPE_COUNT;
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

    auto& reg = registries[type];
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

    auto& reg = registries[type];
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

void NotificationManager::SendNotification(NotificationType type, void *data, size_t size, void *sender)
{
    auto& reg = registries[type];
    NotificationNode* node = reg.firstNode;
    while (node != nullptr)
    {
        node->listener->OnNotify(type, data, size, sender);
        node = node->next;
    }
}

NotificationNode* NotificationManager::GetNode(INotificable* listener, NotificationType type) const
{
    auto& reg = registries[type];
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
    auto& reg = registries[type];
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
