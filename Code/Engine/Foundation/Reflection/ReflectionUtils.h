#pragma once

#include <Foundation/Reflection/Reflection.h>
#include <Foundation/IO/JSONWriter.h>

class ezJSONWriter;

/// \brief Helper functions for handling reflection related operations.
class EZ_FOUNDATION_DLL ezReflectionUtils
{
public:
  static ezVariant GetMemberPropertyValue(const ezAbstractMemberProperty* pProp, const void* pObject); // [tested] via ToolsFoundation 
  static void SetMemberPropertyValue(ezAbstractMemberProperty* pProp, void* pObject, const ezVariant& value); // [tested] via ToolsFoundation 

  static ezAbstractMemberProperty* GetMemberProperty(const ezRTTI* pRtti, ezUInt32 uiPropertyIndex);
  static ezAbstractMemberProperty* GetMemberProperty(const ezRTTI* pRtti, const char* szPropertyName); // [tested] via ToolsFoundation 

  /// \brief Writes all property values of the reflected \a pObject of type \a pRtti to \a stream in (extended) JSON format.
  ///
  /// Using ReadObjectPropertiesFromJSON() you can read those properties back into an existing object.
  /// Using ReadObjectFromJSON() an object of the same type is allocated an its properties are restored from the JSON data.
  ///
  /// Non-existing objects (pObject == nullptr) are stored as objects of type "null".
  /// The whitespace mode should be set according to whether the JSON data is used for interchange with other code only,
  /// or might also be read by humans.
  ///
  /// Read-only properties are not written out, as they cannot be restored anyway.
  static void WriteObjectToJSON(ezStreamWriterBase& stream, const ezRTTI* pRtti, const void* pObject, ezJSONWriter::WhitespaceMode::Enum = ezJSONWriter::WhitespaceMode::NewlinesOnly);

  /// \brief Allocator callback to create an object of the given ezRTTI type.
  typedef ezDelegate<void* (const ezRTTI& rtti)> TypeAllocator;

  /// \brief Reads the entire JSON data in the stream and restores a reflected object.
  ///
  /// The object type is read from the JSON information in the stream and the object is either allocated through the given allocator,
  /// or, if none is provided, the default allocator for the type is used.
  ///
  /// All properties are set to the values as described in the JSON data, as long as the properties can be matched to the runtime type.
  static void* ReadObjectFromJSON(ezStreamReaderBase& stream, const ezRTTI*& pRtti, TypeAllocator Allocator = TypeAllocator());

  /// \brief Reads the entire JSON data in the stream and sets all properties of the given object.
  ///
  /// All properties are set to the values as described in the JSON data, as long as the properties can be matched to the runtime type.
  /// The given object should ideally be of the same type as the object had that was written to the stream. However, if the types do
  /// not match or the properties have changed, the data will still be restored as good as possible.
  ///
  /// The object itself will not be reset to the default state before the properties are set, so properties that do not appear
  /// in the JSON data, or cannot be matched, will not be affected.
  static void ReadObjectPropertiesFromJSON(ezStreamReaderBase& stream, const ezRTTI& rtti, void* pObject);

};
