
#define EZ_CHECK_CLASS(T) \
  EZ_CHECK_AT_COMPILETIME_MSG(!std::is_trivial<T>::value, "Pod type is treated as class, did you forget EZ_DECLARE_POD_TYPE?")

// public methods: redirect to implementation
template <typename T>
EZ_FORCE_INLINE void ezMemoryUtils::Construct(T* pDestination, size_t uiCount)
{
  // Default constructor is always called, so that debug helper initializations (e.g. ezVec3 initializes to NaN) take place. 
  // Note that destructor is ONLY called for class types.
  // Special case for c++11 to prevent default construction of "real" Pod types, also avoids warnings on msvc
  Construct(pDestination, uiCount, ezTraitInt<ezIsPodType<T>::value && std::is_trivial<T>::value>());
}

template <typename T>
EZ_FORCE_INLINE void ezMemoryUtils::Construct(T* pDestination, const T& copy, size_t uiCount)
{
  Construct(pDestination, copy, uiCount, ezIsPodType<T>());
}

template <typename T>
EZ_FORCE_INLINE void ezMemoryUtils::Construct(T* pDestination, const T* pSource, size_t uiCount)
{
  Construct(pDestination, pSource, uiCount, ezIsPodType<T>());
}

template <typename T>
EZ_FORCE_INLINE void ezMemoryUtils::Destruct(T* pDestination, size_t uiCount)
{
  Destruct(pDestination, uiCount, ezIsPodType<T>());
}

template <typename T>
EZ_FORCE_INLINE void ezMemoryUtils::DefaultConstruct(T* pDestination, size_t uiCount)
{
  for (size_t i = 0; i < uiCount; i++)
  {
    ::new (pDestination + i) T();
  }
}

template <typename T>
EZ_FORCE_INLINE void ezMemoryUtils::Copy(T* pDestination, const T* pSource, size_t uiCount)
{
  EZ_ASSERT(pDestination < pSource || pSource + uiCount <= pDestination, "Memory regions must not overlap when using Copy. Use Move instead.");
  Copy(pDestination, pSource, uiCount, ezIsPodType<T>());
}

template <typename T>
EZ_FORCE_INLINE void ezMemoryUtils::Move(T* pDestination, const T* pSource, size_t uiCount)
{
  Move(pDestination, pSource, uiCount, ezIsPodType<T>());
}

template <typename T>
EZ_FORCE_INLINE bool ezMemoryUtils::IsEqual(const T* a, const T* b, size_t uiCount /*= 1*/)
{
  return IsEqual(a, b, uiCount, ezIsPodType<T>());
}

template <typename T>
EZ_FORCE_INLINE void ezMemoryUtils::ZeroFill(T* pDestination, size_t uiCount /*= 1*/)
{
  memset(pDestination, 0, uiCount * sizeof(T));
}

template <typename T>
EZ_FORCE_INLINE ezInt32 ezMemoryUtils::ByteCompare(const T* a, const T* b, size_t uiCount /*= 1*/)
{
  return memcmp(a, b, uiCount * sizeof(T));
}

template <typename T>
EZ_FORCE_INLINE T* ezMemoryUtils::AddByteOffset(T* ptr, ptrdiff_t iOffset)
{
  return reinterpret_cast<T*>(reinterpret_cast<ezUInt8*>(ptr) + iOffset);
}

template <typename T>
EZ_FORCE_INLINE const T* ezMemoryUtils::AddByteOffsetConst(const T* ptr, ptrdiff_t iOffset)
{
  return reinterpret_cast<const T*>(reinterpret_cast<const ezUInt8*>(ptr) + iOffset);
}

template <typename T>
EZ_FORCE_INLINE T* ezMemoryUtils::Align(T* ptr, size_t uiAlignment)
{
  return reinterpret_cast<T*>(reinterpret_cast<size_t>(ptr) & ~(uiAlignment - 1));
}

template <typename T>
EZ_FORCE_INLINE T ezMemoryUtils::AlignSize(T uiSize, T uiAlignment)
{
  return ((uiSize + (uiAlignment - 1)) & ~(uiAlignment - 1));
}

template <typename T>
EZ_FORCE_INLINE bool ezMemoryUtils::IsAligned(const T* ptr, size_t uiAlignment)
{
  return (reinterpret_cast<size_t>(ptr) & (uiAlignment - 1)) == 0;
}


// private methods

template <typename T>
EZ_FORCE_INLINE void ezMemoryUtils::Construct(T* pDestination, size_t uiCount, ezTypeIsPod)
{
  EZ_CHECK_AT_COMPILETIME_MSG(std::is_trivial<T>::value, "This method should only be called for 'real' pod types");
}

template <typename T>
EZ_FORCE_INLINE void ezMemoryUtils::Construct(T* pDestination, size_t uiCount, ezTypeIsClass)
{
  EZ_CHECK_CLASS(T);

  for (size_t i = 0; i < uiCount; i++)
  {
    ::new (pDestination + i) T();
  }
}

template <typename T>
EZ_FORCE_INLINE void ezMemoryUtils::Construct(T* pDestination, const T& copy, size_t uiCount, ezTypeIsPod)
{
  for (size_t i = 0; i < uiCount; i++)
  {
    pDestination[i] = copy;
  }
}

template <typename T>
EZ_FORCE_INLINE void ezMemoryUtils::Construct(T* pDestination, const T& copy, size_t uiCount, ezTypeIsClass)
{
  EZ_CHECK_CLASS(T);

  for (size_t i = 0; i < uiCount; i++)
  {
    ::new (pDestination + i) T(copy);
  }
}

template <typename T>
EZ_FORCE_INLINE void ezMemoryUtils::Construct(T* pDestination, const T* pSource, size_t uiCount, ezTypeIsPod)
{
  memcpy(pDestination, pSource, uiCount * sizeof(T));
}

template <typename T>
EZ_FORCE_INLINE void ezMemoryUtils::Construct(T* pDestination, const T* pSource, size_t uiCount, ezTypeIsClass)
{
  EZ_CHECK_CLASS(T);

  for (size_t i = 0; i < uiCount; i++)
  {
    ::new (pDestination + i) T(pSource[i]);
  }
}

template <typename T>
EZ_FORCE_INLINE void ezMemoryUtils::Destruct(T* pDestination, size_t uiCount, ezTypeIsPod)
{
  // Nothing to do here. See Construct of for more info.
}

template <typename T>
EZ_FORCE_INLINE void ezMemoryUtils::Destruct(T* pDestination, size_t uiCount, ezTypeIsClass)
{
  EZ_CHECK_CLASS(T);

  for (size_t i = uiCount; i-- > 0; )
  {
    pDestination[i].~T();
  }
}

template <typename T>
EZ_FORCE_INLINE void ezMemoryUtils::Copy(T* pDestination, const T* pSource, size_t uiCount, ezTypeIsPod)
{
  memcpy(pDestination, pSource, uiCount * sizeof(T));
}

template <typename T>
EZ_FORCE_INLINE void ezMemoryUtils::Copy(T* pDestination, const T* pSource, size_t uiCount, ezTypeIsClass)
{
  EZ_CHECK_CLASS(T);

  for (size_t i = 0; i < uiCount; i++)
  {
    pDestination[i] = pSource[i];
  }
}

template <typename T>
EZ_FORCE_INLINE void ezMemoryUtils::Move(T* pDestination, const T* pSource, size_t uiCount, ezTypeIsPod)
{
  memmove(pDestination, pSource, uiCount * sizeof(T));
}

template <typename T>
inline void ezMemoryUtils::Move(T* pDestination, const T* pSource, size_t uiCount, ezTypeIsClass)
{
  EZ_CHECK_CLASS(T);

  if (pDestination == pSource)
    return;

  if (pDestination < pSource)
  {
    for (size_t i = 0; i < uiCount; i++)
    {
      pDestination[i] = pSource[i];
    }
  }
  else
  {
    for (size_t i = uiCount; i-- > 0; )
    {
      pDestination[i] = pSource[i];
    }
  }
}

template <typename T>
EZ_FORCE_INLINE bool ezMemoryUtils::IsEqual(const T* a, const T* b, size_t uiCount, ezTypeIsPod)
{
  return memcmp(a, b, uiCount * sizeof(T)) == 0;
}

template <typename T>
EZ_FORCE_INLINE bool ezMemoryUtils::IsEqual(const T* a, const T* b, size_t uiCount, ezTypeIsClass)
{
  EZ_CHECK_CLASS(T);

  for (size_t i = 0; i < uiCount; i++)
  {
    if (!(a[i] == b[i]))
      return false;
  }
  return true;
}

#undef EZ_CHECK_CLASS

