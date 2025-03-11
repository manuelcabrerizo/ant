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

void NotificationManager::RegisterListener(INotificable *listener, NotificationType type)
{
     registries[type].listeners.Push(listener);
}
     
void NotificationManager::SendNotification(Notification notification, NotificationType type, void *sender)
{
     Array<INotificable *>& listeners = registries[type].listeners;
     for(i32 i = 0; i < listeners.size; ++i)
     {
          listeners[i]->OnNotify(type, notification, sender);
     }
}
