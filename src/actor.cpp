#include "actor.h"

const char *tagNames[(int)ActorTag::TagCount] =
{
    "Default",
    "Player",
    "Enemy"
};

ActorTag Actor::TagNameToTag(const char* tagName)
{
    for (int i = 0; i < (int)ActorTag::TagCount; i++)
    {
        if (strcmp(tagName, tagNames[i]) == 0)
        {
            return (ActorTag)i;
        }
    }
}

void Actor::SetTag(ActorTag tag)
{
    this->tag = tag;
}

ActorTag Actor::GetTag()
{
    return tag;
}
