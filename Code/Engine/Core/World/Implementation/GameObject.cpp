#include <Core/PCH.h>
#include <Core/World/World.h>

EZ_BEGIN_STATIC_REFLECTED_TYPE(ezGameObject, ezNoBase, ezRTTINoAllocator);
  EZ_BEGIN_PROPERTIES
    EZ_ACCESSOR_PROPERTY("Name", GetName, SetName),
    EZ_ACCESSOR_PROPERTY("Position", GetLocalPosition, SetLocalPosition),
    EZ_ACCESSOR_PROPERTY("Rotation", GetLocalRotation, SetLocalRotation),
    EZ_ACCESSOR_PROPERTY("Scaling", GetLocalScaling, SetLocalScaling),
  EZ_END_PROPERTIES
EZ_END_STATIC_REFLECTED_TYPE();

void ezGameObject::ChildIterator::Next()
{
  m_pObject = m_pObject->m_pWorld->GetObjectUnchecked(m_pObject->m_NextSiblingIndex);
}

void ezGameObject::operator=(const ezGameObject& other)
{
  EZ_ASSERT(m_pWorld == other.m_pWorld, "Cannot copy between worlds.");

  m_InternalId = other.m_InternalId;
  m_Flags = other.m_Flags;

  m_ParentIndex = other.m_ParentIndex;
  m_FirstChildIndex = other.m_FirstChildIndex;
  m_LastChildIndex = other.m_LastChildIndex;
 
  m_NextSiblingIndex = other.m_NextSiblingIndex;
  m_PrevSiblingIndex = other.m_PrevSiblingIndex;
  m_ChildCount = other.m_ChildCount;

  if (m_pTransformationData != other.m_pTransformationData)
  {
    m_uiHierarchyLevel = other.m_uiHierarchyLevel;
    m_uiTransformationDataIndex = other.m_uiTransformationDataIndex;  
    ezMemoryUtils::Copy(m_pTransformationData, other.m_pTransformationData, 1);
  }

  m_pTransformationData->m_pObject = this;

  m_Components = other.m_Components;
  for (ezUInt32 i = 0; i < m_Components.GetCount(); ++i)
  {
    ezComponent* pComponent = m_Components[i];
    EZ_ASSERT(pComponent->m_pOwner == &other, "");
    pComponent->m_pOwner = this;
  }
}

void ezGameObject::Activate()
{
  m_Flags.Add(ezObjectFlags::Active);

  for (ezUInt32 i = 0; i < m_Components.GetCount(); ++i)
  {
    m_Components[i]->Activate();
  }
}

void ezGameObject::Deactivate()
{
  m_Flags.Remove(ezObjectFlags::Active);

  for (ezUInt32 i = 0; i < m_Components.GetCount(); ++i)
  {
    m_Components[i]->Deactivate();
  }
}

void ezGameObject::SetParent(const ezGameObjectHandle& parent)
{
  ezGameObject* pParent = nullptr;
  m_pWorld->TryGetObject(parent, pParent);
  m_pWorld->SetParent(this, pParent);
}

ezGameObject* ezGameObject::GetParent() const
{
  return m_pWorld->GetObjectUnchecked(m_ParentIndex);
}

void ezGameObject::AddChild(const ezGameObjectHandle& child)
{
  ezGameObject* pChild = nullptr;
  if (m_pWorld->TryGetObject(child, pChild))
  {
    m_pWorld->SetParent(pChild, this);
  }
}

void ezGameObject::DetachChild(const ezGameObjectHandle& child)
{
  ezGameObject* pChild = nullptr;
  if (m_pWorld->TryGetObject(child, pChild))
  {
    if (pChild->GetParent() == this)
    {
      m_pWorld->SetParent(pChild, nullptr);
    }
  }
}

ezGameObject::ChildIterator ezGameObject::GetChildren() const
{
  return ChildIterator(m_pWorld->GetObjectUnchecked(m_FirstChildIndex));
}

ezResult ezGameObject::AddComponent(const ezComponentHandle& component)
{
  ezComponent* pComponent = nullptr;
  if (m_pWorld->TryGetComponent(component, pComponent))
  {
    return AddComponent(pComponent);
  }

  return EZ_FAILURE;
}

ezResult ezGameObject::AddComponent(ezComponent* pComponent)
{
  EZ_ASSERT(pComponent->IsInitialized(), "Component must be initialized.");
  EZ_ASSERT(pComponent->m_pOwner == nullptr, "Component must not be added twice.");
  EZ_ASSERT(IsDynamic() || !pComponent->IsDynamic(),
    "Cannot attach a dynamic component to a static object. Call MakeDynamic() first.");

  pComponent->m_pOwner = this;
  if (pComponent->OnAttachedToObject() == EZ_SUCCESS)
  {
    m_Components.PushBack(pComponent);
    return EZ_SUCCESS;
  }

  pComponent->m_pOwner = nullptr;
  return EZ_FAILURE;
}

ezResult ezGameObject::RemoveComponent(const ezComponentHandle& component)
{
  ezComponent* pComponent = nullptr;
  if (m_pWorld->TryGetComponent(component, pComponent))
  {
    return RemoveComponent(pComponent);
  }  
  
  return EZ_FAILURE;
}

ezResult ezGameObject::RemoveComponent(ezComponent* pComponent)
{
  EZ_ASSERT(pComponent->IsInitialized(), "Component must be initialized.");

  ezUInt32 uiIndex = m_Components.IndexOf(pComponent);
  if (uiIndex == ezInvalidIndex)
    return EZ_FAILURE;

  if (pComponent->OnDetachedFromObject() == EZ_SUCCESS)
  {
    pComponent->m_pOwner = nullptr;

    m_Components.RemoveAtSwap(uiIndex);
    return EZ_SUCCESS;
  }

  return EZ_FAILURE;
}

void ezGameObject::FixComponentPointer(ezComponent* pOldPtr, ezComponent* pNewPtr)
{
  ezUInt32 uiIndex = m_Components.IndexOf(pOldPtr);
  EZ_ASSERT(uiIndex != ezInvalidIndex, "Memory corruption?");
  m_Components[uiIndex] = pNewPtr;
}

void ezGameObject::PostMessage(ezMessage& msg, ezObjectMsgQueueType::Enum queueType,
  ezObjectMsgRouting::Enum routing)
{
  m_pWorld->PostMessage(GetHandle(), msg, queueType, routing);
}

void ezGameObject::PostMessage(ezMessage& msg, ezObjectMsgQueueType::Enum queueType, ezTime delay,
  ezObjectMsgRouting::Enum routing)
{
  m_pWorld->PostMessage(GetHandle(), msg, queueType, delay, routing);
}

void ezGameObject::OnMessage(ezMessage& msg, ezObjectMsgRouting::Enum routing)
{
  if (routing == ezObjectMsgRouting::ToSubTree)
  {
    // walk up the sub tree and send to children form there to prevent double handling
    ezGameObject* pCurrent = this;
    ezGameObject* pParent = GetParent();
    while (pParent != nullptr)
    {
      pCurrent = pParent;
      pParent = pCurrent->GetParent();
    }

    pCurrent->OnMessage(msg, ezObjectMsgRouting::ToChildren);
    return;
  }

  // always send message to all components
  for (ezUInt32 i = 0; i < m_Components.GetCount(); ++i)
  {
    m_Components[i]->OnMessage(msg);
  }

  // route message to parent or children
  if (routing == ezObjectMsgRouting::ToParent)
  {
    if (ezGameObject* pParent = GetParent())
    {
      pParent->OnMessage(msg, routing);
    }
  }
  else if (routing == ezObjectMsgRouting::ToChildren)
  {
    for (ChildIterator it = GetChildren(); it.IsValid(); ++it)
    {
      it->OnMessage(msg, routing);
    }
  }
}


EZ_STATICLINK_FILE(Core, Core_World_Implementation_GameObject);

