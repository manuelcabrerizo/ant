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

     for(i32 i = 0; i < instance.registries.size; i++)
     {
          instance.registries[i].listeners.Init(MAX_LISTENERS_PER_NOTIFICATION, stackNum);
     }

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

SlotmapKey<INotificable *> NotificationManager::AddListener(INotificable *listener, NotificationType type)
{
     return registries[type].listeners.Add(listener);
}

void NotificationManager::RemoveListener(SlotmapKey<INotificable *> listenerKey, NotificationType type)
{
     return registries[type].listeners.Remove(listenerKey);
}
     
void NotificationManager::SendNotification(NotificationType type, void *data, size_t size, void *sender)
{
     Array<INotificable *> *listeners = registries[type].listeners.GetArray();;
     for(i32 i = 0; i < listeners->size; ++i)
     {
          (*listeners)[i]->OnNotify(type, data, size, sender);
     }
}
